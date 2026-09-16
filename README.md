# MCL-DSP8x8
## Description
Audio Signal Processor based on ESP32-S3 and ADAU1452. 
Description last updated September 16, 2026

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
- [x] New analog rail
- [x] Reset IC for ESP32
- [ ] Upgrade buttons
- [ ] Upgrade/Replace audio connectors
- [x] swap I2S connections for AK4619VN
- [x] \(optional) Ability to power from USB 5V rail
- [ ] Update output gain stage
- [x] Add Schottky diode(s) for power input
- [x] Upgrade capacitors on input
- [ ] add capacitors on power rails of opamps
- [ ] ESP32 controls PDN of AK4619VN
- [ ] \(optional) Input Gain
