#!/usr/bin/env python3
# This script validates a parameterized LTC7871 design based on some given design constraints. Mostly it just
# checks that the design is within the specified limits and feasible. Only the core buck converter is validated.

import math
from enum import Enum
from dataclasses import dataclass
from eseries import find_nearest, E96


class ILim(Enum):
    """ Enumeration of the current limit programming options for the LTC7871 Table 5 """
    ILIM_0 = 0
    ILIM_1_4 = 1
    ILIM_FLOAT = 2
    ILIM_3_4 = 3
    ILIM_FULL = 4


@dataclass
class Mosfet:
    """ N-Channel mosfet parameters for one of the switches in the buck converter """
    name: str  # Part number
    t_delta: float  # Compensation factor for temperature, per degree C
    rds_on: float  # On resistance, in ohms
    crss: float  # Reverse transfer (miller) capacitance, in farads
    i_avg: float  # Rated average current, in amperes
    vth: float  # Gate threshold voltage at desired switching current, in volts


@dataclass
class Inductor:
    """ Inductor parameters for the buck converter """
    name: str  # Part number
    l: float  # Inductance, in henries
    dcr: float  # DC resistance, in ohms
    i_sat: float  # Saturation current, in amperes
    i_avg: float  # Rated average current, in amperes


@dataclass
class Capacitor:
    """ Capacitor parameters for the buck converter """
    name: str  # Part number
    c: float  # Capacitance, in farads
    esr: float  # Equivalent series resistance, in ohms
    

@dataclass
class HybridCapacitor:
    """ Specifies a combination of ceramic and electrolytic capacitors """
    ceramic: Capacitor
    num_ceramic: int
    electrolytic: Capacitor
    num_electrolytic: int


@dataclass
class CurrentSenseFilter:
    """ Sense network configuration for the inductor current """
    r1: float  # Sense resistor 1, in ohms
    c1: float  # Sense capacitor 1, in farads
    r2: float  # Sense resistor 2, in ohms
    c2: float  # Sense capacitor 2, in farads


@dataclass
class OPP:
    """ Operational performance parameters for the buck converter """
    description: str  # Description of the operating point
    v_in: float  # Input voltage, in volts
    v_out: float  # Output voltage, in volts
    p_max: float  # Maximum output power, in watts
    phases: int  # Number of output phases driving the load
    f_sw: float  # Switching frequency, in hertz
    i_ripple: float  # Allowed output current ripple, in percent [0, 1]
    v_ripple: float  # Allowed output voltage ripple, in volts
    i_lim: ILim  # Current limit programming option
    drvcc: float = 10.0  # Gate driver supply voltage, in volts (Pin 46, must be 5v, 8v, or 10v)


# Maximum voltage sense values for a DCR type current sense resistor.
# Values are in volts.
VSense_Max_DCR = {
    ILim.ILIM_0: 0.1,
    ILim.ILIM_1_4: 0.2,
    ILim.ILIM_FLOAT: 0.3,
    ILim.ILIM_3_4: 0.4,
    ILim.ILIM_FULL: 0.5
}

VSense_VDiv_Resistors = {
    ILim.ILIM_0: (22e3, 1e3),
    ILim.ILIM_1_4: (10.2e3, 3.4e3),
    ILim.ILIM_FLOAT: (10e3, 10e3),
    ILim.ILIM_3_4: (3.4e3, 10.2e3),
    ILim.ILIM_FULL: (1e3, 22e3)
}


@dataclass
class LTC7871Parameters:
    """ Design parameters for the buck converter """
    name: str  # Design name
    mosfet: Mosfet
    inductor: Inductor
    high_side_cap: HybridCapacitor
    low_side_cap: HybridCapacitor
    i_sense_filter: CurrentSenseFilter
    opp: OPP


