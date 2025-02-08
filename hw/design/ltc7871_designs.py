#!/usr/bin/env/python3

from ltc7871_validator import *


# Mosfet Options
MCAC80N10Y = Mosfet("MCAC80N10Y", t_delta=0.005, rds_on=4.3e-3, crss=15e-12, i_avg=80, vth=2.0, vds=100)

# Inductor Options
PQ2614BLA3R3KND = Inductor("PQ2614BLA-3R3K-ND", l=3.3e-6, dcr=1.6e-3, i_sat=54, i_avg=30)
PQ2614BLA4R7KND = Inductor("PQ2614BLA-4R7K-ND", l=4.7e-6, dcr=1.6e-3, i_sat=36.9, i_avg=30)

# Ceramic Capacitor Options (De-rated for max output voltage)
CGA6P1X7R1N106M250AC = Capacitor("CGA6P1X7R1N106M250AC", c=2e-6, v=75, esr=36e-3)
CL32B106KMVNNWE = Capacitor("CL32B106KMVNNWE", c=2e-6, v=63, esr=31e-3)
GRM32EC72A106KE05L = Capacitor("GRM32EC72A106KE05L", c=10e-6, v=100, esr=0.1)

# Electrolytic Capacitor Options
C_220uF_100V = Capacitor("80ZLH220MEFCT810X23", c=220e-6, v=80, esr=0.11)

# Current Sense Filter Options
filter_3_3uH_1_6mOhm = CurrentSenseFilter(r1=4.9e3, c1=100e-9, r2=22e3, c2=100e-9)  # Works with PQ2614BLA3R3KND
filter_4_7uH_1_6mOhm = CurrentSenseFilter(r1=6.2e3, c1=100e-9, r2=6.2e3, c2=470e-9)  # Works with PQ2614BLA4R7KND

# Power Stage Operating Points
opp_peak_stress = OPP(
    description="Put peak stress on the power stage with max v_in, min v_out, and max power",
    v_in=75.4,
    v_out=12.0,
    p_max=2000,
    v_ripple=20e-3,
    i_ripple=0.2,
    f_sw=600e3,
    phases=6,
    i_lim=ILim.ILIM_3_4,
)

opp_max_vin_min_vout_min_pwr = OPP(
    description="Put peak voltage stress on the power stage, but low current stress",
    v_in=75.4,
    v_out=12.0,
    p_max=150,
    v_ripple=100e-3,
    i_ripple=0.99,
    f_sw=250e3,
    phases=6,
    i_lim=ILim.ILIM_3_4,
)

opp_vlow_max_power = OPP(
    description="Low voltage input, high power",
    v_in=24.0,
    v_out=14,
    p_max=1000,
    v_ripple=20e-3,
    i_ripple=0.1,
    f_sw=400e3,
    phases=6,
    i_lim=ILim.ILIM_3_4,
)

opp_vlow_min_power = OPP(
    description="Low voltage input, high power",
    v_in=24.0,
    v_out=14,
    p_max=10,
    v_ripple=20e-3,
    i_ripple=0.99,
    f_sw=750e3,
    phases=6,
    i_lim=ILim.ILIM_3_4,
)

opp_target_loading = OPP(
    description="Expected operating condition full power",
    v_in=75.4,
    v_out=56.0,
    p_max=2000,
    v_ripple=20e-3,
    i_ripple=0.2,
    f_sw=650e3,
    phases=6,
    i_lim=ILim.ILIM_3_4,
)


# A few design corner cases to explore. This by no means is robust enough for a full design, but it gets
# me in the ballpark for a few different scenarios. More robust testing is required with the real hardware
# to learn the full dynamics. The LTC7871 has some unique regulation modes I can enable to help with some
# of these corner cases, but I have a hard time simulating their expected impact in these scripts. Use the
# LTC7871 LTSPICE simulation in this same folder to see more detailed analysis.
#
# Mode Pin: GND = Forced Continuous Conduction Mode (FCCM)
#    Observed fantastic voltage regulation but the ripple current was rather large, as you'd expect.
# Mode Pin: Float = Burst Mode
#    Voltage regulation was very poor, but the ripple current was better (?) though quite peaky.
# Mode Pin: V5 = Discontinuous Conduction Mode (DCM)
#    Does pretty much what you'd expect. Works really well for low power regimes.
num_ceramic_caps = 8
num_electrolytic_caps = 6
low_side_cap = HybridCapacitor(
    ceramic=CGA6P1X7R1N106M250AC,
    num_ceramic=num_ceramic_caps,
    electrolytic=C_220uF_100V,
    num_electrolytic=num_electrolytic_caps,
)

ltc7871_designs = [
    LTC7871Parameters(
        name="High power with high voltage differential",
        opp=opp_peak_stress,
        mosfet=MCAC80N10Y,
        inductor=PQ2614BLA4R7KND,
        high_side_cap=HybridCapacitor(
            ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4, electrolytic=C_220uF_100V, num_electrolytic=4
        ),
        low_side_cap=low_side_cap,
        i_filter=filter_4_7uH_1_6mOhm,
    ),
    LTC7871Parameters(
        name="Low power with high voltage differential",
        opp=opp_max_vin_min_vout_min_pwr,
        mosfet=MCAC80N10Y,
        inductor=PQ2614BLA4R7KND,
        high_side_cap=HybridCapacitor(
            ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4, electrolytic=C_220uF_100V, num_electrolytic=4
        ),
        low_side_cap=low_side_cap,
        i_filter=filter_4_7uH_1_6mOhm,
    ),
    LTC7871Parameters(
        name="Low input voltage with high power",
        opp=opp_vlow_max_power,
        mosfet=MCAC80N10Y,
        inductor=PQ2614BLA4R7KND,
        high_side_cap=HybridCapacitor(
            ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4, electrolytic=C_220uF_100V, num_electrolytic=4
        ),
        low_side_cap=low_side_cap,
        i_filter=filter_4_7uH_1_6mOhm,
    ),
    LTC7871Parameters(
        name="Low input voltage with low power",
        opp=opp_vlow_min_power,
        mosfet=MCAC80N10Y,
        inductor=PQ2614BLA4R7KND,
        high_side_cap=HybridCapacitor(
            ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4, electrolytic=C_220uF_100V, num_electrolytic=4
        ),
        low_side_cap=low_side_cap,
        i_filter=filter_4_7uH_1_6mOhm,
    ),
    LTC7871Parameters(
        name="Target loading",
        opp=opp_target_loading,
        mosfet=MCAC80N10Y,
        inductor=PQ2614BLA4R7KND,
        high_side_cap=HybridCapacitor(
            ceramic=CGA6P1X7R1N106M250AC, num_ceramic=4, electrolytic=C_220uF_100V, num_electrolytic=4
        ),
        low_side_cap=low_side_cap,
        i_filter=filter_4_7uH_1_6mOhm,
    ),
]


if __name__ == "__main__":
    for design in ltc7871_designs:
        LTC7871Validator(design).validate()
