#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const int RX_PIN = 3;
const int TX_PIN = 1;
const int BAUD_RATE = 115200;
#define SERVICE_UUID        "0000180d-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002a37-0000-1000-8000-00805f9b34fb"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
std::string commandValue = ""; // Add the declaration of commandValue
bool isDigitalPinValid(int pin);
bool isPinWritable(int pin);

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      commandValue = value;
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);

  BLEDevice::init("ESP32-BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                                      CHARACTERISTIC_UUID,
                                                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE |
                                                      BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void loop() {
  if (deviceConnected) {
    if (!commandValue.empty()) {
      String commandString(commandValue.c_str());
      char command = commandString.charAt(0);
      int pin, value;

      switch (command) {
        case 'R': // READ
          pin = commandString.substring(1).toInt();
          if (isDigitalPinValid(pin)) {
            int status = digitalRead(pin);
            Serial.print("Pin ");
            Serial.print(pin);
            Serial.print(" status: ");
            Serial.println(status);

            if (deviceConnected) {
              char statusString[50];
              sprintf(statusString, "Pin %d status: %d\n", pin, status);
              pCharacteristic->setValue(statusString);
              pCharacteristic->notify();
            }
          }
          break;

        case 'W': // WRITE
          pin = commandString.substring(1, 3).toInt();
          value = commandString.substring(3).toInt();
          if (isDigitalPinValid(pin) && isPinWritable(pin)) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, value);
            Serial.print("Pin ");
            Serial.print(pin);
            Serial.print(" set to ");
            Serial.println(value);

            if (deviceConnected) {
              char setString[50];
              sprintf(setString, "Pin %d set to %d\n", pin, value);
              pCharacteristic->setValue(setString);
              pCharacteristic->notify();
            }
          }
          break;

        case 'A': // READ_ALL
          for (pin = 0; pin <= 39; pin++) {
            if (isDigitalPinValid(pin)) {
              int status = digitalRead(pin);
              Serial.print("Pin ");
              Serial.print(pin);
              Serial.print(" status: ");
              Serial.println(status);

              if (deviceConnected) {
                char statusString[50];
                sprintf(statusString, "Pin %d status: %d\n", pin, status);
                pCharacteristic->setValue(statusString);
                pCharacteristic->notify();
              }
            }
          }
          break;
      }

      commandValue = ""; // Reset command value after processing
    }
  }
}

bool isDigitalPinValid(int pin) {
  // Exclude unusable pins, not available pins, and INPUT only pins
  return !(pin == 6 || pin == 7 || pin == 8 || pin == 9 || pin == 10 || pin == 11 ||
           pin == 34 || pin == 35 || pin == 36 || pin == 39 ||
           pin == 20 || pin == 24 || pin == 28 || pin == 29 ||
           pin == 30 || pin == 31 || pin == 37 || pin == 38);
}

bool isPinWritable(int pin) {
  // Exclude pins that are not writable
  return !(pin == 34 || pin == 35 || pin == 36 || pin == 39);
}
