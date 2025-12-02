/******************************************************************************
   PROJECT:     Pocket Caddy Modular Hardware Test
   FILE:        hw_test_modular.ino
   AUTHOR:      Brady Kuhn, Bryan York, Gemini
   DATE:        11/18/2025
   VERSION:     2.2 (Preprocessor Fixes)

   DESCRIPTION: This sketch serves as a modular, comprehensive hardware test
                for the Pocket Caddy device. You can choose which components
                to initialize by commenting/uncommenting their #define
                switches at the top of the file. This is useful for
                debugging specific hardware without initializing everything.
                The main loop is event-driven based on button presses for
                the enabled components.

   V2.2 Update: Fixed "unterminated #ifdef" compilation errors by adding
                the missing #endif directives to the function definitions.

 ******************************************************************************/

/* --- Tools Setup ---
  Install ESP32 Core: Tools > Board > Board Manager > "esp32"
  Board: esp32 > ESP32C3 Dev Module
  USB CDC On Boot: "Enabled"
  JTAG Adapter: "Disabled"
*/

#include <Arduino.h>
#include <Wire.h>                                  // I2C
#include <SPI.h>                                   // SPI
#include <SD.h>                                    // SD Card
#include <U8g2lib.h>                               // OLED Graphics Library
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>  // GPS module
#include <SparkFunBQ27441.h>                       // LiPo fuel gauge
#include "Adafruit_SHT31.h"                        // Temp/humidity sensor


// =============================================================================
// --- COMPONENT SELECTION SWITCHES ---
// Comment out a line to disable the initialization of that component.
// =============================================================================
#define INITIALIZE_OLED
#define INITIALIZE_SHT31
#define INITIALIZE_BQ27441
#define INITIALIZE_GPS
#define INITIALIZE_SD_CARD
// =============================================================================


// --- PIN DEFINITIONS ---
#define SCL_PIN 4
#define SDA_PIN 10
#define OLED_WAIT_TIME 1000
#define GPS_EN_N_PIN 3
#define GPS_RESET_N_PIN 21
#define SD_CS_PIN 5
#define SPI_MOSI_PIN 6
#define SPI_MISO_PIN 7
#define SPI_SCK_PIN 8
#define BTN_1_PIN 9
#define BTN_2_PIN 20
#define BTN_3_PIN 2

// --- GLOBAL OBJECTS ---
#ifdef INITIALIZE_OLED
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL_PIN, /* data=*/SDA_PIN);
#endif

#ifdef INITIALIZE_GPS
SFE_UBLOX_GNSS myGNSS;
#endif

#ifdef INITIALIZE_SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31();
#endif

#ifdef INITIALIZE_SD_CARD
SPIClass spi_sd(FSPI);
File myFile;
#endif

// --- FUNCTION PROTOTYPES ---
void scanI2CBus();
#ifdef INITIALIZE_BQ27441
void printBatteryStats();
void displayFuelGaugeInfo();
#endif
#ifdef INITIALIZE_SHT31
void displayTempSensorData();
#endif
#ifdef INITIALIZE_GPS
void displayGpsData();
#endif
#ifdef INITIALIZE_SD_CARD
void performSdCardTest();
#endif


// --- SETUP FUNCTION ---
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n--- Pocket Caddy Hardware Test ---");

  // Initialize Pins
  pinMode(BTN_1_PIN, INPUT_PULLUP);
  pinMode(BTN_2_PIN, INPUT_PULLUP);
  pinMode(BTN_3_PIN, INPUT_PULLUP);
#if defined(INITIALIZE_GPS)
  pinMode(GPS_EN_N_PIN, OUTPUT);
  pinMode(GPS_RESET_N_PIN, OUTPUT);
  digitalWrite(GPS_EN_N_PIN, LOW);
  digitalWrite(GPS_RESET_N_PIN, HIGH);
#endif

  // Initialize I2C Bus
#if defined(INITIALIZE_OLED) || defined(INITIALIZE_SHT31) || defined(INITIALIZE_BQ27441) || defined(INITIALIZE_GPS)
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  Wire.setTimeout(25);
  Serial.println("I2C bus initialized.");
#endif

  // --- SEQUENTIAL HARDWARE TESTS ---

#ifdef INITIALIZE_OLED
  Serial.print("1. Testing OLED Display... ");
  if (!u8g2.begin()) {
    Serial.println("FAILED. Halting.");
    while (1);
  }
  Serial.println("OK");
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 15, "OLED OK");
  u8g2.sendBuffer();
  delay(OLED_WAIT_TIME);
