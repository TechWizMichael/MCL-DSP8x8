// MCL-DSP8x8 Firmware
// Author: Michael Larson
// Created: 3/20/2026
// Last modified: 3/23/2026
// Description: Firmware for ESP32-S3-WROOM-1U to control ADAU1452 and AK4619 chips for an 8 channel in, 8 channel out DSP device.

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "AK4619VN.h"
#include "ADAU1452\SigmaStudioFW.h"
#include "ADAU1452\USER_SETTINGS.h"

// Note: uncomment below for simple ADAU1452 program that pulses ADAU1452 MP7 pin
// #define ADAU1452_VERIFY

#ifdef ADAU1452_VERIFY
#include "ADAU1452\ADAU1452_Heartbeat_IC_1.h"
#include "ADAU1452\ADAU1452_Heartbeat_IC_1_PARAM.h"
#else
#include "ADAU1452\MCL-DSP8x8_IC_1.h"
#include "ADAU1452\MCL-DSP8x8_IC_1_PARAM.h"
#endif

#define HEARTBEAT_HALF_PERIOD_MS 1000
#define HEARTBEAT_LED 1
#define I2C_SDA 4
#define I2C_SCL 5

// Function prototypes
void DSP_HARD_RESET();  // Reset ADAU1452 and prepare for programming
void init_ADAU1452();   // Initialize ADAU1452 over SPI
void init_AK4619VN();  // Initialize AK4619VN over I2C

bool heartbeatState = HIGH;
int heartbeatTime = 0;

void setup() {
    Serial.begin(115200);
    // Initialize Heartbeat LED
    pinMode(HEARTBEAT_LED, OUTPUT);
    heartbeatTime = millis();

    init_AK4619VN();
    init_ADAU1452();
}

void loop() {
    if (millis() > (heartbeatTime + HEARTBEAT_HALF_PERIOD_MS)) {
        heartbeatTime = millis();
        digitalWrite(HEARTBEAT_LED, heartbeatState);
        heartbeatState = !heartbeatState;
        // init_AK4619VN();
    }
}

// Initialize ADAU1452 over SPI
// Pins defined in ADAU1452\USER_SETTINGS.h:
//      IO9  -> DSP_RESET_PIN
//      IO10 -> DSP_SS_PIN
// Default Pins (ESP32-S3-WROOM-1):
//      IO11 -> MOSI
//      IO12 -> SCLK
//      IO13 -> MISO
void init_ADAU1452() {
    delay(1000);
    Serial.begin(115200);
    digitalWrite(DSP_SS_PIN, HIGH);
    pinMode(DSP_SS_PIN, OUTPUT);
    SPI.begin();
    DSP_HARD_RESET();
    default_download_IC_1();
}

// Reset ADAU1452 and prepare for programming
// Note: Copied from example Analog Devices code found here: https://wiki.analog.com/resources/tools-software/sigmastudio/tutorials/microcontroller
// Modified to remove code for I2C. Device is controlled solely through SPI.
void DSP_HARD_RESET() {
    pinMode(DSP_RESET_PIN, OUTPUT);
    digitalWrite(DSP_RESET_PIN, LOW);
    delay(10);
    digitalWrite(DSP_RESET_PIN, HIGH);
    // This delay accounts for ADM811T reset generator hold time on ADAU1467 eval board, adjust
    // as needed
    delay(560);

    // Toggle the DSP into SPI mode by pulling the /SS pin low, then high, 3 times.
    for (uint8_t i = 0; i < 3; i++) {
        digitalWrite(DSP_SS_PIN, LOW);
        delayMicroseconds(10);
        digitalWrite(DSP_SS_PIN, HIGH);
        delayMicroseconds(10);
    }
}

// Write value to register on AK4619VN
uint8_t write_AK4619VN(uint8_t device_address, byte register_address, byte register_value) {
    Wire.beginTransmission(device_address);
    Wire.write(register_address);
    Wire.write(register_value);
    
    uint8_t transmission_result = Wire.endTransmission(true);
    
    String message = "transmitting 0x" + String(int(register_value), HEX) + " to register 0x" + String(int(register_address), HEX);
    Serial.println(message);

    Serial.println("0x" + String(int(transmission_result), HEX));

    return transmission_result;
}

