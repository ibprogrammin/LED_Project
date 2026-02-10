#ifndef BLESERVICEADAPTER_H
#define BLESERVICEADAPTER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUIDs for BLEService
#define SERVICE_UUID        "2b629673-93b1-4324-902c-4dc4db49b435"
#define CHARACTERISTIC_UUID "0ef370b3-d434-447a-b4da-7c5b20f6a248"

// BLEServiceAdapter class definition
class BLEServiceAdapter {
    public:
    BLEServiceAdapter();
    void load();
    void Run();
    void SendValue(std::string value);

    private:
    // Add any private member variables or functions here
    BLEServer *pServer;
    BLEService *pService;
    BLECharacteristic *pCharacteristic;
};

#endif