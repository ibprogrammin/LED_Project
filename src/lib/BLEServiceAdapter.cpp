#include "BLEServiceAdapter.h"

BLEServiceAdapter::BLEServiceAdapter() {
    // Constructor implementation (if needed)
}

void BLEServiceAdapter::load() {
    // Initialize BLE device, server, service, and characteristics here

    BLEDevice::init("LED Controller"); // Initialize the BLE device with a name
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_WRITE
                                        );

    pCharacteristic->setValue("Hello World says Neil");
    pService->start();

    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void BLEServiceAdapter::Run() {
    // Implement any runtime behavior for the BLE service here (if needed)
}