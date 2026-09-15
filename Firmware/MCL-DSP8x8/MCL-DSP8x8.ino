// MCL-DSP8x8 Firmware
// Author: Michael Larson
// Created: 3/20/2026
// Last modified: 5/13/2026
// Description: Firmware for ESP32-S3-WROOM-1U to control ADAU1452 and AK4619VN chips for an 8 channel in, 8 channel out DSP device.

// Note: uncomment below for simple ADAU1452 program
// #define ADAU1452_VERIFY

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Wire.h>
#include <SPI.h>
#include "AK4619VN.h"
#include "ADAU1452.h"

// Heartbeat Constants and variables
#define HEARTBEAT_HALF_PERIOD_MS 1000
#define HEARTBEAT_LED 1
bool heartbeatState = HIGH;
int heartbeatTime = 0;

// BLE constants, variables, and objects
// https://www.uuidgenerator.net/
#define SERVICE_UUID "13ddec86-d2ca-4c9f-b360-20804ca3f1b4"
#define CHARACTERISTIC_UUID "13ddec86-d2ca-4c9f-b360-20804ca3f1b4"
BLEServer *pServer;
BLEService *pService;
BLEAdvertising *pAdvertising;
BLECharacteristic *gainCharacteristic;
// BLECharacteristic *


// Functions
void init_BLE();

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial begin success");

  // Initialize Heartbeat LED
  pinMode(HEARTBEAT_LED, OUTPUT);
  heartbeatTime = millis();

  // Initializing AK4619VN
  Serial.println("Programming AK4619VN");
  init_AK4619VN();
  Serial.println("Finished AK4619VN programming sequence");

  // mute during programming to prevent pop on output
  Serial.println("Muting output and programming ADAU1452");
  mute_AK4619VN();
  init_ADAU1452();
  unmute_AK4619VN();
  Serial.println("Finished ADAU1452 programming sequence... unmuting output");

  // Initializing BLE
  init_BLE();
}

void loop() {
  if (millis() > (heartbeatTime + HEARTBEAT_HALF_PERIOD_MS)) {
    heartbeatTime = millis();
    // digitalWrite(HEARTBEAT_LED, HIGH);
    digitalWrite(HEARTBEAT_LED, heartbeatState);

    heartbeatState = !heartbeatState;
// #ifdef ADAU1452_VERIFY
//     String temp_string = gainCharacteristic->getValue();
//     if(!temp_string.equals("X")) {
//       SIGMA_WRITE_REGISTER_FLOAT(MOD_MASTER_GAIN_ALG0_TARGET_ADDR, 0.125*(temp_string.toInt()));
//     }
// #endif
  }
}

void init_BLE() {
  if (!BLEDevice::init("MCL-DSP8x8")) {
    Serial.println("BLE initialization failed!");
    return;
  }

  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pServer->advertiseOnDisconnect(true);
  gainCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  gainCharacteristic->setValue("8");
  pService->start();

  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE Started");
  return;
}
