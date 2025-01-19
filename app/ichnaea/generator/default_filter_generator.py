from dataclasses import dataclass
from ichnaea.generator.filter_coefficients import *
from loguru import logger
import math

@dataclass
class LPFilterSpec:
    name: str  # Name of the filter
    order: int # Order of the filter
    freq_cutoff_hz: float  # Cutoff frequency (Hz)
    freq_sample_hz: float  # Sampling frequency (Hz)

    @property
    def sample_rate_ms(self) -> int:
        return int((1.0/self.freq_sample_hz) * 1000.0)

    @property
    def coefficients(self) -> List[float]:
        logger.info(f"Generating coefficients for {self.name}: order={self.order}, cutoff={self.freq_cutoff_hz}Hz, fs={self.freq_sample_hz}Hz")
        return design_lowpass_filter(self.order, self.freq_cutoff_hz, self.freq_sample_hz)

    def generate_cpp_declarations(self) -> List[str]:
        declarations = []
        declarations.append(f"static constexpr size_t DFLT_FLTR_SAMPLE_RATE_{self.name.upper()}_MS = {self.sample_rate_ms};")
        declarations.append(f"static constexpr size_t DFLT_FLTR_ORDER_{self.name.upper()} = {self.order};")
        declarations.append(format_coefficients_as_c_array(self.name, self.coefficients))
        return declarations


if __name__ == "__main__":
    # Generate the default filter configurations
    filters = [
        LPFilterSpec("input_voltage", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
        LPFilterSpec("output_voltage", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
        LPFilterSpec("output_current", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
        LPFilterSpec("1v1_voltage", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
        LPFilterSpec("3v3_voltage", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
        LPFilterSpec("5v0_voltage", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
        LPFilterSpec("12v0_voltage", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
        LPFilterSpec("temperature", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
        LPFilterSpec("fan_speed", order=2, freq_cutoff_hz=25, freq_sample_hz=100),
    ]

    declarations = []
    for f in filters:
        declarations.extend(f.generate_cpp_declarations())

    # Write the declarations to a header file
    output_file = Path(__file__).parent.parent.parent.parent / "src" / "app" / "generated" / "default_filter_config.hpp"
    write_hpp_file(output_file, declarations)
    logger.info(f"Generated default filter configuration file: {output_file}")