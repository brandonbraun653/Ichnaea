from functools import lru_cache
from typing import Tuple

from app.ichnaea.board_values import V_LOW_FB_RB, V_LOW_FB_RA

# On-chip registers
REG_MFR_FAULT = 0x01
REG_MFR_OC_FAULT = 0x02
REG_MFR_NOC_FAULT = 0x03
REG_MFR_STATUS = 0x04
REG_MFR_CONFIG1 = 0x05
REG_MFR_CONFIG2 = 0x06
REG_MFR_CHIP_CTRL = 0x07
REG_MFR_IDAC_VLOW = 0x08
REG_MFR_IDAC_VHIGH = 0x09
REG_MFR_IDAC_SETCUR = 0x0A
REG_MFR_SSFM = 0x0B
REG_MIN_ADDR = 0x01
REG_MAX_ADDR = 0x0B

# Tolerances and limits
PWR_GOOD_TOLERANCE = 0.1  # Voltage regulation tolerance percent (0.0-1.0)

# VFB LOW/HIGH IDAC Lookup Table
idac_lookup = {
    -64: 0b1000000,
    -63: 0b1000001,
    -62: 0b1000010,
    -61: 0b1000011,
    -60: 0b1000100,
    -59: 0b1000101,
    -58: 0b1000110,
    -57: 0b1000111,
    -56: 0b1001000,
    -55: 0b1001001,
    -54: 0b1001010,
    -53: 0b1001011,
    -52: 0b1001100,
    -51: 0b1001101,
    -50: 0b1001110,
    -49: 0b1001111,
    -48: 0b1010000,
    -47: 0b1010001,
    -46: 0b1010010,
    -45: 0b1010011,
    -44: 0b1010100,
    -43: 0b1010101,
    -42: 0b1010110,
    -41: 0b1010111,
    -40: 0b1011000,
    -39: 0b1011001,
    -38: 0b1011010,
    -37: 0b1011011,
    -36: 0b1011100,
    -35: 0b1011101,
    -34: 0b1011110,
    -33: 0b1011111,
    -32: 0b1100000,
    -31: 0b1100001,
    -30: 0b1100010,
    -29: 0b1100011,
    -28: 0b1100100,
    -27: 0b1100101,
    -26: 0b1100110,
    -25: 0b1100111,
    -24: 0b1101000,
    -23: 0b1101001,
    -22: 0b1101010,
    -21: 0b1101011,
    -20: 0b1101100,
    -19: 0b1101101,
    -18: 0b1101110,
    -17: 0b1101111,
    -16: 0b1110000,
    -15: 0b1110001,
    -14: 0b1110010,
    -13: 0b1110011,
    -12: 0b1110100,
    -11: 0b1110101,
    -10: 0b1110110,
    -9: 0b1110111,
    -8: 0b1111000,
    -7: 0b1111001,
    -6: 0b1111010,
    -5: 0b1111011,
    -4: 0b1111100,
    -3: 0b1111101,
    -2: 0b1111110,
    -1: 0b1111111,
    0: 0b0000000,
    1: 0b0000001,
    2: 0b0000010,
    3: 0b0000011,
    4: 0b0000100,
    5: 0b0000101,
    6: 0b0000110,
    7: 0b0000111,
    8: 0b0001000,
    9: 0b0001001,
    10: 0b0001010,
    11: 0b0001011,
    12: 0b0001100,
    13: 0b0001101,
    14: 0b0001110,
    15: 0b0001111,
    16: 0b0010000,
    17: 0b0010001,
    18: 0b0010010,
    19: 0b0010011,
    20: 0b0010100,
    21: 0b0010101,
    22: 0b0010110,
    23: 0b0010111,
    24: 0b0011000,
    25: 0b0011001,
    26: 0b0011010,
    27: 0b0011011,
    28: 0b0011100,
    29: 0b0011101,
    30: 0b0011110,
    31: 0b0011111,
    32: 0b0100000,
    33: 0b0100001,
    34: 0b0100010,
    35: 0b0100011,
    36: 0b0100100,
    37: 0b0100101,
    38: 0b0100110,
    39: 0b0100111,
    40: 0b0101000,
    41: 0b0101001,
    42: 0b0101010,
    43: 0b0101011,
    44: 0b0101100,
    45: 0b0101101,
    46: 0b0101110,
    47: 0b0101111,
    48: 0b0110000,
    49: 0b0110001,
    50: 0b0110010,
    51: 0b0110011,
    52: 0b0110100,
    53: 0b0110101,
    54: 0b0110110,
    55: 0b0110111,
    56: 0b0111000,
    57: 0b0111001,
    58: 0b0111010,
    59: 0b0111011,
    60: 0b0111100,
    61: 0b0111101,
    62: 0b0111110,
    63: 0b0111111
}


