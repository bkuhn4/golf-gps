/******************************************************************************
 * PROJECT:     Pocket Caddy - Golf GPS
 * FILE:        PocketCaddy_GolfGPS.ino
 * AUTHOR:      Brady Kuhn, Bryan York
 * DATE:        10/15/2025
 * VERSION:     0
 *
 * REQUIRED LIBRARIES:
 * - Wire.h                                (Built-in)
 * - SPI.h                                 (Built-in)
 * - SD.h                                  (Built-in with ESP32 core)
 * - Adafruit GFX Library                  (by Adafruit)
 * - Adafruit SSD1306                      (by Adafruit)
 * - SparkFun u-blox GNSS Arduino Library  (by SparkFun Electronics)
 * - SparkFun BQ27441 LiPo Fuel Gauge      (by SparkFun Electronics)
 * - Adafruit SHT31 Library                (by Adafruit)
 *
 * TODO: 
 *
 ******************************************************************************/

// --- PIN DEFINITIONS ---

// I2C Bus Pins (for OLED, Fuel Gauge, Temp/Hum Sensor, and GPS Module)
#define SCL_PIN         4   // I2C Clock
#define SDA_PIN         10  // I2C Data

// GPS Module Control Pins (Power and Reset)
#define GPS_EN_N_PIN    3
#define GPS_RESET_N_PIN 2

// SPI Bus Pin (for MicroSD Card)
#define SD_CS_PIN       5   // SPI Chip Select

// User Input Buttons
#define BTN_1_PIN       9   // BOOT button
#define BTN_2_PIN       20
#define BTN_3_PIN       21

// --- SETUP FUNCTION ---
void setup() {
  // Start serial for debugging output
  Serial.begin(115200);
  Serial.println("\n--- Pocket Caddy: Initializing GPIO Pins (I2C GPS) ---");

  // Initialize Button Pins
  pinMode(BTN_1_PIN, INPUT_PULLUP);
  pinMode(BTN_2_PIN, INPUT_PULLUP);
  pinMode(BTN_3_PIN, INPUT_PULLUP);
  Serial.println("Button pins initialized.");

  // Initialize GPS Control Pins
  pinMode(GPS_EN_N_PIN, OUTPUT);
  pinMode(GPS_RESET_N_PIN, OUTPUT);
  digitalWrite(GPS_EN_N_PIN, HIGH);   // Default state: GPS powered ON
  digitalWrite(GPS_RESET_N_PIN, HIGH); // Default state: GPS not in reset
  Serial.println("GPS control pins initialized.");

  // Initialize SD Card Chip Select Pin
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH); // Default state: SD card not selected
  Serial.println("SD Card CS pin initialized.");

  // Note: The I2C pins (SDA, SCL) and SPI pins (MOSI, MISO, SCK)
  // are configured by their respective library `begin()` calls (e.g., Wire.begin()).
  // No explicit pinMode() is needed for them here.

  Serial.println("\n--- Pin initialization complete. ---");
}

// --- MAIN LOOP ---
void loop() {
  // Intentionally empty.
}
