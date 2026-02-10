#include "BLEServiceAdapter.h"

#define SERVER_NAME "LED Controller"

bool deviceConnected = false; // Global variable to track connection status

//Setup callbacks onConnect and onDisconnect
class DeviceServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class DeviceCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = String(pCharacteristic->getValue().c_str());

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
      }

      Serial.println();
      Serial.println("*********");
    }
  }
};

BLEServiceAdapter::BLEServiceAdapter() {
    // Constructor implementation (if needed)
}

void BLEServiceAdapter::load() {
    // Initialize BLE device, server, service, and characteristics here

    BLEDevice::init(SERVER_NAME); // Initialize the BLE device with a name

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new DeviceServerCallbacks()); // Set the server callbacks for connection events
    
    // Create the BLE Service
    pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
                                    CHARACTERISTIC_UUID,
                                    BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_WRITE
                                );


    pCharacteristic->setCallbacks(new DeviceCallbacks()); // Set the characteristic callbacks for write events
    pCharacteristic->setValue("Hello World says Neil");

    pService->start();

    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    //BLEDevice::startAdvertising();
    pServer->getAdvertising()->start();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void BLEServiceAdapter::Run() {
    // Implement any runtime behavior for the BLE service here (if needed)
}

void BLEServiceAdapter::SendValue(std::string value) {
    Serial.println("Sending value to BLE clients: " + String(value.c_str()));
    // Implement a method to send a value to connected BLE clients (if needed)
    pService->getCharacteristic(CHARACTERISTIC_UUID)->setValue(value);
    pService->getCharacteristic(CHARACTERISTIC_UUID)->notify(); // Notify connected clients of the new value
}