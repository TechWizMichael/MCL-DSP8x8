# MCL-DSP8x8
## Description
Audio Signal Processor based on ESP32-S3 and ADAU1452.

## Goals
+ Eight channels of analog audio input and output
+ Compatibility with "12V" automotive systems
+ Remote control through ESP32
+ download and upload of settings to restore after flashing new firmware

## Current State of Firmware
+ Bit of a mess. It's been changing as I've been messing with testing.
+ USB programming of ESP32 is functional.
+ ESP32 successfully programs ADAU1452 and AK4619VN.
+ Bluetooth functionality was briefly tested, with a generic BLE app on android controlling output gain.
+ Recommended to edit SigmaStudio project for own needs before programming.

## Current state of hardware
### V0.7 PCB (latest complete version)
- On paper, everything works. The ESP32-S3 may need to be flashed using UART pins for the first time, but it is otherwise functional.
- In practice, there are many issues that make it problematic for use with actual audio systems.
- View issues tab for current issues and progress/solutions


### v0.8 (In Progress)
- Goal to address many of the issues seen in v0.7.
- The primary issue is the noise in the analog signal, which is addressed by a second 3.3V rail for analog components.
- This means the main board has a maximum input and output of around +2.2dBu.
- There is also additional options for input connectors, but they must be added separately.
- The following connectors have complete v1 designs:
  - Stereo XLR
  - Stereo RCA
  - Eight-channel RCA
- Current designs for input boards passively attenuate signal -20dB, so a maximum input signal of +22dBu is acceptable.
- Maximum output is still around +2.2 dBu.
- If the maximum input signal will be under +2.2 dBu, the output boards can be used as input boards.
  - This is because they are direct connections to the main board.

#### v0.8 To-do:
- [x] New analog rail
- [x] Reset IC for ESP32
- [x] Upgrade buttons
- [x] Upgrade/Replace audio connectors
- [x] swap I2S connections for AK4619VN
- [x] \(optional) Ability to power from USB 5V rail
- [x] Update output gain stage
- [x] Add Schottky diode(s) for power input
- [x] Upgrade capacitors on input
- [x] add capacitors on power rails of opamps
- [x] ESP32 controls PDN of AK4619VN
- [x] Updated layout of main board
- [x] Connector boards
- [ ] Validation of assembled design

### Preview of v0.8 from JLCPCB
<img width="760" height="513" alt="image" src="https://github.com/user-attachments/assets/3d379e2b-e514-4c6f-8108-fa303ee4f4f1" />

