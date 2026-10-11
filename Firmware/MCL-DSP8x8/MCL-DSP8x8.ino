// MCL-DSP8x8 Firmware
// Author: Michael Larson
// Created: 3/20/2026
// Last modified: 5/13/2026
// Description: Firmware for ESP32-S3-WROOM-1U to control ADAU1452 and AK4619VN chips for an 8 channel in, 8 channel out DSP device.

// Note: uncomment below for simple ADAU1452 program
// #define CUSTOM_DSP

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <USB.h>
// #include "BLE.h"
#include "AK4619VN.h"
#include "ADAU1452.h"

// Heartbeat Constants and variables
#define HEARTBEAT_HALF_PERIOD_MS 1000
#define HEARTBEAT_LED 1
bool heartbeatState = HIGH;
int heartbeatTime = 0;

// AK4619VN PDN/enable pin
#define CODEC_EN 14

// USB Communication and handshake
const char* handshake_cmd =  "opensesame";
const char* handshake_response =  "behold";

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial begin success");

  pinMode(CODEC_EN, OUTPUT);
  digitalWrite(CODEC_EN, LOW);

  // Initialize Heartbeat LED
  pinMode(HEARTBEAT_LED, OUTPUT);
  heartbeatTime = millis();

  // mute during programming to prevent pop on output
  Serial.println("Muting output and programming ADAU1452");
  digitalWrite(CODEC_EN, HIGH);
  mute_AK4619VN();
  init_ADAU1452();
  unmute_AK4619VN();
  Serial.println("Finished ADAU1452 programming sequence... unmuting output");
}

void loop() {
  if (millis() > (heartbeatTime + HEARTBEAT_HALF_PERIOD_MS)) {
    heartbeatTime = millis();
    // digitalWrite(HEARTBEAT_LED, HIGH);
    digitalWrite(HEARTBEAT_LED, heartbeatState);

    heartbeatState = !heartbeatState;
  }

  if(Serial.available()){
    String input = Serial.readStringUntil('\n');

    if (input == handshake_cmd) {
      Serial.println(handshake_response);
    }
  }
}