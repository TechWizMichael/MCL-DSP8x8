// MCL-DSP8x8 Firmware
// Author: Michael Larson
// Created: March 20, 2026
// Last modified: October 28, 2026

// Note: uncomment below for simple ADAU1452 program
// #define CUSTOM_DSP

// Generate unique UUIDs here: https://www.uuidgenerator.net/
#define SERVICE_UUID "13ddec86-d2ca-4c9f-b360-20804ca3f1b4"
#define CHARACTERISTIC_UUID "13ddec86-d2ca-4c9f-b360-20804ca3f1b4"

#include <Arduino.h>
#include "MCL-DSP_SERIAL.h"
#include "MCL-DSP_BLE.h"
#include "MCL-DSP_AK4619VN.h"
#include "MCL-DSP_Programming.h"

void setup() {
  setup_serial();
  setup_heartbeat();
  setup_ADAU1452();
  setup_AK4619VN();
  setup_BLE();
  setup_param();
}

void loop() {
  process_heartbeat();
  serial_respond(command_ADAU1452(read_serial()));
  ble_respond(command_ADAU1452(read_BLE()));
}