class LTC7871Validator:
    """ Validate a parameterized LTC7871 design """

    def __init__(self, param: LTC7871Parameters):
        self.p = param

        # Validate input parameter limits
        self.validate_absolute_limits()

        # Max output current
        self.op_i_out_max = self.p.opp.p_max / self.p.opp.v_out
        self.op_i_out_phase = self.op_i_out_max / self.p.opp.phases

        # Inductor ripple current (A)
        self.il_ripple = self.op_i_out_max * self.p.opp.i_ripple

        self._failed_assertions = []

    def validate_absolute_limits(self):
        # Switching frequency limits
        self._log_assert(60e3 <= self.p.opp.f_sw <= 750e3, "Switching frequency must be between 60 kHz and 750 kHz")

        # Minimum On-Time (pg. 29)
        ref_on_time = 150e-9  # (sec) Best case performance from datasheet
        act_on_time = self.p.opp.v_out / (self.p.opp.v_in * self.p.opp.f_sw)
        self._log_assert(ref_on_time < act_on_time, f"{ref_on_time * 1e9:.2f}nS > {act_on_time*1e9:.2f}nS")

    def validate_inductor(self):
        # Ensure VSenseMax isn't exceeded
        act_sense = self.p.inductor.dcr * (self.op_i_out_phase + (self.il_ripple / 2.0))
        max_sense = VSense_Max_DCR[self.p.opp.i_lim]
        self._log_assert(act_sense < max_sense, f"Actual voltage sense {act_sense} > maximum allowed voltage sense {max_sense}")
        print(f"Actual VSense: {act_sense:.2f}V")

        # Compute approximate equality of the output inductor characteristics and the two filter stages
        tolerance = 1e-2
        l_by_dcr = round(self.p.inductor.l / self.p.inductor.dcr, 3)
        r1_c1_5 = round(self.p.i_sense_filter.r1 * self.p.i_sense_filter.c1 * 5, 3)
        r2_c2 = round(self.p.i_sense_filter.r2 * self.p.i_sense_filter.c2, 3)

        # Compute the minimum inductor value to achieve desired ripple current. << current == lower core loss
        l_min = ((self.p.opp.v_in - self.p.opp.v_out) / (self.p.opp.f_sw * self.il_ripple)) * (self.p.opp.v_out / self.p.opp.v_in)
        print(f"Inductance: {self.p.inductor.l * 1e6:.2f}uH, Min: {l_min * 1e6:.2f}uH")
        self._log_assert(self.p.inductor.l >= l_min, f"Inductor value {self.p.inductor.l} < minimum {l_min}")

        # Validate relationship between the inductor and the two filter stages
        print(f"L/DCR: {l_by_dcr}, R1C1_5: {r1_c1_5}, R2C2: {r2_c2}")
        self._log_assert(math.isclose(l_by_dcr, r1_c1_5 / 5.0, abs_tol=tolerance), f"L/DCR {l_by_dcr} != R1C1_5 {r1_c1_5}")
        self._log_assert(math.isclose(l_by_dcr, r2_c2, abs_tol=1e-3), f"L/DCR {l_by_dcr} != R2C2 {r2_c2}")

        # Ensure the current sense input ripple voltage is less than 2mV
        v_ripple = (self.p.opp.v_out / self.p.opp.v_in) * ((self.p.opp.v_in - self.p.opp.v_out) / (self.p.i_sense_filter.r1 * self.p.i_sense_filter.c1 * self.p.opp.f_sw))
        self._log_assert(v_ripple < 10e-3, f"SNSA+, SNS- ripple voltage {v_ripple} > 10mV recommended")

        # Peak Current Rating
        il_rated = self.op_i_out_phase + self.il_ripple / 2.0
        print(f"Inductor Avg Current: {self.op_i_out_phase:.2f}A")
        print(f"Inductor Peak Current: {il_rated:.2f}A")

        # Compute copper loss
        l_i2r_loss = self.op_i_out_phase ** 2 * self.p.inductor.dcr

        # Summarize this block
        print(f"Exp SNS +/- Ripple {v_ripple * 1e3:.2f}mV")
        print(f"Inductor Avg I2R Loss: {l_i2r_loss:.2f}W")
        
    def validate_mosfet(self):
        temp_delta = self.p.mosfet.t_delta
        rds_on = self.p.mosfet.rds_on
        crss = self.p.mosfet.crss
        iavg = self.p.mosfet.i_avg
        vth = self.p.mosfet.vth
        rdr = 1.5  # Resistance of top switch driver (Ohm), from LTC7060 R_UP(TG)?

        # Junction Temp Adjustment
        est_junction_temp = 75.0  # Celsius
        ambient_temp = 25.0  # Celsius
        temp_factor = temp_delta * (est_junction_temp - ambient_temp)

        # Top Switch Mosfet Dissipation
        mos_top_1 = (self.p.opp.v_out / self.p.opp.v_in) * (self.op_i_out_phase ** 2.0) * (1.0 + temp_factor) * rds_on
        mos_top_2 = (self.p.opp.v_in ** 2.0) * (self.op_i_out_phase / 2.0) * rdr * crss
        mos_top_3 = ((1.0 / (self.p.opp.drvcc - vth)) + (1.0 / vth)) * self.p.opp.f_sw
        mos_top_pwr_dissipation = mos_top_1 + mos_top_2 * mos_top_3

        # Bottom Switch Mosfet Dissipation
        mos_bot_1 = (self.p.opp.v_in - self.p.opp.v_out) / self.p.opp.v_in
        mos_bot_2 = self.op_i_out_phase ** 2.0
        mos_bot_3 = (1.0 + temp_factor) * rds_on
        mos_bot_pwr_dissipation = mos_bot_1 * mos_bot_2 * mos_bot_3

        print(f"Dissipation Top: {mos_top_pwr_dissipation:.2f}W")
        print(f"Dissipation Bottom: {mos_bot_pwr_dissipation:.2f}W")
        
    def validate_high_side_capacitor(self):
        pass 
    
    def validate_low_side_capacitor(self):
        # Capacitor: CGA6P1X7R1N106M250AC
        # https://product.tdk.com/en/search/capacitor/ceramic/mlcc/info?part_no=CGA6P1X7R1N106M250AC
        CLow_ceramic_cap = self.p.low_side_cap.ceramic.c
        CLow_ceramic_ESR = self.p.low_side_cap.ceramic.esr
        CLow_ceramic_N = self.p.low_side_cap.num_ceramic
        _ceramic_eff_esr = CLow_ceramic_ESR / CLow_ceramic_N

        # Capacitor: 80ZLH220MEFCT810X23
        # https://www.digikey.com/en/products/detail/rubycon/80ZLH220MEFCT810X23/3568221
        CLow_electrolytic_cap = self.p.low_side_cap.electrolytic.c
        CLow_electrolytic_ESR = self.p.low_side_cap.electrolytic.esr
        CLow_electrolytic_N = self.p.low_side_cap.num_electrolytic
        _electrolytic_eff_esr = CLow_electrolytic_ESR / CLow_electrolytic_N

        # Compute effective capacitance and ESR from both ceramic and electrolytic capacitors
        CLow_eff_ESR = 1.0 / ((1.0 / _ceramic_eff_esr) + (1.0 / _electrolytic_eff_esr))
        CLow_eff = (CLow_ceramic_cap * CLow_ceramic_N) + (CLow_electrolytic_cap * CLow_electrolytic_N)

        # Validate steady state output ripple voltage isn't exceeded
        dVLow_act = self.il_ripple * (CLow_eff_ESR + (1.0 / (8.0 * self.p.opp.f_sw * CLow_eff)))
        print(f"Output voltage ripple:\t{dVLow_act * 1e3:.2f}mV, Min: {self.p.opp.v_ripple * 1e3:.2f}mV")
        self._log_assert(dVLow_act <= self.p.opp.v_ripple, f"Output voltage ripple {dVLow_act} > {self.p.opp.v_ripple}")

        # Validate minimum capacitance is met
        CLow_min_ref = 1.0 / (8.0 * self.p.opp.f_sw * self.p.inductor.dcr)
        print(f"Output capacitance:\t{CLow_eff * 1e6:.2f}uF, Min: {CLow_min_ref * 1e6:.2f}uF")
        self._log_assert(CLow_eff > CLow_min_ref, f"Output capacitance {CLow_eff} < {CLow_min_ref}")

        # TODO: Compute power dissipation in the capacitors
        
    def compute_programmable_output_voltage_range(self):
        NominalVoltageRanges = {
            # VLow: (RA, RB)
            "12v": (10e3, 100e3),
            "24v": (10e3, 160e3),
            "48v": (10e3, 325e3),
        }

        IDAC_min = -64e-6
        IDAC_max = 64e-6

        for name, (RA, RB) in NominalVoltageRanges.items():
            VLow_Nominal = 1.2 * (1.0 + (RB / RA))
            VLow_programmable_min = 1.2 * (1.0 + (RB / RA)) - (IDAC_max * RB)
            VLow_programmable_max = 1.2 * (1.0 + (RB / RA)) - (IDAC_min * RB)

            print(
                f"{name} Nominal: {VLow_Nominal:.2f}V -> Range: {VLow_programmable_min:.2f}V - {VLow_programmable_max:.2f}V")
            
    def validate(self) -> bool:
        print(f"Validating {self.p.name}")
        self.validate_inductor()
        self.validate_mosfet()
        self.validate_high_side_capacitor()
        self.validate_low_side_capacitor()
        self.compute_programmable_output_voltage_range()

        if self._failed_assertions:
            print("Validation failed:")
            for msg in self._failed_assertions:
                print(f"  {msg}")
            return False

        return True

    def _log_assert(self, condition: bool, message: str):
        if not condition:
            self._failed_assertions.append(message)
