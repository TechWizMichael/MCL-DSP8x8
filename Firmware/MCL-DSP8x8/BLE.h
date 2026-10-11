// Values and functions for using BLE
// Largely copied from https://randomnerdtutorials.com/esp32-web-bluetooth/
// Last updated September 29, 2026

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// BLE constants, variables, and objects
// https://www.uuidgenerator.net/
#define SERVICE_UUID "13ddec86-d2ca-4c9f-b360-20804ca3f1b4"
#define CHARACTERISTIC_UUID "13ddec86-d2ca-4c9f-b360-20804ca3f1b4"
BLEServer *pServer;
BLEService *pService;
BLEAdvertising *pAdvertising;
BLECharacteristic *pCharacteristic;


void init_BLE() {
  if (!BLEDevice::init("MCL-DSP8x8")) {
    Serial.println("BLE initialization failed!");
    return;
  }

  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pServer->advertiseOnDisconnect(true);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("8");
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
