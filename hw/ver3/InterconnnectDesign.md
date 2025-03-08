## Communication Interface
- Single RJ45 connector to distribute the data and power
  - Pin 1: RS485 A1 (orange/white)
  - Pin 2: RS485 B1 (orange)
  - Pin 3: RS485 A2 (green/white)
  - Pin 4: +48v (blue/white)
  - Pin 5: +48v (blue)
  - Pin 6: RS485 B2 (green)
  - Pin 7: GND (brown/white)
  - Pin 8: GND (brown)
- Uses a multi-drop configuration to allow the controller to communicate with all nodes (up to 255)
- RS485 Bus 1 is the primary interface
- RS485 Bus 2 is the backup for many nodes
- Each node in the system should have selectable pin headers for which bus it's connected to
- Resiliant to accidental cross-over cable usage. RS485 channels become swapped, which will likely
    break communication, but the power wires stay the same. This should prevent magic smoke release.

## Resolving System Addressing
- Ask Grok. Had some interesting things to say about random response times and resolve collisions
    using serial numbers and explicit commands to back off delay windows.
- Addresses are re-resolved each time the brain boots up
- Need to solve discovery of a node that joins in the middle of a running system.
  - Ah, use periodic "discover" messages. Only non-assigned nodes respond.