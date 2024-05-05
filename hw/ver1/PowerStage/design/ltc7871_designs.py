#!/usr/bin/env/python3

from ltc7871_validator import *


# Mosfet Options
MCAC80N10Y = Mosfet("MCAC80N10Y", t_delta=0.005, rds_on=4.3e-3, crss=15e-12, i_avg=80, vth=2.0, vds=100)

# Inductor Options
PQ2614BLA3R3KND = Inductor("PQ2614BLA-3R3K-ND", l=3.3e-6, dcr=1.6e-3, i_sat=54, i_avg=30)

# Ceramic Capacitor Options
CGA6P1X7R1N106M250AC = Capacitor("CGA6P1X7R1N106M250AC", c=2e-6, v=75, esr=3e-3)

# Electrolytic Capacitor Options
C_220uF_100V = Capacitor("80ZLH220MEFCT810X23", c=220e-6, v=80, esr=0.11)

# Current Sense Filter Options
default_filter = CurrentSenseFilter(r1=4.9e3, c1=100e-9, r2=22e3, c2=100e-9)  # Works with PQ2614BLA3R3KND

# Power Stage Operating Points
opp_peak_stress = OPP(
    description="Put peak stress on the power stage with max v_in, min v_out, and max power",
    v_in=75.4, v_out=12.0, p_max=2000, v_ripple=20e-3, i_ripple=0.4, f_sw=400e3, phases=6,
    i_lim=ILim.ILIM_0)

opp_max_vin_min_vout_min_pwr = OPP(
    description="Put peak voltage stress on the power stage, but low current stress",
    v_in=75.4, v_out=12.0, p_max=150, v_ripple=100e-3, i_ripple=0.99, f_sw=250e3, phases=6,
    i_lim=ILim.ILIM_0)


# Full Designs
ltc7871_designs = [
    LTC7871Parameters(
        name="High power with high voltage differential",
        opp=opp_peak_stress,
        mosfet=MCAC80N10Y, inductor=PQ2614BLA3R3KND,
        high_side_cap=HybridCapacitor(ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4,
                                      electrolytic=C_220uF_100V, num_electrolytic=4),
        low_side_cap=HybridCapacitor(ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4,
                                     electrolytic=C_220uF_100V, num_electrolytic=2),
        i_filter=default_filter
        ),

    LTC7871Parameters(
        name="Low power with high voltage differential",
        opp=opp_max_vin_min_vout_min_pwr,
        mosfet=MCAC80N10Y, inductor=PQ2614BLA3R3KND,
        high_side_cap=HybridCapacitor(ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4,
                                      electrolytic=C_220uF_100V, num_electrolytic=4),
        low_side_cap=HybridCapacitor(ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4,
                                     electrolytic=C_220uF_100V, num_electrolytic=2),
        i_filter=default_filter
        )
]


if __name__ == "__main__":
    for design in ltc7871_designs:
        LTC7871Validator(design).validate()
