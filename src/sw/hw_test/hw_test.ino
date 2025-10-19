/******************************************************************************
   PROJECT:     Pocket Caddy Hardware Test
   FILE:        hw_test.ino
   AUTHOR:      Brady Kuhn, Bryan York
   DATE:        10/17/2025
   VERSION:     0

   TODO:

 ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>                                  // I2C
#include <SPI.h>                                   // SPI
#include <SD.h>                                    // SD Card
#include <Adafruit_GFX.h>                          // OLED Graphics
#include <Adafruit_SSD1306.h>                      // OLED Driver
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>  // GPS module
#include <SparkFunBQ27441.h>                       // LiPo fuel gauge
#include "Adafruit_SHT31.h"                        // Temp/humidity sensor

// --- PIN DEFINITIONS ---

// I2C Bus Pins (for OLED, Fuel Gauge, Temp/Hum Sensor, and GPS Module)
#define SCL_PIN         4   // I2C Clock
#define SDA_PIN         10  // I2C Data

// GPS Module Control Pins (Power and Reset)
#define GPS_EN_N_PIN    3
#define GPS_RESET_N_PIN 2

// SPI Bus Pin (for MicroSD Card)
#define SD_CS_PIN       5   // SD Card Chip Select
#define SPI_MOSI_PIN    6   // SPI MOSI
#define SPI_MISO_PIN    7   // SPI MISO
#define SPI_SCK_PIN     8   // SPI SCK

// User Input Buttons
#define BTN_1_PIN       9   // Also serves as BOOT button
#define BTN_2_PIN       20
#define BTN_3_PIN       21

// --- GLOBAL OBJECTS ---
Adafruit_SSD1306  display(128, 32, &Wire, -1);
SFE_UBLOX_GNSS    myGNSS;
SFE_BQ27441       lipo;
Adafruit_SHT31    sht30 = Adafruit_SHT31();

// --- FUNCTION PROTOTYPES ---
void scanI2CBus();

// --- SETUP FUNCTION ---
void setup() {
  // Start serial for debugging output
  Serial.begin(115200);
  delay(2000); // Wait for Serial Monitor to connect
  Serial.println("\n--- Pocket Caddy Hardware Test ---");

  // Initialize Button Pins
  pinMode(BTN_1_PIN, INPUT_PULLUP);
  pinMode(BTN_2_PIN, INPUT_PULLUP);
  pinMode(BTN_3_PIN, INPUT_PULLUP);
  Serial.println("Button pins initialized.");

  // Initialize GPS Control Pins
  pinMode(GPS_EN_N_PIN, OUTPUT);
  pinMode(GPS_RESET_N_PIN, OUTPUT);
  digitalWrite(GPS_EN_N_PIN, LOW);   // Default state: Set low to turn on PMOS and power GPS
  digitalWrite(GPS_RESET_N_PIN, HIGH); // Default state: GPS not in reset
  Serial.println("GPS control pins initialized.");

  // Initialize SD Card Chip Select Pin
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH); // Ensure SD card is not selected by default
  Serial.println("SD Card CS pin initialized.");

  // Initialize I2C Bus
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);  // 100kHz recommended by u-blox library
  Serial.println("I2C bus initialized at 100kHz.");

  // --- SEQUENTIAL HARDWARE TESTS ---

  // 1. OLED Display Test
  Serial.print("1. Testing OLED Display... ");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("FAILED. Halting.");
    while (1); // Stop here if the display fails
  }
  Serial.println("OK");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("HW TEST: OLED OK");
  display.display();
  delay(1000);

  // 2. I2C Bus Scan
  scanI2CBus();
  delay(1000);

  // 3. SHT30 Temperature/Humidity Sensor Test
  Serial.print("3. Testing SHT30 Sensor... ");
  display.print("SHT30: ");
  if (!sht30.begin(0x44)) { // 0x44 is the default address
    Serial.println("FAILED");
    display.println("FAIL");
  } else {
    Serial.println("OK");
    display.println("OK");
  }
  display.display();
  delay(1000);

  // 4. BQ27441 LiPo Fuel Gauge Test
  Serial.print("4. Testing BQ27441 Fuel Gauge... ");
  display.print("BQ27441: ");
  if (!lipo.begin()) {
    Serial.println("FAILED");
    display.println("FAIL");
  } else {
    Serial.println("OK");
    display.println("OK");
  }
  display.display();
  delay(1000);

  // 5. u-blox GPS Module Test
  Serial.print("5. Testing u-blox GPS... ");
  display.print("GPS: ");
  if (!myGNSS.begin(Wire)) {
    Serial.println("FAILED");
    display.println("FAIL");
  } else {
    Serial.println("OK");
    display.println("OK");
  }
  display.display();
  delay(1000);

  // 6. SD Card Test
  Serial.print("6. Testing SD Card... ");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Testing SD Card...");
  display.display();
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Mount FAILED");
    display.println("SD Mount: FAIL");
  } else {
    Serial.println("Mount OK.");
    display.println("SD Mount: OK");
    display.display();
    File testFile = SD.open("/hw_test.txt", FILE_WRITE);
    if (testFile) {
      testFile.println("SD write OK");
      testFile.close();
      Serial.println("Write OK.");
      display.println("SD Write: OK");
    } else {
      Serial.println("Write FAILED");
      display.println("SD Write: FAIL");
    }
  }
  display.display();
  delay(2000);

  Serial.println("\n--- Initial tests complete. Entering live loop. ---");
  display.clearDisplay();
  display.println("Tests Complete!");
  display.println("Entering Live Mode");
  display.display();
  delay(2000);
}

// --- MAIN LOOP ---
void loop() {

}
