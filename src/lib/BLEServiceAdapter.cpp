#include "BLEServiceAdapter.h"
#include "LightState.h"

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
        
        pCharacteristic->setValue(pCharacteristic->getValue().c_str()); // Echo the received value back to the characteristic

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

void BLEServiceAdapter::init() {
    // Initialize BLE device, server, service, and characteristics here

    Serial.println("Initializing BLE device...");
    BLEDevice::init(SERVER_NAME); // Initialize the BLE device with a name

    // Create the BLE Server
    Serial.println("Creating BLE server...");
    pServer = BLEDevice::createServer();
    Serial.println("BLE server created successfully");
    Serial.println("Setting BLE server callbacks...");
    pServer->setCallbacks(new DeviceServerCallbacks()); // Set the server callbacks for connection events
    
    // Create the BLE Service
    Serial.println("Creating BLE service...");
    pService = pServer->createService(SERVICE_UUID);

    Serial.println("BLE service created successfully");
    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
                                    CHARACTERISTIC_UUID,
                                    BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_WRITE
                                );


    
    // Set the characteristic callbacks for write events
    Serial.println("Setting BLE characteristic callbacks");
    pCharacteristic->setCallbacks(new DeviceCallbacks()); // Set the characteristic callbacks for write events
    Serial.println("BLE characteristic callbacks set successfully");
    // Set an initial value for the characteristic
    pCharacteristic->setValue("Hello World says Neil");

    Serial.println("Starting BLE service...");
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

// Function to send a value to connected BLE clients
void BLEServiceAdapter::SendValue(std::string value) {
    Serial.println("Sending value to BLE clients: " + String(value.c_str()));
    if (deviceConnected) {
        Serial.println("Device is connected, sending value...");
        pCharacteristic->setValue(value);
        pCharacteristic->notify(); // Notify connected clients of the new value
    } else {
        Serial.println("No device connected, cannot send value");
    }
}

BLECommand BLEServiceAdapter::parseCommand(const std::string& command) {
    BLECommand result;

    size_t pos = 0;
    while (pos < command.length()) {
        char identifier = command[pos];
        
        switch (identifier) {
            case 'M':
                // Extract mode value (single digit)
                if (pos + 1 < command.length() && std::isdigit(command[pos + 1])) {
                    int modeValue = command[pos + 1] - '0';
                    result.state = static_cast<LightState>(modeValue);
                    pos += 2;
                } else {
                    Serial.println("Invalid light state format");
                }
                break;
                
            case 'R':
                // Extract R value (3 digits)
                if (pos + 3 < command.length()) {
                    result.r = std::stoi(command.substr(pos + 1, 3));
                    pos += 4;
                } else {
                    Serial.println("Invalid RGB format");
                }
                break;
                
            case 'G':
                // Extract G value (3 digits)
                if (pos + 3 < command.length()) {
                    result.g = std::stoi(command.substr(pos + 1, 3));
                    pos += 4;
                } else {
                    Serial.println("Invalid RGB format");
                }
                break;
                
            case 'B':
                // Extract B value (3 digits)
                if (pos + 3 < command.length()) {
                    result.b = std::stoi(command.substr(pos + 1, 3));
                    pos += 4;
                } else {
                    Serial.println("Invalid RGB format");
                }
                break;
                
            default:
                Serial.println("Unknown identifier: " + identifier);
        }
    }
    
    return result;
}