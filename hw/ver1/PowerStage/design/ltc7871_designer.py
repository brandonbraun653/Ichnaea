#!/usr/bin/env python3
# This script validates a parameterized LTC7871 design based on some given design constraints. Mostly it just
# checks that the design is within the specified limits and feasible. Only the core buck converter is validated.

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
    crss: float   # Reverse transfer (miller) capacitance, in farads
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
class CurrentSense:
    """ Sense network configuration for the inductor current """
    r1: float  # Sense resistor 1, in ohms
    c1: float  # Sense capacitor 1, in farads
    r2: float  # Sense resistor 2, in ohms
    c2: float  # Sense capacitor 2, in farads


@dataclass
class OPP:
    """ Operational performance parameters for the buck converter """
    v_in: float  # Input voltage, in volts
    v_out: float  # Output voltage, in volts
    p_max: float  # Maximum output power, in watts
    phases: int  # Number of output phases driving the load
    f_sw: float  # Switching frequency, in hertz
    i_ripple: float  # Allowed output current ripple, in percent [0, 1]
    v_ripple: float  # Allowed output voltage ripple, in volts
    i_lim: ILim  # Current limit programming option


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
class Parameters:
    """ Design parameters for the buck converter """
    mosfet: Mosfet
    inductor: Inductor
    ceramic_cap: Capacitor
    electrolytic_cap: Capacitor
    sense: CurrentSense
    opp: OPP


class LTC7871Design:
    """ Validate a parameterized LTC7871 design """

    def __init__(self, param: Parameters):
        self.param = param

        # Validate input parameter limits
        self.validate_absolute_limits()

        # Max output current
        self.op_i_out_max = self.param.opp.p_max / self.param.opp.v_out
        self.op_i_out_phase = self.op_i_out_max / self.param.opp.phases

        # Inductor ripple current (A)
        self.dIL = self.op_i_out_max * self.param.opp.i_ripple

    def validate_absolute_limits(self):
        # Switching frequency limits
        assert 60e3 <= self.param.opp.f_sw <= 750e3, "Switching frequency must be between 60 kHz and 750 kHz"

        # Minimum On-Time (pg. 29)
        ref_on_time = 150e-9  # (sec) Best case performance from datasheet
        act_on_time = self.param.opp.v_out / (self.param.opp.v_in * self.param.opp.f_sw)
        assert ref_on_time < act_on_time, f"{ref_on_time * 1e9:.2f}nS > {act_on_time*1e9:.2f}nS"

    def validate_inductor(self):
        # Ensure VSenseMax isn't exceeded

    def validate(self) -> bool:
        """ Validate the given design parameters """
        pass


if __name__ == '__main__':
    pass