// Read current value on specific AK4619VN register
uint8_t read_AK4619VN(uint8_t device_address, byte register_address, uint8_t * register_value) {
    Wire.beginTransmission(device_address);
    Wire.write(register_address);
    Wire.endTransmission(true);
    
    uint8_t numbytes = 0;
    numbytes = Wire.requestFrom(device_address, (uint8_t)1, (uint8_t)false);
    
    if((bool)numbytes){
        Wire.readBytes(register_value, numbytes);
    }

    uint8_t transmission_result = Wire.endTransmission(true); //Send STOP

    Serial.println("0x" + String(int(transmission_result), HEX));

    return transmission_result;
}

// Initialize AK4619VN1 over I2C
// NOTE: many settings maintain their default values, but having them here already makes changes easier
void init_AK4619VN() {
    Wire.begin(I2C_SDA, I2C_SCL); // SDA, SCL
    Wire.setClock(400000); // 400 kHz (Fast Mode)
    byte temp_data = 0x00;
    byte temp_address = 0x00;

    // Reset both AK4619VN
    // Clearing last bit (RSTN)

    temp_address = PWRMGM; // {0, 0, PMAD2, PMAD1, 0, PMDA2, PMDA1, RSTN}
    read_AK4619VN(AK4619VN_1_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data & 0xFE;
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    read_AK4619VN(AK4619VN_2_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data & 0xFE;
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);
    
    // ADC/DAC Power
    // Set bits for ADC and DAC power (PMAD2, PMAD1, PMDA2, PMDA1)
    // HIGH (1) means enabled

    temp_address = PWRMGM; // {0, 0, PMAD2, PMAD1, 0, PMDA2, PMDA1, RSTN}
    temp_data = 0x36; // ADC and DAC on
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Audio Format Registers
    // TDM128 with four channels, 32-bit slot length, 24-bit data in and out, negative polatity on BICK, slow SDOUT.
    // Note: MSB first, left justified, delayed by 1 BICK period.

    temp_address = AUDFORM1; // {TDM, DCF[2], DCF[1], DCF[0], DSL[1], DSL[0], BCKP, SDOPH}
    temp_data = 0x8C; // TDM, I2S Compatible, 32 bit slot, falling edge of BICK, SDOUT slow mode
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = AUDFORM2; // {0, 0, 0, SLOT, DIDL[1], DIDL[0], DODL[1], DODL[0]}
    temp_data = 0x00; // Data start on LRCLK edge, 24 bit date in, 24-bit data out
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Clock Speed

    temp_address = SYSCLKSET; // {0, 0, 0, 0, 0, FS[2], FS[1], FS[0]}
    temp_data = 0x00; // MCLK = 256*Fs, BICK <= 48kHz
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);
    
    // Analog Input Gain
    // labeled as microphone gain in datasheet.
    // each register holds two gain values (4 bits each).
    // adjusted in 3dB steps.
    //      0x0 = +6dB
    //      0x2 = 0dB
    //      0xF = -24dB

    temp_address = MICGAIN1; // {MGN1L[3], MGN1L[2], MGN1L[1], MGN1L[0], MGN1R[3], MGN1R[2], MGN1R[1], MGN1R[0]}
    temp_data = 0x22; // 0dB gain on both channels
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = MICGAIN2; // {MGN2L[3], MGN2L[2], MGN2L[1], MGN2L[0], MGN2R[3], MGN2R[2], MGN2R[1], MGN2R[0]}
    temp_data = 0x22; // 0dB gain on both channels
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Digital Input Gain
    // Each Register holds one gain value (8 bits each).
    // adjust in 0.5dB steps.
    //      0x00 = +24.0dB
    //      0x30 = 0.0dB
    //      0xFE = -103.0dB
    //      0xFF = MUTE

    temp_address = ADC1LVOL; // {VOLAD1L[7], VOLAD1L[6], VOLAD1L[5], VOLADL1[4], VOLADL1[3], VOLADL1[2], VOLADL1[1], VOLADL1[0]}
    temp_data = 0x30; // 0dB digital gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = ADC1RVOL; // {VOLAD1R[7], VOLAD1R[6], VOLAD1R[5], VOLAD1R[4], VOLAD1R[3], VOLAD1R[2], VOLAD1R[1], VOLAD1R[0]}
    temp_data = 0x30; // 0dB digital gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = ADC2LVOL; // {VOLAD2L[7], VOLAD2L[6], VOLAD2L[5], VOLAD2L[4], VOLAD2L[3], VOLAD2L[2], VOLAD2L[1], VOLAD2L[0]}
    temp_data = 0x30; // 0dB digital gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = ADC2RVOL; // {VOLAD2R[7], VOLAD2R[6], VOLAD2R[5], VOLAD2R[4], VOLAD2R[3], VOLAD2R[2], VOLAD2R[1], VOLAD2R[0]}
    temp_data = 0x30; // 0dB digital gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set ADC input filter
    // ADxVO: Enable "voice" filter (if set, other bits are ignored)
    // ADxSD: Enable short delay (possibly minimum-phase vs linear-phase?)
    // ADxSL: Enable slow roll-off

    temp_address = ADCFILT; // {0, AD2VO, AD2SD, AD2SL, 0, AD1VO, AD1SD, AD1SL}
    temp_data = 0x00; // sharp roll-off on both channels
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set ADC input mode

    temp_address = ADCAIN; // {ADC1LSEL[1], ADC1LSEL[0], ADC1RSEL[1], ADC1RSEL[0], ADC2LSEL[1], ADC2LSEL[0], ADC2RSEL[1], ADC2RSEL[0]}
    temp_data = 0x00; // differential on all ADC inputs
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set Mute and HPF on ADC

    temp_address = ADCMUTEHPF; // {ATSPAD, AD2MUTE, AD1MUTE, 0, 0, AD2HPF, AD1HPF, 0}
    temp_data = 0x86; // Slow transition, unmute both ADC, enable HPF on both ADC
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set DAC digital gain

    temp_address = DAC1LVOL; // {VOLDA1L[7], VOLDA1L[6], VOLDA1L[5], VOLDA1L[4], VOLDA1L[3], VOLDA1L[2], VOLDA1L[1], VOLDA1L[0]}
    temp_data = 0x18; // 0dB gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = DAC1RVOL; // {VOLDA1R[7], VOLDA1R[6], VOLDA1R[5], VOLDA1R[4], VOLDA1R[3], VOLDA1R[2], VOLDA1R[1], VOLDA1R[0]}
    temp_data = 0x18; // 0dB gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = DAC2LVOL; // {VOLDA12L[7], VOLDA2L[6], VOLDA2L[5], VOLDA2L[4], VOLDA2L[3], VOLDA2L[2], VOLDA2L[1], VOLDA2L[0]}
    temp_data = 0x18; // 0dB gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    temp_address = DAC2RVOL; // {VOLDA2R[7], VOLDA2R[6], VOLDA2R[5], VOLDA2R[4], VOLDA2R[3], VOLDA2R[2], VOLDA2R[1], VOLDA2R[0]}
    temp_data = 0x18; // 0dB gain
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set DAC input source

    temp_address = DACDIN; // {0, 0, 0, 0,  DAC2SEL[1], DAC2SEL[0], DAC1SEL[1], DAC1SEL[0]}
    temp_data = 0x05; // SDIN1 on both outputs
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // DAC De-Emphasis

    temp_address = DACDEEM; // {0, 0, 0, 0, DEM2[1], DEM2[0], DEM1[1], DEM1[0]}
    temp_data = 0x05; // Disable de-emphasis
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);
    

    // DAC Mute and Filter

    temp_address = DACMUTFLT; // {ATSPDA, 0, DA2MUTE, DA1MUTE, DA2SD, DA2SL, DA1SD, DA1SL}
    temp_data = 0x80; // Slow transition. Mute off. Filters with sharp roll off, long delay.
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    // Set RSTN to re-enable devices

    temp_address = PWRMGM; // {0, 0, PMAD2, PMAD1, 0, PMDA2, PMDA1, RSTN}
    read_AK4619VN(AK4619VN_1_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data | 0x01;
    write_AK4619VN(AK4619VN_1_ADDRESS, temp_address, temp_data);
    read_AK4619VN(AK4619VN_2_ADDRESS, temp_address, &temp_data);
    temp_data = temp_data | 0x01;
    write_AK4619VN(AK4619VN_2_ADDRESS, temp_address, temp_data);

    return;
}