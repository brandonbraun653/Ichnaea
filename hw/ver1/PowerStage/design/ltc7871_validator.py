#!/usr/bin/env python3
# This script validates a parameterized LTC7871 design based on some given design constraints. Mostly it just
# checks that the design is within the specified limits and feasible. Only the core buck converter is validated.

import math
from enum import Enum
from dataclasses import dataclass
from loguru import logger
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
    vds: float  # Drain-source voltage, in volts


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
    v: float  # Rated voltage, in volts
    esr: float  # Equivalent series resistance, in ohms


@dataclass
class HybridCapacitor:
    """ Specifies a combination of ceramic and electrolytic capacitors """
    ceramic: Capacitor
    num_ceramic: int
    electrolytic: Capacitor
    num_electrolytic: int
    fos_v: float = 1.5  # Factor of safety for voltage rating


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
    tj_max: float = 75.0  # Maximum junction temperature of mosfet switches, in Celsius
    t_ambient: float = 25.0  # Ambient temperature, in Celsius


# Maximum voltage sense values for a DCR type current sense resistor.
# Values are in volts.
VSense_Max_DCR = {
    ILim.ILIM_0: 10e-3,
    ILim.ILIM_1_4: 20e-3,
    ILim.ILIM_FLOAT: 30e-3,
    ILim.ILIM_3_4: 40e-3,
    ILim.ILIM_FULL: 50e-3
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
    i_filter: CurrentSenseFilter
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
        self.il_ripple_target = self.op_i_out_max * self.p.opp.i_ripple

        self._failed_assertions = []

        # Computed values
        self.total_power_dissipation = 0.0

        # Computed values per-phase
        self.l_min = 0.0
        self.l_avg_pwr_loss = 0.0
        self.l_peak_pwr_loss = 0.0
        self.l_peak_current = 0.0
        self.l_v_ripple = 0.0
        self.l_v_sense_ripple = 0.0
        self.l_v_sense_ripple_max = 0.0
        self.il_ripple_actual = 0.0
        self.il_r1_pwr_loss = 0.0
        self.m_top_pwr_loss = 0.0
        self.m_bot_pwr_loss = 0.0

        # Output capacitor computed values
        self.c_low_v_ripple = 0.0
        self.c_low_esr_eff = 0.0
        self.c_low_cap_eff = 0.0
        self.c_low_cap_min = 0.0
        self.c_low_pwr_loss = 0.0

    def validate_absolute_limits(self):
        # Switching frequency limits
        self._log_assert(60e3 <= self.p.opp.f_sw <= 750e3, "Switching frequency must be between 60 kHz and 750 kHz")

        # Minimum On-Time (pg. 29)
        ref_on_time = 150e-9  # (sec) Best case performance from datasheet
        act_on_time = self.p.opp.v_out / (self.p.opp.v_in * self.p.opp.f_sw)
        self._log_assert(ref_on_time < act_on_time, f"{ref_on_time * 1e9:.2f}nS > {act_on_time * 1e9:.2f}nS")

    def validate_inductor(self):
        """
        Validates the inductor selection using the assumption of DCR style current sensing.
        """
        # Compute approximate equality of the output inductor characteristics and the two RC filter stages
        l_by_dcr = round(self.p.inductor.l / self.p.inductor.dcr, 6)
        r1_c1_5 = round(self.p.i_filter.r1 * self.p.i_filter.c1 * 5, 6)
        r2_c2 = round(self.p.i_filter.r2 * self.p.i_filter.c2, 6)

        # print(f"L/DCR: {l_by_dcr}, R1C1_5: {r1_c1_5}, R2C2: {r2_c2}")
        self._log_assert(math.isclose(l_by_dcr, r1_c1_5, abs_tol=1e-4), f"L/DCR {l_by_dcr} != 5 * R1C1 {r1_c1_5}")
        self._log_assert(math.isclose(l_by_dcr, r2_c2, abs_tol=1e-4), f"L/DCR {l_by_dcr} != R2C2 {r2_c2}")

        # Pre-compute a few useful values.
        duty_max = (self.p.opp.v_out / (0.9 * self.p.opp.v_in))  # Estimated 90% efficiency (see SLVA477B section 2)
        v_delta = self.p.opp.v_in - self.p.opp.v_out

        # Compute the minimum inductor value to achieve desired ripple current. << current == lower core loss
        self.l_min = (v_delta / (self.p.opp.f_sw * self.il_ripple_target)) * duty_max
        self._log_assert(self.p.inductor.l >= self.l_min,
                         f"Inductor value {self.p.inductor.l * 1e6:.2f}uH < minimum {self.l_min * 1e6:.2f}uH"
                         f" for ripple current target")

        # Compute the actual ripple current in the inductor
        self.il_ripple_actual = (v_delta * duty_max) / (self.p.inductor.l * self.p.opp.f_sw)
        if self.il_ripple_actual > self.op_i_out_phase:
            self.il_ripple_actual = self.op_i_out_phase

        # Compute voltage ripple peak voltage due to inductor current ripple
        self.l_v_ripple = self.p.inductor.dcr * (self.op_i_out_phase + (self.il_ripple_actual / 2.0))

        # Compute voltage ripple across the SNSA+ and SNS- pins after filtering through R1C1
        self.l_v_sense_ripple = duty_max * v_delta / (self.p.i_filter.r1 * self.p.i_filter.c1 * self.p.opp.f_sw)

        # Ensure the actual voltage ripple can be sensed by the LTC7871 using the current limit setting
        self.l_v_sense_ripple_max = VSense_Max_DCR[self.p.opp.i_lim]
        self._log_assert(self.l_v_sense_ripple < self.l_v_sense_ripple_max,
                         f"Inductor ripple vSense {self.l_v_sense_ripple * 1e3:.2f}mV > configured max "
                         f"vSense {self.l_v_sense_ripple_max * 1e3:.2f}mV")

        # Ensure we meet the minimum voltage sense ripple requirement for SNR and accuracy
        self._log_assert(self.l_v_ripple >= 10e-3,
                         f"SNSA+, SNS- ripple voltage {self.l_v_ripple * 1e3:.2f}mV < 10mV recommended. "
                         f"Inductor DCR is too low to regulate per-phase current of {self.op_i_out_phase * 1e3:.2f}mA.")

        # Ensure sense pin voltage limits aren't exceeded
        self._log_assert(self.p.opp.v_out <= 60, f"Output voltage {self.p.opp.v_out} > 60V limit for SNS pins")

        # Peak Current Rating
        self.l_peak_current = self.op_i_out_phase + self.il_ripple_actual / 2.0
        self._log_assert(self.p.inductor.i_sat >= self.l_peak_current,
                         f"Peak current rating {self.p.inductor.i_sat} < {self.l_peak_current}")

        # Compute copper loss
        self.l_avg_pwr_loss = self.op_i_out_phase ** 2 * self.p.inductor.dcr
        self.l_peak_pwr_loss = self.l_peak_current ** 2 * self.p.inductor.dcr
        self.total_power_dissipation += (self.l_avg_pwr_loss * self.p.opp.phases)

        # Compute power loss in R1, which has to handle a bit more dissipation than typical
        self.il_r1_pwr_loss = ((100 - self.p.opp.v_out) * self.p.opp.v_out) / self.p.i_filter.r1
        self.total_power_dissipation += (self.il_r1_pwr_loss * self.p.opp.phases)

    def validate_mosfet(self):
        rds_on = self.p.mosfet.rds_on
        crss = self.p.mosfet.crss
        iavg = self.p.mosfet.i_avg
        vth = self.p.mosfet.vth
        rdr = 1.5  # Resistance of top switch driver (Ohm), from LTC7060 R_UP(TG)?

        # Junction Temp Adjustment
        temp_factor = self.p.mosfet.t_delta * (self.p.opp.tj_max - self.p.opp.t_ambient)

        # Top Switch Mosfet Dissipation
        mos_top_1 = (self.p.opp.v_out / self.p.opp.v_in) * (self.op_i_out_phase ** 2.0) * (1.0 + temp_factor) * rds_on
        mos_top_2 = (self.p.opp.v_in ** 2.0) * (self.op_i_out_phase / 2.0) * rdr * crss
        mos_top_3 = ((1.0 / (self.p.opp.drvcc - vth)) + (1.0 / vth)) * self.p.opp.f_sw
        self.m_top_pwr_loss = mos_top_1 + mos_top_2 * mos_top_3

        # Bottom Switch Mosfet Dissipation
        mos_bot_1 = (self.p.opp.v_in - self.p.opp.v_out) / self.p.opp.v_in
        mos_bot_2 = self.op_i_out_phase ** 2.0
        mos_bot_3 = (1.0 + temp_factor) * rds_on
        self.m_bot_pwr_loss = mos_bot_1 * mos_bot_2 * mos_bot_3

        # Validate average current rating
        self._log_assert(iavg >= self.op_i_out_phase, f"Average current rating {iavg} < {self.op_i_out_phase}")

        # Summarize this block
        self.total_power_dissipation += ((self.m_top_pwr_loss + self.m_bot_pwr_loss) * self.p.opp.phases)

    def validate_high_side_capacitor(self):
        pass

    def validate_low_side_capacitor(self):
        # Ensure voltage limits aren't exceeded
        self._log_assert(self.p.low_side_cap.fos_v * self.p.opp.v_out < self.p.low_side_cap.ceramic.v,
                         f"Low side ceramic voltage rating FOS not met.")
        self._log_assert(self.p.low_side_cap.fos_v * self.p.opp.v_out < self.p.low_side_cap.electrolytic.v,
                         f"Low side electrolytic voltage rating FOS not met.")

        # Compute effective ESR and capacitance for the low side capacitors
        c_low_ceramic_cap = self.p.low_side_cap.ceramic.c
        c_low_ceramic_esr = self.p.low_side_cap.ceramic.esr
        c_low_ceramic_n = self.p.low_side_cap.num_ceramic
        _ceramic_eff_esr = c_low_ceramic_esr / c_low_ceramic_n

        c_low_electrolytic_cap = self.p.low_side_cap.electrolytic.c
        c_low_electrolytic_esr = self.p.low_side_cap.electrolytic.esr
        c_low_electrolytic_n = self.p.low_side_cap.num_electrolytic
        _electrolytic_eff_esr = c_low_electrolytic_esr / c_low_electrolytic_n

        # Compute effective capacitance and ESR from both ceramic and electrolytic capacitors
        self.c_low_esr_eff = 1.0 / ((1.0 / _ceramic_eff_esr) + (1.0 / _electrolytic_eff_esr))
        self.c_low_cap_eff = (c_low_ceramic_cap * c_low_ceramic_n) + (c_low_electrolytic_cap * c_low_electrolytic_n)

        # Validate steady state output ripple voltage isn't exceeded
        self.c_low_v_ripple = self.il_ripple_actual * (self.c_low_esr_eff + (1.0 / (8.0 * self.p.opp.f_sw * self.c_low_cap_eff)))
        self._log_assert(self.c_low_v_ripple <= self.p.opp.v_ripple,
                         f"Output voltage ripple {self.c_low_v_ripple * 1e3:.2f}mV > {self.p.opp.v_ripple * 1e3:.2f}mV")

        # Validate minimum capacitance is met
        self.c_low_cap_min = 1.0 / (8.0 * self.p.opp.f_sw * self.p.inductor.dcr)
        self._log_assert(self.c_low_cap_eff > self.c_low_cap_min,
                         f"Output capacitance {self.c_low_cap_eff * 1e6}uF < {self.c_low_cap_min * 1e6:.2f}uF")

        # Compute total capacitor power dissipation. This is spread out across all the capacitors, but it's
        # relative to the ESR of each capacitor. The RMS current is used to compute the power loss, which is
        # equivalent the inductor ripple current. See ROHM 64AN035E for more details.
        self.c_low_pwr_loss = (self.il_ripple_actual * self.p.opp.phases) ** 2 * self.c_low_esr_eff
        self.total_power_dissipation += self.c_low_pwr_loss

    def summarize(self):
        logger.info(f"\tPer-Phase R1 Power Loss: {self.il_r1_pwr_loss:.2f}W")
        logger.info(f"\tPer-Phase Exp SNS +/- Ripple {self.l_v_sense_ripple * 1e3:.2f}mV")
        logger.info(f"\tPer-Phase Inductance: {self.p.inductor.l * 1e6:.2f}uH, Min: {self.l_min * 1e6:.2f}uH")
        logger.info(f"\tPer-Phase Inductor Avg Current: {self.op_i_out_phase:.2f}A")
        logger.info(f"\tPer-Phase Inductor Peak Current: {self.l_peak_current:.2f}A")
        logger.info(f"\tPer-Phase Inductor Ripple Current: {self.il_ripple_actual:.2f}A")
        logger.info(f"\tPer-Phase Inductor Avg I2R Loss: {self.l_avg_pwr_loss:.2f}W")
        logger.info(f"\tPer-Phase Inductor Peak I2R Loss: {self.l_peak_pwr_loss:.2f}W")
        logger.info(f"\tPer-Phase High Side Switch Power Dissipation: {self.m_top_pwr_loss:.2f}W")
        logger.info(f"\tPer-Phase Low Side Switch Power Dissipation: {self.m_bot_pwr_loss:.2f}W")
        logger.info(f"\tOutput voltage ripple: {self.c_low_v_ripple * 1e3:.2f}mV, Max: {self.p.opp.v_ripple * 1e3:.2f}mV")
        logger.info(f"\tOutput capacitance: {self.c_low_cap_eff * 1e6:.2f}uF, Min: {self.c_low_cap_min * 1e6:.2f}uF")
        logger.info(f"\tCapacitor Dissipation: {self.c_low_pwr_loss:.2f}W")
        logger.info(f"\tTotal Power Dissipation: {self.total_power_dissipation:.2f}W")

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
        logger.info(f"Validating: {self.p.name}")
        self.validate_inductor()
        self.validate_mosfet()
        self.validate_high_side_capacitor()
        self.validate_low_side_capacitor()
        self.summarize()
        # self.compute_programmable_output_voltage_range()

        if self._failed_assertions:
            logger.warning(f"Failed Assertions: {self.p.name}")
            for msg in self._failed_assertions:
                logger.warning(f"\t{msg}")
            return False

        return True

    def _log_assert(self, condition: bool, message: str):
        if not condition:
            self._failed_assertions.append(message)
