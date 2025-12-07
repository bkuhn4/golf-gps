/******************************************************************************
   PROJECT:     I2C Scanner with OLED Display and GPIO Control
   AUTHOR:      Modified from various online examples
   DATE:        11/12/2025
   VERSION:     1.3

   DESCRIPTION: This sketch initializes GPIO pins 2 and 3, initializes the
                I2C bus, scans for connected devices, and prints the
                addresses of any found devices to the Serial Monitor and a
                128x64 SSD1306 OLED display using U8g2.
 ******************************************************************************/

/* --- Tools Setup ---
  Board: esp32 > ESP32C3 Dev Module
*/

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// --- PIN DEFINITIONS (for ESP32C3) ---
#define SCL_PIN 4  // I2C Clock
#define SDA_PIN 10 // I2C Data
#define GPS_RESET_PIN 21  // GPS Reset (active low)
#define GPIO3   3  // GPS enable (active low)


// --- OLED Display Setup ---
// U8g2 Constructor for 128x64 OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL_PIN, /* data=*/SDA_PIN);

// --- SETUP FUNCTION ---
// This function runs once when the board is powered on or reset.
void setup() {
  // Start serial communication at 115200 baud for debugging.
  Serial.begin(115200);
  while (!Serial); // Wait for Serial to be ready

  Serial.println("\n--- I2C Scanner with OLED & GPIO ---");

  // --- GPIO SETUP ---
  // Set GPIO pins 2 and 3 as outputs
  pinMode(GPS_RESET_PIN, OUTPUT);
  pinMode(GPIO3, OUTPUT);

  // Set GPIO 2 HIGH and GPIO 3 LOW
  digitalWrite(GPS_RESET_PIN, HIGH); // Do not reset GPS

  
  digitalWrite(GPIO3, LOW); // Power GPS
  Serial.println("GPS powered.");
  // --- END GPIO SETUP ---

  // Initialize I2C bus
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED display
  if(!u8g2.begin()) {
    Serial.println(F("U8g2 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the display buffer
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

// --- MAIN LOOP ---
// This function runs repeatedly after setup() has finished.
void loop() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning for I2C devices...");

  // Clear the display and set text properties for each scan
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf); // Small font
  u8g2.setCursor(0, 10);
  u8g2.print("Scanning for I2C...");
  u8g2.sendBuffer();
  delay(100); // Short delay to show "Scanning" message

  nDevices = 0;
  int yPos = 22; // Start printing results below the header

  // The I2C address space is 7-bits, from 0 to 127.
  // Addresses 0-7 are reserved.
  for(address = 8; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Wire.endTransmission to see if
    // a device acknowledged the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      // Print to OLED
      if (yPos < 64) { // Only print if it fits on screen
          u8g2.setCursor(0, yPos);
          u8g2.print("Found: 0x");
          if (address < 16)
            u8g2.print("0");
          u8g2.print(address, HEX);
          yPos += 10;
      }
      u8g2.sendBuffer();

      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
    // Display message on OLED if no devices are found
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("No I2C devices");
    u8g2.setCursor(0, 22);
    u8g2.print("found.");
    u8g2.sendBuffer();
  } else {
    Serial.println("Scan complete\n");
    // Optionally add a "Scan complete" message to the OLED
    if (yPos < 64) {
        u8g2.setCursor(0, yPos);
        u8g2.print("Scan complete.");
        u8g2.sendBuffer();
    }
  }

  delay(5000); // Wait 5 seconds before scanning again
}
