// MCL-DSP8x8 Firmware
// Author: Michael Larson
// Created: 3/20/2026
// Last modified: 5/13/2026
// Description: Firmware for ESP32-S3-WROOM-1U to control ADAU1452 and AK4619VN chips for an 8 channel in, 8 channel out DSP device.

// Note: uncomment below for simple ADAU1452 program that pulses ADAU1452 MP7 pin
// #define ADAU1452_VERIFY

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "AK4619VN.h"
#include "ADAU1452.h"

#define HEARTBEAT_HALF_PERIOD_MS 1000
#define HEARTBEAT_LED 1

bool heartbeatState = HIGH;
int heartbeatTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Serial begin success");
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
        init_AK4619VN();
    }
}
