# Version 3 Notes

## **Major Design Flaw**
I think I may have overlooked something. I need precise voltage control to charge a battery, right? The voltage feedback loop
for the LTC7871 doesn't allow me to get fine control in the voltage ranges I need, i.e. 48V. The steps are roughly 0.47V each.
I need to somehow achieve a very fine level of control over that feedback loop, or else I'm screwed with this design.

Is this a valid assumption? I'm not sure. At the worst case, I lose the ability to fully charge the pack because I can't get
close enough to the max charge voltage. That may be ok.

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
- [ ] Smaller flash memory chip (2x 1MB?). Even the debug software really isn't that big and I don't need much space for logging/PDI.
- [ ] Cheaper buttons. We have so much board space they can be bigger.
- [ ] USB uB Port instead of USB-C. Each USB-C is $1 and we're not even using USB-3.0 speeds.

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

#### **Cost Optimization**
- [ ] Measure ripple currents under designed peak load and start removing capacitors (not cheap). How much do we really need?
  - Current BOM is 44x 100v 10uF MLCC @$0.31 ea: $13.64
  - I think I could use electrolytics + ceramics and get the cost down.
- [ ] Optimize MOSFET selection for cost/current/RdsON.
  - Each switch only needs to handle 2 (50A / 6) for peak inductor load, or 16.6A.
  - I've selected parts that can handle 80A continuous for $2.72 ea -> $32.64
  - [HYG053N10](https://www.lcsc.com/product-detail/MOSFETs_HUAYI-HYG053N10NS1C2_C2986239.html) is $0.31 per 10+.
    - 16x == $4.96
- [x] ~~Can I buy power inductors cheaper elsewhere, but in the same form factor?~~
  - Previously spent $3.12 ea x6 == $18.72
  - Even a toroid that fits in the same space is OK. They can be glued down with high-temp epoxy.
  - May need to increase switching frequency to drop the inductance requirement. Ran 1.15kW at 150kHz.
  - [1.0uH 32A, 54A Sat](https://www.lcsc.com/product-detail/Power-Inductors_SHOU-HAN-CYA1265-1-0UH_C19268663.html)
    - $0.63 each
    - 13.8mm x 12.8mm
  - Starting to realize this will really screw with my system design.
- [ ] Are there cheaper gate drivers? Previously spent $1.83/10x == $18.30.
  - Legitimately struggling to find solutions that can meet deadtime + voltage requirements. May be best option.
  - Challenge the requirements. Do I actually need 3A src/sink? Much cheaper chips can be had for dropping some current "requirements"
- [ ] Find cheaper binding posts for terminal connections that can still handle 50A. Spent $12.50 previously on 4x connectors.
  - $1.50 for 2x [76A 1x2P Barrier Terminal Block](https://www.lcsc.com/product-detail/Barrier-Terminal-Blocks_Cixi-Kefa-Elec-KF88SA-16-0-2P_C707749.html)
  - They have a 4-terminal variant for $1, but not very much stock at all


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
