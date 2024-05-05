#!/usr/bin/env/python3

from ltc7871_validator import *


# Mosfet Options
MCAC80N10Y = Mosfet("MCAC80N10Y", t_delta=0.005, rds_on=3.3e-3, crss=15e-12, i_avg=80, vth=2.0)

# Inductor Options
PQ2614BLA3R3KND = Inductor("PQ2614BLA-3R3K-ND", l=3.3e-6, dcr=1.6e-3, i_sat=54, i_avg=30)

# Ceramic Capacitor Options
CGA6P1X7R1N106M250AC = Capacitor("CGA6P1X7R1N106M250AC", c=2e-6, esr=3e-3)

# Electrolytic Capacitor Options
C_220uF_100V = Capacitor("80ZLH220MEFCT810X23", c=220e-6, esr=0.11)

# Power Stage Operating Points
opp_peak_stress = OPP(description="Put peak stress on the power stage with max v_in, min v_out, and max power",
                      v_in=75.4, v_out=12.0, p_max=1000, v_ripple=100e-3, i_ripple=0.4, f_sw=500e3, phases=4,
                      i_lim=ILim.ILIM_0)


# Full Designs
ltc7871_designs = [
    LTC7871Parameters(name="Design1", mosfet=MCAC80N10Y, inductor=PQ2614BLA3R3KND, opp=opp_peak_stress,
                      high_side_cap=HybridCapacitor(ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4,
                                                    electrolytic=C_220uF_100V, num_electrolytic=4),
                      low_side_cap=HybridCapacitor(ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4,
                                                   electrolytic=C_220uF_100V, num_electrolytic=4),
                      i_sense_filter=CurrentSenseFilter(r1=10e3, c1=0.2e-6, r2=10e3, c2=0.33e-6)
                      )
]


if __name__ == "__main__":
    for design in ltc7871_designs:
        LTC7871Validator(design).validate()
