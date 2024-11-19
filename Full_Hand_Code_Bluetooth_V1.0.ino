#include "arduino_secrets.h"

// The individual fingers will be indexed according to the hex code number input into 'any' external BLE program (I used LightBlue on the Iphone 11)
// The smaller number correlates to the original set position of the servo, while the larger number will move the servo to the preset position

// Servo 1 = Pinky  ; 0(Original Position) & 1(Set Position)
// Servo 2 = Ring   ; 2(Original Position) & 3(Set Position)
// Servo 3 = Middle ; 4(Original Position) & 5(Set Position)
// Servo 4 = Index  ; 6(Original Position) & 7(Set Position)
// Servo 5 = Thumb  ; 8(Original Position) & 9(Set Position)


#include <Servo.h> // Include the servo library
#include <ArduinoBLE.h> // Include the Bluetooth library for Uno 

Servo servos[5]; // Array to hold 5 servo objects
int servoPins[5] = {0, 1, 2, 3, 4}; // Control pins for the servos



// Define the Bluetooth service and characteristic
BLEService controlService("12345678-1234-1234-1234-123456789abc"); // Custom service UUID
BLEByteCharacteristic commandCharacteristic("abcdef01-1234-1234-1234-123456789abc", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);  // Initialize serial communication for debugging
  while (!Serial);

  // Attach the servos to their respective pins and set to original position
  for (int i = 0; i < 5; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(0); // Set each servo to its original position
  }

  if (!BLE.begin()) {  // Initialize Bluetooth
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("EL.UNO.R4.Wi-Fi"); // Set Bluetooth device name
  BLE.setAdvertisedService(controlService); // Advertise the service

  controlService.addCharacteristic(commandCharacteristic); // Add characteristic to the service
  BLE.addService(controlService); // Add service to BLE stack

  commandCharacteristic.writeValue(255); // Initialize characteristic with a default value
  BLE.advertise(); // Start advertising Bluetooth
  Serial.println("BLE device is now advertising...");
}

void loop() {
  BLEDevice central = BLE.central(); // Wait for a central device to connect

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (commandCharacteristic.written()) { // Check if the characteristic is written
        byte receivedValue = commandCharacteristic.value();

        // Control servos based on received value
        for (int i = 0; i < 5; i++) {
          if (receivedValue == i * 2) { // Command to move servo to original position
            Serial.print("Servo ");
            Serial.print(i + 1);
            Serial.println(": Moving to 0 degrees");
            servos[i].write(0);
          } else if (receivedValue == i * 2 + 1) { // Command to move servo to 180 degrees
            Serial.print("Servo ");
            Serial.print(i + 1);
            Serial.println(": Moving to 180 degrees");
            servos[i].write(180);
          }
        }
      }
    }
    Serial.println("Disconnected from central");
  }
}
