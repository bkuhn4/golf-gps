/******************************************************************************
   PROJECT:     I2C Scanner with OLED Display and GPIO Control
   AUTHOR:      Modified from various online examples
   DATE:        11/12/2025
   VERSION:     1.2

   DESCRIPTION: This sketch initializes GPIO pins 2 and 3, initializes the
                I2C bus, scans for connected devices, and prints the
                addresses of any found devices to the Serial Monitor and a
                128x32 SSD1306 OLED display.
 ******************************************************************************/

/* --- Tools Setup ---
  Board: esp32 > ESP32C3 Dev Module
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- PIN DEFINITIONS (for ESP32C3) ---
#define SCL_PIN 4  // I2C Clock
#define SDA_PIN 10 // I2C Data
#define GPIO2   2  // GPS Reset (active low)
#define GPIO3   3  // GPS enable (active low)


// --- OLED Display Setup ---
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- SETUP FUNCTION ---
// This function runs once when the board is powered on or reset.
void setup() {
  // Start serial communication at 115200 baud for debugging.
  Serial.begin(115200);
  while (!Serial); // Wait for Serial to be ready

  Serial.println("\n--- I2C Scanner with OLED & GPIO ---");

  // --- GPIO SETUP ---
  // Set GPIO pins 2 and 3 as outputs
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);

  // Set GPIO 2 HIGH and GPIO 3 LOW
  digitalWrite(GPIO2, HIGH); // Do not reset GPS

  
  digitalWrite(GPIO3, LOW); // Power GPS
  Serial.println("GPS powered.");
  // --- END GPIO SETUP ---

  // Initialize I2C bus
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the display buffer
  display.clearDisplay();
  display.display();
}

// --- MAIN LOOP ---
// This function runs repeatedly after setup() has finished.
void loop() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning for I2C devices...");

  // Clear the display and set text properties for each scan
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Scanning for I2C...");
  display.display();
  delay(100); // Short delay to show "Scanning" message

  nDevices = 0;
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
      display.print("Found: 0x");
      if (address < 16)
        display.print("0");
      display.println(address, HEX);
      display.display();

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
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("No I2C devices");
    display.println("found.");
    display.display();
  } else {
    Serial.println("Scan complete\n");
    // Optionally add a "Scan complete" message to the OLED
    display.println("Scan complete.");
    display.display();
  }

  delay(5000); // Wait 5 seconds before scanning again
}
