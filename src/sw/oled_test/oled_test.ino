/******************************************************************************
   PROJECT:     Simple I2C Scanner
   AUTHOR:      Simplified from various online examples
   DATE:        11/5/2025
   VERSION:     1.0

   DESCRIPTION: This sketch initializes the I2C bus, scans for connected
                devices, and prints the addresses of any found devices to
                the Serial Monitor.
 ******************************************************************************/

/* --- Tools Setup ---
  Board: esp32 > ESP32C3 Dev Module (or any Arduino compatible board)
*/

#include <Wire.h> // Needed for I2C communication

// --- PIN DEFINITIONS (for ESP32C3) ---
// If you are using a different board, you might not need to define these
// as the Wire library often uses the board's default I2C pins.
#define SCL_PIN 4  // I2C Clock
#define SDA_PIN 10 // I2C Data

// --- SETUP FUNCTION ---
// This function runs once when the board is powered on or reset.
void setup() {
  // Start serial communication at 115200 baud for debugging.
  Serial.begin(115200);
  while (!Serial); // Wait for Serial to be ready

  Serial.println("\n--- I2C Scanner ---");

  // Initialize the I2C bus. For some boards like ESP32,
  // you can specify the SDA and SCL pins.
  // For standard Arduino boards (like Uno), you can just use Wire.begin();
  Wire.begin(SDA_PIN, SCL_PIN);
}

// --- MAIN LOOP ---
// This function runs repeatedly after setup() has finished.
void loop() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning for I2C devices...");

  nDevices = 0;
  // The I2C address space is 7-bits, from 0 to 127.
  // Addresses 0-7 are reserved.
  for(address = 8; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device acknowledged to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("Scan complete\n");

  delay(5000); // Wait 5 seconds before scanning again
}