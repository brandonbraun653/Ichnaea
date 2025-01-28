# Version 3 Notes

## Circuit Updates

- [ ] Add an active bleeder resistor to the output to act as a "short" for calibration and system safe-ing. 1k should be sufficient.
- [ ] Add high current, high side load switch to enable safe connection to battery (?)
  - Evaluate feasibility vs control from the BMS side
  - Add ADC measurement before/after the load switch to check for output match before connection
- [ ] Improve control resolution on output voltage by varying Ra/Rb. Write a small python program to compute. Targeting 10mV steps 12v-60v output.
- [ ] Split into two PCBs
  - [ ] BMS (Sophrosyne) logic control board
  - [ ] PWR (Ichnaea) power switching/balance/isolation board
- [ ] Increase the size of ESD protection diodes

## Circuit Bugs

- [ ] A 100k resistor across the SS pin causes the system to not boot properly. Removing fixes it, but could also go several MOhm(?).
- [ ] A sufficiently cold temperature (5C) on the NTC thermistors can cause the sense output to exceed 3.3v, breaking the ADC input.
- [ ] 12v/5v capacitors on the bottom of the board were placed too close together.

## Assembly Notes

- The ESD diode SOD-923 packages are **incredibly** tiny. Not fun to place by hand.


## Sophrosyne
- Logic control board only. No power components.
- SPI interface to power board:
  - LTC7871
  - Discrete IO for controlling balance/isolation switches
- Analog line + selection pins:
  - Power stage voltage measurement
  - Battery cell voltage measurement

## Ichnaea
- Try to get a single flat plane for component height. Ideally the power IC side of the system is covered in a giant block of aluminium.
- Mount the PCB via tapped holes in the heatsink. May use small screws.
- Needs to be safe-off when Sophrosyne isn't connected.
