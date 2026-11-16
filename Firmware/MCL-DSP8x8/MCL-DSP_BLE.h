// Values and functions for using BLE
// Largely copied from https://randomnerdtutorials.com/esp32-web-bluetooth/
// Last updated October 28, 2026

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define BLE_HANDSHAKE_QUERY "secret"
#define BLE_HANDSHAKE_RESPONSE "hush hush"

BLEServer *pServer;
BLEService *pService;
BLEAdvertising *pAdvertising;
BLECharacteristic *pCharacteristic;
String latest_response = "READY";

void setup_BLE() {
  if (!BLEDevice::init("MCL-DSP8x8")) {
    Serial.println("BLE initialization failed!");
    return;
  }

  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pServer->advertiseOnDisconnect(true);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue(latest_response);
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

String read_BLE(void){
  String temp_string = pCharacteristic->getValue();

  if(!latest_response.equals(temp_string)){
    return temp_string;
  }

  return "";
}

void ble_respond(String response){
  if(!response.equals("NOCOMMAND")){
    latest_response = response;
    pCharacteristic->setValue("");
  }

  return;
}