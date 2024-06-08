# Circuit Updates (Desirements)
- Add HV lightning protection, and IO ESD protection
- Update resistors that come in contact with high voltage input to a better voltage rated part. Currently only rated to 75V and we can technically go up to 90V.
- Green LEDs next to the SWD connector are far too bright.
- All LEDs are showing a "ghosting" effect for a second during boot while the IO are tri-stated. Add a weak pullup on these IO to prevent the ghosting.

# Assembly Errors
- Forgot to order D3 (SS210) on the bottom of the board. Feeds power from VLow to AP66200.
Dang this happened twice. I have two diodes like this.
- PWM fan pin header is not the expected type. It has a peg for alignment and the latch mechanism in the back is shorter than expected.

# PCB Layout
- Improve thermal pad connections. Too hard to solder large components to the board.
  - Inductors
  - Output capacitors (through hole)
- Don't put components right next to pins you may need to rework. It's hard to get a soldering iron in there.
- USB C connector sticks too far through the board. Had to raise it up for the inductor to lay flat beneath it. Maybe switch to SMD USB micro B?
- Try and get as many components on the top of the board as possible. Soldering rework is a *nightmare* once the inductors go on the back.

# General Notes
- Output capacitors should be same height or shorter than inductors. You can't lay it flat on the back without it feeling unstable.
- Might need Kapton tape to prevent shorting heatsink to through hole capacitor leads.

# Design Faults
- Failed to connect all 1.1V rail pins together.
- Pretty certain I screwed up the USB connection by routing it all the way over to the other side of the board.
What I should have done is stuck with a surface mount USB micro B connector, kept the traces short, and moved the connector near the RP2040.
Specifically I'm thinking next to the lower left ground terminal.

# Assembly Steps (Worked Well)
- There was so much thermal mass with the copper pours that I needed a hot plate to do all the initial placement and rework. Was truly beautiful. Next
time I should validate the logic board bringup before soldering the bottom components. There are no functional dependencies on those components.
