# Version 3 Notes

## Case Considerations
- The new board layout seems to favor airflow over both sides of the circuit.
  - Design around a standard PC fan or maybe a server fan?
  - Could 3d print a shroud to better direct airflow.
- Integrate terminal connection accessibility
  - Power terminals need protection from stray connections.
  - User input (USB, Reset, BOOT) needs access from the side of the case.

## Circuit Updates
- [ ] Split into two PCBs
  - BMS (Sophrosyne) logic control board
  - PWR (Ichnaea) power switching/balance/isolation board
- [ ] Increase the size of ESD protection diodes

### **System Sensor Improvements**
- [ ] Input Current Sense (enable efficiency calculations)
- [ ] 2x External temperature for heatsink temp (board temp isn't a sufficient indicator of power switch temp)
- [ ] Voltage sample before inline load switch (enable matching to load before connection is made)

### **Cost Optimization**
- [ ] Switch sourcing to LCSC as much as possible. Very cheap/great components there.
- [ ] Optimize MOSFET selection for cost/current/RdsON.
  - Each switch only needs to handle 2 (50A / 6) for peak inductor load, or 16.6A.
  - I've selected parts that can handle 80A continuous for $2.72 ea.
- [ ] Find cheaper binding posts for terminal connections that can still handle 50A. Spent $12.50 previously on 4x connectors.
- [ ] Smaller flash memory chip (2x 1MB?). Even the debug software really isn't that big and I don't need much space for logging/PDI.
- [ ] Cheaper buttons. We have so much board space they can be bigger.
- [ ] USB uB Port instead of USB-C. Each USB-C is $1 and we're not even using USB-3.0 speeds.
- [ ] Measure ripple currents under designed peak load and start removing capacitors (not cheap). How much do we really need?
- [ ] Can I buy power inductors cheaper elsewhere, but in the same form factor?
  - Even a toroid that fits in the same space is OK. They can be glued down with high-temp epoxy.

### **Ichnaea**
- [ ] Dead-man switch to disable power output when Sophrosyne isn't connected
- [ ] Add high current, high side load switch to enable safe connection to battery
- [ ] Add an active bleeder resistor to the output to act as a "short" for calibration and system safe-ing. 1k should be sufficient.
- [ ] Improve control resolution on output voltage by varying Ra/Rb.
  - Write a small python program to compute. Targeting 10mV steps 12v-60v output.
  - May want to consider using a digital potentiometer. Could tune the resolution to the target output in extremely fine steps.
  - Alternatively, there may be a purely passive solution that solves the problem.
- [ ] Try to get a single flat plane for component height. Ideally the power IC side of the system is covered in a giant block of aluminium.
- [ ] Design mounts to allow for airflow. We have a bit of a heat problem.
- [ ] 12v/5v capacitors on the bottom of the board were placed too close together. Give space to power capacitors.

### **Sophrosyne**
- [ ] Watchdog circuit to act as a safety net
- [ ] POR latch signal to hold RST low until the system supply voltage reaches 3.3v. Currently having power-on issues when powering direct from solar.
- [ ] Mounting posts for case
- [ ] Ambient temperature sense input. May be acceptable to have an on-board NTC. Used for computing thermal limits.
- [ ] Right angle button inputs to ease case integration.


## Circuit Bugs

- [x] A 100k resistor across the SS pin causes the system to not boot properly. Removing fixes it, but could also go several MOhm(?).
- [ ] A sufficiently cold temperature (5C) on the NTC thermistors can cause the sense output to exceed 3.3v, breaking the ADC input.
- [ ] MCU doesn't boot from solar power application. Have to press reset button.
