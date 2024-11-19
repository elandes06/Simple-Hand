// The individual fingers will be indexed according to the hex code number input into 'any' external BLE program (I used LightBlue on the iPhone 11)
// The smaller number correlates to the original set position of the servo, while the larger number will move the servo to the preset position

// Power LED is located on pin 5 
// Bluetooth LED is located on pin 6, and lights when connected to an external system

// On startup servos are actuated 1-5 in numerical order, once the previous servo reaches half rotation the next servo starts 
// Once complete all servos go to the point of origin
// Servo 1 = Pinky  ; 0(Original Position) & 1(Set Position)
// Servo 2 = Ring   ; 2(Original Position) & 3(Set Position)
// Servo 3 = Middle ; 4(Original Position) & 5(Set Position)
// Servo 4 = Index  ; 6(Original Position) & 7(Set Position)
// Servo 5 = Thumb  ; 8(Original Position) & 9(Set Position)
// Hex Code 10 = Full Fist (1,2,3,4,5)
// Hex Code 11 = Peace (1,2,5)
// Hex Code 12 = Frank (1,2,4,5)
// Hex Code 13 = Evan (2,3)
// Hex Code 14 = Thumbs Up (1,2,3,4)
// Hex Code 15 = Eli (2,3,4)
// Hex Code 16 = Reset ()

#include <Servo.h>
#include <ArduinoBLE.h> // Include the Bluetooth library for Uno R4 WiFi

Servo servos[5]; // Array to hold 5 servo objects
int servoPins[5] = {0, 1, 2, 3, 4}; // Control pins for the servos

// LED pins
const int powerLED = 5;
const int bluetoothLED = 6;

// Define the Bluetooth service and characteristic
BLEService controlService("12345678-1234-1234-1234-123456789abc"); // Custom service UUID
BLEByteCharacteristic commandCharacteristic("abcdef01-1234-1234-1234-123456789abc", BLERead | BLEWrite);

void startupSequence() {
  // Sequentially actuate servos 1-5, waiting for each to reach 90 degrees
  for (int i = 0; i < 5; i++) {
    Serial.print("Actuating Servo ");
    Serial.println(i + 1);
    servos[i].write(180); // Actuate servo to full range

    // Wait for servo to reach 90 degrees (approximation)
    delay(500); // Adjust delay if servos are slower/faster
  }

  // Reset all servos to original position
  Serial.println("Resetting all servos to original position...");
  for (int i = 0; i < 5; i++) {
    servos[i].write(0); // Move servo back to 0 degrees
  }

  // Wait to ensure all servos return to original position
  delay(1000);
}

void setup() {
  Serial.begin(9600);  // Initialize serial communication for debugging
  while (!Serial);

  // Configure LED pins
  pinMode(powerLED, OUTPUT);
  pinMode(bluetoothLED, OUTPUT);

  // Turn on power LED
  digitalWrite(powerLED, HIGH);

  // Attach the servos to their respective pins and set to original position
  for (int i = 0; i < 5; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(0); // Set each servo to its original position
  }

  // Perform startup sequence
  startupSequence();

  if (!BLE.begin()) {  // Initialize Bluetooth
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("MultiServoController"); // Set Bluetooth device name
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

    // Turn on Bluetooth connection LED
    digitalWrite(bluetoothLED, HIGH);

    while (central.connected()) {
      if (commandCharacteristic.written()) { // Check if the characteristic is written
        byte receivedValue = commandCharacteristic.value();

        // Single servo control
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

        // Group servo control based on hex code
        switch (receivedValue) {
          case 0x10: // Actuate servos 1-5
            Serial.println("Actuating servos 1-5");
            for (int i = 0; i < 5; i++) servos[i].write(180);
            break;

          case 0x11: // Actuate servos 1, 2, and 5 (exclude 3 and 4)
            Serial.println("Actuating servos 1, 2, and 5");
            servos[0].write(180);
            servos[1].write(180);
            servos[4].write(180);
            servos[2].write(0);
            servos[3].write(0);
            break;

          case 0x12: // Actuate servos 1, 2, 4, 5 (exclude 3)
            Serial.println("Actuating servos 1, 2, 4, and 5");
            for (int i = 0; i < 5; i++) {
              if (i == 2) servos[i].write(0); // Servo 3 stays at 0
              else servos[i].write(180);
            }
            break;

          case 0x13: // Actuate servos 2 and 3 only
            Serial.println("Actuating servos 2 and 3");
            for (int i = 0; i < 5; i++) {
              if (i == 1 || i == 2) servos[i].write(180); // Servos 2 and 3
              else servos[i].write(0);
            }
            break;

          case 0x14: // Actuate servos 1-4 (exclude 5)
            Serial.println("Actuating servos 1-4");
            for (int i = 0; i < 4; i++) servos[i].write(180); // Servos 1-4
            servos[4].write(0); // Servo 5 stays at 0
            break;

          case 0x15: // Actuate servos 2, 3, and 4 (exclude 1 and 5)
            Serial.println("Actuating servos 2, 3, and 4");
            for (int i = 0; i < 5; i++) {
              if (i == 1 || i == 2 || i == 3) servos[i].write(180); // Servos 2, 3, and 4
              else servos[i].write(0);
            }
            break;

          case 0x16: // Move all servos to position 0
            Serial.println("Resetting all servos to 0 degrees.");
            for (int i = 0; i < 5; i++) {
              servos[i].write(0);
            }
            break;

          default:
            Serial.println("Invalid command received.");
            break;
        }
      }
    }

    // Turn off Bluetooth connection LED when disconnected
    digitalWrite(bluetoothLED, LOW);
    Serial.println("Disconnected from central");
  }
}