def get_mfr_idac_value(ivfb: int):
    """
    Get the MFR_IDAC_VLOW/VHIGH register value for a given IVFBLOW/VFBHIGH current.

    Args:
        ivfb: The IVFBLOW/VFBHIGH current value in μA

    Returns:
        The corresponding MFR_IDAC_VLOW/VHIGH register value
    """
    try:
        return idac_lookup[ivfb]
    except KeyError:
        raise ValueError(f"Invalid IVFBLOW/VFBHIGH value: {ivfb}")


def compute_optimal_vlow_idac(vout: float, ra: float = V_LOW_FB_RA, rb: float = V_LOW_FB_RB) -> Tuple[int, float]:
    """
    Computes the optimal IDAC register setting to achieve an output voltage given
    the input voltage and resistor values.

    See Also:
        Page 17 of the LTC7871 datasheet

    Args:
        vin: Input voltage to the system
        vout: Desired output voltage
        ra: Bottom resistor value in the feedback voltage divider
        rb: Top resistor value in the feedback voltage divider

    Returns:
        The optimal IDAC register setting and the actual output voltage
    """
    assert rb > 0, "Rb must be greater than 0"
    assert ra > 0, "Ra must be greater than 0"
    assert 0 < vout < 60, f"Output voltage must be between 0 and 60V, got {vout}"

    # Compute the nominal voltage the circuit is trying to regulate to
    regulating_voltage = 1.2 * (1 + (rb / ra))

    # Search the idac lookup table for the optimal idac setting
    best_idac = 0
    best_voltage = 0.0
    lowest_error = 1000.0

    for ivfb in idac_lookup.keys():
        idac = get_mfr_idac_value(ivfb)
        actual_voltage = regulating_voltage - ((ivfb * 1e-6) * rb)

        error = abs(vout - actual_voltage) / vout
        if error < lowest_error:
            best_idac = idac
            best_voltage = actual_voltage
            lowest_error = error

    return best_idac, best_voltage


@lru_cache(maxsize=2)
def voltage_resolution(ra: float = V_LOW_FB_RA, rb: float = V_LOW_FB_RB) -> float:
    """
    Computes the controllable output voltage resolution of the LTC7871 in volts.

    See Also:
        Page 17 of the LTC7871 datasheet

    Args
        ra: Bottom resistor value in the feedback voltage divider
        rb: Top resistor value in the feedback voltage divider

    Returns:
        The output voltage resolution in volts
    """
    controllable_range = []
    for ivfb in idac_lookup.keys():
        actual_voltage = 1.2 * (1.0 + (rb / ra)) - ((ivfb * 1e-6) * rb)
        controllable_range.append(actual_voltage)

    # Find the maximum step size in the controllable range. This is effectively
    # the "worst-case" resolution of the circuit over then entire output voltage range.
    return max([abs(x - y) for x, y in zip(controllable_range, controllable_range[1:])])


if __name__ == "__main__":
    idac, _ = compute_optimal_vlow_idac(12.0)
    print(hex(idac))
