# Circuit Updates
- [ ] Add HV lightning protection, and IO ESD protection
- [ ] Update resistors that come in contact with high voltage input to a better voltage rated part. Currently only rated to 75V
and we can technically go up to 90V.
- [ ] Add a weak pullup on LED IO to prevent ghosting.
- [ ] Add a versioning resistor divider for one of the extra ADC pins. Can be used to identify future revisions in software.
- [ ] Remove 0-ohm resistors on validated interfaces.
- [ ] Remove the daisy chain HW interface. Design has solidified into micro-string inverter.
- [ ] Increase power switch height to create uniform plane for heatsink contact.
- [ ] Add a second serial interface for debug and system testing control.
- [ ] Add an eeprom for saving settings from the BMS.
- [ ] Add NOR flash for logging asserts/faults. Prioritize over EEPROM w/pin constraints.
- [ ] Add a CAN bus serial interface for distributed control.
- [ ] Consider an integrated half-bridge mosfet solution. Might actually be cheaper than discrete components and take up less space.
- [ ] Output capacitors should be same height or shorter than inductors. You can't lay it flat on the back without it feeling unstable.
- [ ] Add MOSFET to PWMEN pin (see Design Faults)
- [ ] Add test points to critical connection for bed of nails testing
  - [ ] SPI
  - [ ] Power
    - [ ] 1.1V
    - [ ] 3.3V
    - [ ] 5.0V
    - [ ] 12.0V
    - [ ] Vout
    - [ ] Vin
- [ ] Remove R11 from ADC circuitry. Not needed.
- [ ] Add ADC channels:
  - [ ] 5V from LTC
  - [ ] 12V from Buck Converter
  - [ ] ?

# Assembly Errors
- Forgot to order D3 (SS210) on the bottom of the board. Feeds power from VLow to AP66200. Dang this happened twice. I have two diodes like this.
- PWM fan pin header is not the expected type. It has a peg for alignment and the latch mechanism in the back is shorter than expected.

# PCB Layout
- [ ] Improve thermal pad connections. Too hard to solder large components to the board without the hot plate.
  - Inductors
  - Output capacitors (through hole)
- [ ] Don't put components right next to pins you may need to rework. It's hard to get a soldering iron in there.
- [ ] USB C connector sticks too far through the board. Had to raise it up for the inductor to lay flat beneath it. Switch to SMD USB micro B.
- [ ] Place all low power SMD components on top layer. Soldering rework is a *nightmare* once the inductors go on the back.
- [ ] Shrink the SOT-23-5 package to the standard layout side. The hand soldering pad sizing is excessive.
- [ ] Increase copper pour coverage for high voltage input

# PCB Design
- [ ] Manually add the fiducials for manufacturing tooling. I want to control their location.
- [ ] Specify serial location for JLC.

# General Notes
- Might need Kapton tape to prevent shorting heatsink to through hole capacitor leads.
- Need to add mount holes or plan out mounting situation with final inverter packaging.

# Design Faults
- [ ] Failed to connect all 1.1V rail pins together. This prevents the RP2040 from booting.
- [ ] Pretty certain I screwed up the USB connection by routing it all the way over to the other side of the board.
What I should have done is stuck with a surface mount USB micro B connector, kept the traces short, and moved the connector near the RP2040.
- [ ] LTC7871 SDO pin requires an external pull up resistor (open drain output). Missed that line in the datasheet.
- [ ] Control of the RUN pin is entirely incorrect. Needs to be a single mosfet that pulls the RUN line low while an external resistor pulls it
high to 5V. Default 5V pull up NOT on the V5 LTC pin because that doesn't turn on until the RUN pin is high. This **MUST** always allow the the
LTC to free run so that a sudden reset of the RP2040 doesn't cause a glitch in the power. It's expected that the BMS will provide the required
power isolation from this converter before allowing charge to flow.
- [ ] Control of output ON/OFF is entirely incorrect (see previous comments about the RUN pin). Need to add a MOSFET on the PWMEN pin to pull the
signal to GND. This will allow the RP2040 to control power ON/OFF behavior without disabling communication with the LTC7871. I need to be able to
program current/voltage limits to a safe value before enabling the output.
- [ ] Pin header for connecting to the BMS needs to change. I just ripped it off accidentally.

# Assembly Steps (Worked Well)
- There was so much thermal mass with the copper pours that I needed a hot plate to do all the initial placement and rework. Was truly beautiful. Next
time I should validate the logic board bringup before soldering the bottom components. There are no functional dependencies on those components.