#endif

#if defined(INITIALIZE_OLED) || defined(INITIALIZE_SHT31) || defined(INITIALIZE_BQ27441) || defined(INITIALIZE_GPS)
  scanI2CBus();
  delay(1000);
#endif

#ifdef INITIALIZE_SHT31
#ifdef INITIALIZE_OLED
  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "Testing SHT31...");
  u8g2.sendBuffer();
#endif
  Serial.print("3. Testing SHT31 Sensor... ");
  if (!sht31.begin(0x44)) {
    Serial.println("FAILED");
  } else {
    Serial.println("OK");
  }
  delay(OLED_WAIT_TIME);
#endif

#ifdef INITIALIZE_BQ27441
#ifdef INITIALIZE_OLED
  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "Testing BQ27441...");
  u8g2.sendBuffer();
#endif
  Serial.print("4. Testing BQ27441 Fuel Gauge... ");
  if (!lipo.begin()) {
    Serial.println("FAILED");
  } else {
    Serial.println("OK");
    lipo.setCapacity(1500);
  }
  delay(OLED_WAIT_TIME);
#endif

#ifdef INITIALIZE_GPS
#ifdef INITIALIZE_OLED
  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "Testing GPS...");
  u8g2.sendBuffer();
#endif
  Serial.print("5. Testing u-blox GPS... ");
  if (!myGNSS.begin(Wire)) {
    Serial.println("FAILED");
  } else {
    Serial.println("OK");
    myGNSS.setI2COutput(COM_TYPE_UBX);
    myGNSS.saveConfiguration();
  }
  delay(OLED_WAIT_TIME);
#endif

#ifdef INITIALIZE_SD_CARD
#ifdef INITIALIZE_OLED
  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "Testing SD Card...");
  u8g2.sendBuffer();
#endif
  Serial.print("6. Testing SD Card... ");
  spi_sd.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SD_CS_PIN);
  if (!SD.begin(SD_CS_PIN, spi_sd)) {
    Serial.println("Mount FAILED");
    #ifdef INITIALIZE_OLED
      u8g2.clearBuffer();
      u8g2.drawStr(0, 15, "SD Mount FAILED");
      u8g2.sendBuffer();
    #endif
  } else {
    Serial.println("Mount OK.");
    #ifdef INITIALIZE_OLED
      u8g2.clearBuffer();
      u8g2.drawStr(0, 15, "SD Mount OK");
      u8g2.sendBuffer();
    #endif
  }
  delay(OLED_WAIT_TIME);
#endif

  Serial.println("\n--- Initial tests complete. Entering live loop. ---");
#ifdef INITIALIZE_OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
#ifdef INITIALIZE_SHT31
  u8g2.drawStr(0, 10, "BTN1: Temp/Humidity");
#endif
#if defined(INITIALIZE_SD_CARD) || defined(INITIALIZE_GPS)
  u8g2.drawStr(0, 20, "BTN2: SD Test/GPS");
#endif
#ifdef INITIALIZE_BQ27441
  u8g2.drawStr(0, 30, "BTN3: Battery Info");
#endif
  u8g2.sendBuffer();
#endif
}

// --- MAIN LOOP ---
void loop() {
#ifdef INITIALIZE_SHT31
  if (digitalRead(BTN_1_PIN) == LOW) {
    delay(50);
    displayTempSensorData();
    while (digitalRead(BTN_1_PIN) == LOW);
  }
#endif

  if (digitalRead(BTN_2_PIN) == LOW) {
    delay(50);
    #if defined(INITIALIZE_SD_CARD)
      performSdCardTest();
    #elif defined(INITIALIZE_GPS)
      displayGpsData();
    #endif
    while (digitalRead(BTN_2_PIN) == LOW);
  }

#ifdef INITIALIZE_BQ27441
  if (digitalRead(BTN_3_PIN) == LOW) {
    delay(50);
    displayFuelGaugeInfo();
    while (digitalRead(BTN_3_PIN) == LOW);
  }
#endif

  delay(10);
}

// --- FUNCTION DEFINITIONS ---

