# Circuit Updates
- [x] Add HV lightning protection, and IO ESD protection
- [x] Update resistors that come in contact with high voltage input to a better voltage rated part. Currently only rated to 75V
and we can technically go up to 90V.
- [x] Add a weak pullup on LED IO to prevent ghosting.
- [x] Add a versioning resistor divider for one of the extra ADC pins. Can be used to identify future revisions in software.
- [x] Remove 0-ohm resistors on validated interfaces.
- [x] Remove the daisy chain HW interface. Design has solidified into micro-string inverter.
- [ ] ~~Increase power switch height to create uniform plane for heatsink contact.~~ Sil-pads take up height diff.
- [x] Add a second serial interface for debug and system testing control.
- [ ] ~~Add an eeprom for saving settings from the BMS.~~
- [x] Add NOR flash for logging asserts/faults. Prioritize over EEPROM w/pin constraints.
- [ ] ~~Add a CAN/RS485 bus serial interface for distributed control.~~ Will be added to the BMS controller. This isn't the central brain.
- [x] Consider an integrated half-bridge mosfet solution. Might actually be cheaper than discrete components and take up less space.
- [x] Output capacitors should be same height or shorter than inductors. You can't lay it flat on the back without it feeling unstable.
- [x] Add MOSFET to PWMEN pin (see Design Faults)
- [x] Add test points to critical connection for bed of nails testing (16 IO for Salea Automation)
  - [x] SPI
  - [x] Power
    - [x] 1.1V
    - [x] 3.3V
    - [x] 5.0V
    - [x] 12.0V
    - [x] Vout
    - [x] Vin
  - [x] LTC Control
    - [x] PWMEN
    - [x] SS
    - [x] RUN
    - [x] MODE
    - [x] SYNC
    - [x] SETCUR
    - [x] OV/UV FB High and Low
    - [x] PWM Control Lines
- [x] Remove R11 from ADC circuitry. Not needed.
- [x] Add ADC channels:
  - [ ] ~~5V from LTC~~ Doesn't give useful information
  - [x] Every voltage rail
    - [x] HV DC
    - [x] LV DC
    - [x] +12V
    - [x] +5V
    - [x] +3.3V
    - [x] +1.1V
  - [x] Board Versioning Resistors
- [x] Add back the fault indicator LEDs for `PGOOD` and `FAULT` signals. I really wish I had them right now. (Using SPI status registers + controllable LEDs)
- [x] Add bleeder resistor to SS pin to allow for fault soft start recovery.
- [x] Add bleeder resistors to input and output capacitors. They don't fully discharge when removing power.
- [x] Remove 10k pulldown on MOSFET power stages. Redundant when using the HIP2210 driver.
- [x] Analyze adding secondary SMPS to supply gate driver power. LTC7871 is *burning* through power to supply 10V through an internal LDO.
  - Use diode OR-ing on the LTC supply path to switch over to the SMPS.
  - Control the SMPS power through the RP2040 via an EN signal once the system has bootstrapped.
  - Likely need to adjust the VREF pin of the HIP2210 to not go above 5V when the increased power supply is enabled.
  - Must be small height to fit under the heat sink. Supply peak loads of 20A, average much smaller. Desktop PSU suggests ~500mA average.
  - Add controllable ENABLE signal to manually turn on when system has stabilized.
- [x] Remove the 100 ohm series resistor with the analog switch output. Add test point.
- [x] Add low pass filter to analog signals. Getting lots of noise right now. 100Hz cutoff.
- [x] Remap LTC SYNC and LED_STATUS_2 pins to not be on the same PWM channels
  - [x] Re-verify all PWM channels for conflicts. Excel sheet?
- [x] Remove electrolytic capacitors in favor of a large amount of ceramics. They get very warm, aka terrible lifetime.
  - Might need to use bulk cheaper/smaller capacitors and array them to meet the voltage/capacity requirements.
- [x] Add more capacitance and filtering to the RP2040 3.3v rail. Will reset on large current loads.

# Assembly Errors
- Forgot to order D3 (SS210) on the bottom of the board. Feeds power from VLow to AP66200. Dang this happened twice. I have two diodes like this.

# PCB Layout
- [x] Improve thermal pad connections. Too hard to solder large components to the board without the hot plate.
  - Inductors
  - Output capacitors (through hole)
- [x] Don't put components right next to pins you may need to rework. It's hard to get a soldering iron in there.
- [x] USB C connector sticks too far through the board. Had to raise it up for the inductor to lay flat beneath it. Switch to SMD USB micro B.
- [x] Place all low power SMD components on top layer. Soldering rework is a *nightmare* once the inductors go on the back.
- [x] (Deleted components) Shrink the SOT-23-5 package to the standard layout side. The hand soldering pad sizing is excessive.
- [x] Increase copper pour coverage for high voltage input.
- [ ] Add fiducials for manufacturing.

# PCB Design
- [ ] Manually add the fiducials for manufacturing tooling. I want to control their location.
- [ ] Specify serial location for JLC.

# General Notes
- Might need Kapton tape to prevent shorting heatsink to through hole capacitor leads.
- [x] Need to add mount holes or plan out mounting situation with final inverter packaging.

# Design Faults
- [x] Failed to connect all 1.1V rail pins together. This prevents the RP2040 from booting.
- [x] Pretty certain I screwed up the USB connection by routing it all the way over to the other side of the board.
What I should have done is stuck with a surface mount USB micro B connector, kept the traces short, and moved the connector near the RP2040.
- [x] LTC7871 SDO pin requires an external pull up resistor (open drain output). Missed that line in the datasheet. Pull up to 3.3V.
- [x] Control of the RUN pin is entirely incorrect. Needs to be a single mosfet that pulls the RUN line low. An internal 2uA pullup will
automatically start the converter without an external pullup. It's expected that the BMS will provide the required power isolation during startup.
- [x] Control of output ON/OFF is entirely incorrect (see previous comments about the RUN pin). Need to add a MOSFET on the PWMEN pin to pull the
signal to GND. This will allow the RP2040 to control power ON/OFF behavior without disabling communication with the LTC7871. I need to be able to
program current/voltage limits to a safe value before enabling the output.
- [x] Pin header for connecting to the BMS needs to change. I just ripped it off accidentally.
- [x] Pull up on PWMEN needs to be stronger. R33 was 10k and replacing to 1k was much better.
- [x] Dead time resistor on HIP2210 should be 47k for the MCAC80N10Y to prevent shoot through. Validated experimentally.
- [x] I think I may have sized the heatsink holes wrong? Need to double check against the fan that I have.
- [x] LTC SYNC pullup (R23, 10k) is too weak. Switch to 1k.
- [x] Screw hole near the VIN terminal doesn't allow for the nut/bolt to seat flush. Need to compress all the inductors together more.
- [x] I've inadvertently mapped the RP2040 SYNC (4) and LED_STATUS_2 (20) pins to the same PWM channel (PWM2A).
