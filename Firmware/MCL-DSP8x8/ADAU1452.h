#ifndef ADAU1452
#define ADAU1452

#include <Arduino.h>
#include <SPI.h>
#include "ADAU1452\SigmaStudioFW.h"
#include "ADAU1452\USER_SETTINGS.h"

#ifdef ADAU1452_VERIFY
#include "ADAU1452\ADAU1452_Heartbeat_IC_1.h"
#include "ADAU1452\ADAU1452_Heartbeat_IC_1_PARAM.h"
#else
#include "ADAU1452\MCL-DSP8x8_IC_1.h"
#include "ADAU1452\MCL-DSP8x8_IC_1_PARAM.h"
#endif

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

// Initialize ADAU1452 over SPI
// Pins defined in ADAU1452\USER_SETTINGS.h:
//      IO9  -> DSP_RESET_PIN
//      IO10 -> DSP_SS_PIN
// Default Pins (ESP32-S3-WROOM-1):
//      IO11 -> MOSI
//      IO12 -> SCLK
//      IO13 -> MISO
void init_ADAU1452() {
    digitalWrite(DSP_SS_PIN, HIGH);
    pinMode(DSP_SS_PIN, OUTPUT);
    SPI.begin();
    DSP_HARD_RESET();
    default_download_IC_1();
}

#endif