#ifdef INITIALIZE_SD_CARD
void performSdCardTest() {
  Serial.println("\n--- Performing SD Card Read/Write Test ---");
  #ifdef INITIALIZE_OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 15, "Writing to SD...");
    u8g2.sendBuffer();
  #endif

  myFile = SD.open("/test.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("ESP32-C3 SD test.");
    myFile.println("Write successful!");
    myFile.close();
    Serial.println(" done.");
  } else {
    Serial.println("Error opening test.txt for writing.");
    #ifdef INITIALIZE_OLED
      u8g2.clearBuffer();
      u8g2.drawStr(0, 15, "Write FAILED");
      u8g2.sendBuffer();
    #endif
    return;
  }
  delay(100);

  #ifdef INITIALIZE_OLED
    u8g2.clearBuffer();
    u8g2.drawStr(0, 15, "Reading from SD...");
    u8g2.sendBuffer();
  #endif

  myFile = SD.open("/test.txt");
  if (myFile) {
    Serial.println("Contents of test.txt:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
    Serial.println("Read test complete.");
    #ifdef INITIALIZE_OLED
      u8g2.clearBuffer();
      u8g2.drawStr(0, 15, "SD Test OK");
      u8g2.sendBuffer();
    #endif
  } else {
    Serial.println("Error opening test.txt for reading.");
    #ifdef INITIALIZE_OLED
      u8g2.clearBuffer();
      u8g2.drawStr(0, 15, "Read FAILED");
      u8g2.sendBuffer();
    #endif
  }
}
#endif


#ifdef INITIALIZE_SHT31
void displayTempSensorData() {
  Serial.println("\n--- Reading Temp/Humidity Sensor ---");
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (!isnan(t)) {
    Serial.print("Temp *C = "); Serial.println(t);
  } else {
    Serial.println("Failed to read temperature");
  }
  if (!isnan(h)) {
    Serial.print("Hum. % = "); Serial.println(h);
  } else {
    Serial.println("Failed to read humidity");
  }
#ifdef INITIALIZE_OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 10);
  u8g2.print("Temp: "); u8g2.print(t, 1); u8g2.print(" C");
  u8g2.setCursor(0, 28);
  u8g2.print("Humidity: "); u8g2.print(h, 1); u8g2.print(" %");
  u8g2.sendBuffer();
#endif
}
#endif // <-- FIX: Added missing #endif for SHT31 function block


#ifdef INITIALIZE_GPS
void displayGpsData() {
  Serial.println("\n--- Reading GPS Module ---");
#ifdef INITIALIZE_OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 15);
  u8g2.print("Searching for GPS...");
  u8g2.sendBuffer();
#endif

  if (myGNSS.getPVT()) {
    long latitude = myGNSS.getLatitude();
    long longitude = myGNSS.getLongitude();
    byte siv = myGNSS.getSIV();
    Serial.print("Lat: "); Serial.print(latitude / 10000000.0, 6);
    Serial.print(" Lon: "); Serial.print(longitude / 10000000.0, 6);
    Serial.print(" Sats: "); Serial.println(siv);
#ifdef INITIALIZE_OLED
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10); u8g2.print("Lat: "); u8g2.print(latitude / 10000000.0, 4);
    u8g2.setCursor(0, 22); u8g2.print("Lon: "); u8g2.print(longitude / 10000000.0, 4);
    u8g2.setCursor(0, 32); u8g2.print("Sats: "); u8g2.print(siv);
    u8g2.sendBuffer();
#endif
  } else {
    Serial.println("No GPS lock.");
#ifdef INITIALIZE_OLED
    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    u8g2.print("No GPS Lock");
    u8g2.sendBuffer();
#endif
  }
}
#endif // <-- FIX: Added missing #endif for GPS function block


#ifdef INITIALIZE_BQ27441
void displayFuelGaugeInfo() {
  printBatteryStats();
#ifdef INITIALIZE_OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 10);
  u8g2.print("Voltage: "); u8g2.print(lipo.voltage()); u8g2.print(" mV");
  u8g2.setCursor(0, 28);
  u8g2.print("Charge: "); u8g2.print(lipo.soc()); u8g2.print(" %");
  u8g2.sendBuffer();
#endif
}

void printBatteryStats() {
  Serial.println("\n--- Battery Stats ---");
  Serial.print("Voltage: "); Serial.print(lipo.voltage()); Serial.println(" mV");
  Serial.print("State of Charge: "); Serial.print(lipo.soc()); Serial.println(" %");
  Serial.print("Average Current: "); Serial.print(lipo.current(AVG)); Serial.println(" mA");
}
#endif // <-- FIX: Added missing #endif for BQ27441 function block


void scanI2CBus() {
  Serial.println("2. Scanning I2C bus...");
  byte count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print(" - Device found at 0x");
      Serial.println(address, HEX);
      count++;
    }
  }
  if (count == 0) {
    Serial.println("No I2C devices found.");
  }
}
