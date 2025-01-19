# Version 3 Notes

## Circuit Notes

- A 100k resistor across the SS pin causes the system to not boot properly. Removing fixes it, but could also go several MOhm(?).
- A sufficiently cold temperature (5C) on the NTC thermistors can cause the sense output to exceed 3.3v, breaking the ADC input.

## Assembly Notes

- The ESD diode SOD-923 packages are **incredibly** tiny. Not fun to place by hand.
- 12v/5v capacitors on the bottom of the board were placed too close together.
