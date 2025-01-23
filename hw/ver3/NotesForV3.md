# Version 3 Notes

## Circuit Updates

- Add an active bleeder resistor to the output to act as a short for calibration and system safe-ing
- Add high current, high side load switch to enable safe connection to battery (?)
  - Evaluate feasibility vs control from the BMS side
  - Add ADC measurement before/after the load switch to check for output match before connection
- Integrate BMS and Balancer onto the same board
- Potentially slave the power stage to the BMS
  - SPI: Control of LTC7871, not very high speed
  - Basic ADC: Input Voltage, Output Current/Voltage

## Circuit Bugs

- A 100k resistor across the SS pin causes the system to not boot properly. Removing fixes it, but could also go several MOhm(?).
- A sufficiently cold temperature (5C) on the NTC thermistors can cause the sense output to exceed 3.3v, breaking the ADC input.

## Assembly Notes

- The ESD diode SOD-923 packages are **incredibly** tiny. Not fun to place by hand.
- 12v/5v capacitors on the bottom of the board were placed too close together.
