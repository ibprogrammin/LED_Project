#ifndef BLESERVICEADAPTER_H
#define BLESERVICEADAPTER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Change these UUIDs to unique values for BLEService
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class BLEServiceAdapter {
    public:
    BLEServiceAdapter();
    void load();
    void Run();

    private:
    // Add any private member variables or functions here
    BLEServer *pServer;
    BLEService *pService;
};


#endif