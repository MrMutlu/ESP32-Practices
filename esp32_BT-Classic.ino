#include <Arduino.h>
#include <BluetoothSerial.h>

const int RX_PIN = 3;
const int TX_PIN = 1;
const int BAUD_RATE = 115200;

BluetoothSerial SerialBT;

bool isDigitalPinValid(int pin);
bool isPinWritable(int pin);

void setup() {
  Serial.begin(BAUD_RATE);
  SerialBT.begin("ESP32-HID"); // Set the Bluetooth device name
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    int pin, value;

    switch (command) {
      case 'R': // READ
        pin = Serial.parseInt();
        if (isDigitalPinValid(pin)) {
          int status = digitalRead(pin);
          Serial.print("Pin ");
          Serial.print(pin);
          Serial.print(" status: ");
          Serial.println(status);
          SerialBT.print("Pin ");
          SerialBT.print(pin);
          SerialBT.print(" status: ");
          SerialBT.println(status);
        }
        break;

      case 'W': // WRITE
        pin = Serial.parseInt();
        value = Serial.parseInt();
        if (isDigitalPinValid(pin) && isPinWritable(pin)) {
          pinMode(pin, OUTPUT);
          digitalWrite(pin, value);
          Serial.print("Pin ");
          Serial.print(pin);
          Serial.print(" set to ");
          Serial.println(value);
          SerialBT.print("Pin ");
          SerialBT.print(pin);
          SerialBT.print(" set to ");
          SerialBT.println(value);
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
            SerialBT.print("Pin ");
            SerialBT.print(pin);
            SerialBT.print(" status: ");
            SerialBT.println(status);
          }
        }
        break;
    }
  }

  if (SerialBT.available()) {
    char command = SerialBT.read();
    int pin, value;

    switch (command) {
      case 'R': // READ
        pin = SerialBT.parseInt();
        if (isDigitalPinValid(pin)) {
          int status = digitalRead(pin);
          Serial.print("Pin ");
          Serial.print(pin);
          Serial.print(" status: ");
          Serial.println(status);
          SerialBT.print("Pin ");
          SerialBT.print(pin);
          SerialBT.print(" status: ");
          SerialBT.println(status);
        }
        break;

      case 'W': // WRITE
        pin = SerialBT.parseInt();
        value = SerialBT.parseInt();
        if (isDigitalPinValid(pin) && isPinWritable(pin)) {
          pinMode(pin, OUTPUT);
          digitalWrite(pin, value);
          Serial.print("Pin ");
          Serial.print(pin);
          Serial.print(" set to ");
          Serial.println(value);
          SerialBT.print("Pin ");
          SerialBT.print(pin);
          SerialBT.print(" set to ");
          SerialBT.println(value);
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
            SerialBT.print("Pin ");
            SerialBT.print(pin);
            SerialBT.print(" status: ");
            SerialBT.println(status);
          }
        }
        break;
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
