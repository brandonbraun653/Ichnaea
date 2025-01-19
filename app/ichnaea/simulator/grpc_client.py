import time

import grpc
from loguru import logger

from ichnaea.simulator.proto import sim_intf_pb2
from ichnaea.simulator.proto import sim_intf_pb2_grpc


def get_adc_reading(input_voltage: float, r1: float, r2: float) -> float:
    """
    Calculate the ADC reading for a given output voltage and resistor divider. This is
    inverting the typical calculation where an ADC reading is converted to the real voltage
    being measured.

    Args:
        input_voltage: Actual voltage that the ADC is sensing (volts)
        r1: Resistance of the first resistor in the divider (ohms)
        r2: Resistance of the second resistor in the divider (ohms)

    Returns:
        Scaled voltage the ADC should read, after passing through the resistor divider.
    """
    return input_voltage * r2 / (r1 + r2)

class EnvironmentSpoofer:
    """
    gRPC client interface to the Ichnaea simulator for testing purposes. This will modify the
    perceived environment measured by the software sensors in order to inject specific conditions
    for testing.
    """

    def __init__(self, host: str = "0.0.0.0", port: int = 50051):
        """
        Initialize the gRPC client to the simulator.
        Args:
            host: The hostname or IP address of the simulator.
            port: The port number of the simulator.
        """
        self._channel = grpc.insecure_channel(f"{host}:{port}")
        self._stub = sim_intf_pb2_grpc.EnvironmentSpooferStub(self._channel)

    def set_solar_ocv(self, voltage: float) -> None:
        """
        Set the simulated solar open circuit voltage.
        Args:
            voltage: The measured voltage to set, in volts.

        Returns:
            None
        """
        logger.trace(f"Setting solar OCV to {voltage:.2f} V")
        self._stub.SetSolarOCV(sim_intf_pb2.FloatingPointValue(value=voltage))

    def set_board_12v_rail(self, voltage: float) -> None:
        """
        Set the simulated 12V rail voltage.
        Args:
            voltage: The measured voltage to set, in volts.

        Returns:
            None
        """
        logger.trace(f"Setting 12V rail to {voltage:.2f} V")
        self._stub.SetBoard12VRail(sim_intf_pb2.FloatingPointValue(value=voltage))

    def set_board_5v_rail(self, voltage: float) -> None:
        """
        Set the simulated 5V rail voltage.
        Args:
            voltage: The measured voltage to set, in volts.

        Returns:
            None
        """
        logger.trace(f"Setting 5V rail to {voltage:.2f} V")
        self._stub.SetBoard5VRail(sim_intf_pb2.FloatingPointValue(value=voltage))

    def set_board_3v3_rail(self, voltage: float) -> None:
        """
        Set the simulated 3.3V rail voltage.
        Args:
            voltage: The measured voltage to set, in volts.

        Returns:
            None
        """
        logger.trace(f"Setting 3.3V rail to {voltage:.2f} V")
        self._stub.SetBoard3V3Rail(sim_intf_pb2.FloatingPointValue(value=voltage))

    def set_board_1v1_rail(self, voltage: float) -> None:
        """
        Set the simulated 1.1V rail voltage.
        Args:
            voltage: The measured voltage to set, in volts.

        Returns:
            None
        """
        logger.trace(f"Setting 1.1V rail to {voltage:.2f} V")
        self._stub.SetBoard1V1Rail(sim_intf_pb2.FloatingPointValue(value=voltage))

    def set_board_temperature(self, temperature: float) -> None:
        """
        Set the simulated board temperature.
        Args:
            temperature: The measured temperature to set, in degrees Celsius.

        Returns:
            None
        """
        logger.trace(f"Setting board temperature to {temperature:.2f} C")
        self._stub.SetBoardTemperature(sim_intf_pb2.FloatingPointValue(value=temperature))

    def set_board_fan_speed(self, speed: float) -> None:
        """
        Set the simulated fan speed.
        Args:
            speed: The measured fan speed to set, in RPM.

        Returns:
            None
        """
        logger.trace(f"Setting fan speed to {speed:.2f} RPM")
        self._stub.SetBoardFanSpeed(sim_intf_pb2.FloatingPointValue(value=speed))

    def set_output_voltage(self, voltage: float) -> None:
        """
        Set the simulated output voltage.
        Args:
            voltage: The measured voltage to set, in volts.

        Returns:
            None
        """
        logger.trace(f"Setting output voltage to {voltage:.2f} V")
        self._stub.SetOutputVoltage(sim_intf_pb2.FloatingPointValue(value=voltage))

    def apply_load(self, resistance: float) -> None:
        """
        Apply a resistive load to the system to simulate a load on the power supply.
        Args:
            resistance: The resistance of the load, in ohms.

        Returns:
            None
        """
        logger.trace(f"Applying resistive load of {resistance:.2f} ohms")
        self._stub.ApplyResistiveLoad(sim_intf_pb2.FloatingPointValue(value=resistance))

    def disconnect_load(self) -> None:
        """
        Disconnect the simulated load from the system by setting a very high resistance.
        Returns:
            None
        """
        logger.trace("Disconnecting simulated load")
        self._stub.ApplyResistiveLoad(sim_intf_pb2.FloatingPointValue(value=1e9))