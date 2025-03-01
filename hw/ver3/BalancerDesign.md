# Design Notes for Battery Management System

## System Requirements
- BMS module shall support the following chemistries:
  - Lead Acid
  - Lithium Ion
  - Lithium Iron Phosphate
- The BMS module shall support 16 balance channels
- The BMS module shall support a configurable interface over CAN

## Design Goals
- Less than $40/unit cost manufactured. Could sell for $100?
- Multi-Chemistry
- Interoperable with Ichnaea ecosystem
- High balance current to allow it to be used on parallel cell banks.

## Thoughts
- Probably shouldn't allow for daisy chaining b/c future design changes may destroy the interface
  - RP2040 as a controller is cheap enough to put one on each balancer.
- I want it to be low cost, but the core issue is dealing with the ground offsets from multiple cells
  - Cheaper (time/effort) to just use a dedicated part and get the rest as low cost as possible.