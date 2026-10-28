# MCL-DSP8x8
## Description
Audio Signal Processor based on ESP32-S3 and ADAU1452.

## Goals
+ Eight channels of analog audio input and output
+ Compatibility with "12V" automotive systems
  + As of v0.8 PCB, the device is powered using USB, so 12V operation will require an external USB or 5V power converter.
+ WIP: Remote control through ESP32 (Serial and basic controls through BLE).
+ WIP: Download and upload of settings to restore after flashing new firmware.

## Current State of Firmware
+ Bit of a mess. It's been changing as I've been messing with testing.
+ USB programming of ESP32 is functional.
+ ESP32 successfully programs ADAU1452 and AK4619VN.
+ Bluetooth functionality was briefly tested, with a generic BLE app on android controlling output gain.
+ Can choose between default and custom DSP program by commenting/uncommenting a line in the main .INO file.
  + Default DSP is linear mapping (pass-through) of input to output until ESP32 updates settings after flashing.
    + Defualt will be guaranteed to be compatible with remote control.
      + For this reason, it is highly recommended that the default DSP project, code, and headers remain unchanged.
  + Custom DSP is provided example with registers and input/outputs correctly mapped to physical connections.
    + The Custom DSP program can be modified, but will not offer remote control through the ESP32.
    + The Custom DSP "example" in this repository will likely be whatever I programmed last with it.

## Current state of hardware
### v0.8 (In Testing)
- Important: Buttons are connected incorrectly so they behave as though they are permanently pressed.
  - Removing the buttons or not getting them placed will fix the issue, but the relevant pins may need to be shorted for first-time flashing firmware.
- Maximum input and output around 3.2V peak-peak (+3.3dBu) (to be verified).
- Using passive attenuation, input range can be expanded.
  - For example, a voltage divider consisting of 47k and 5.6k can be used to achieve 20dB attenuation.
- Analog inputs and outputs use pinheaders with 5 pins with 2.54mm pitch.
  - Each connector carries a differential/balanced connection for two channels.
  - GND, A+, A-, B+, B-
- Similar connectors offer connection to Multi-purpose and AUXADC pins on ADAU1452.
- All appears to be functional with minimal noise

### Preview of v0.8 from JLCPCB
<img width="674" height="442" alt="MCL-DSP8x8 v0 8 Preview" src="https://github.com/user-attachments/assets/f40b6a92-9971-4932-a340-858c563d7075" />
