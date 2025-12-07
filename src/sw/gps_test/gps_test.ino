/*
  Precision GPS Averaging for ESP32-C3 + NEO-M9N
  Uses UBX Binary Protocol and 64-bit Integer Math for maximum accuracy.
  
  Required Library: SparkFun u-blox GNSS v3
  Required Library: U8g2
*/

#include <Wire.h>
#include <U8g2lib.h>
#include <SparkFun_u-blox_GNSS_v3.h>

// --- Pin Configuration ---
// Ensure your wiring matches these pins. 
// Standard ESP32-C3 I2C is often 8(SDA)/9(SCL), but 10/4 is valid if wired that way.
#define SDA_PIN 10
#define SCL_PIN 4

// --- User Configuration ---
const int SAMPLE_COUNT = 100;     // Number of samples to average
const int UPDATE_RATE_HZ = 25;    // GPS Update rate (Hz). NEO-M9N supports up to 25Hz.

// OLED Display (128x64)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL_PIN, /* data=*/SDA_PIN);

// GPS Object
SFE_UBLOX_GNSS myGNSS;

// Variables for accumulation
int64_t sumLat = 0;
int64_t sumLon = 0;
int64_t sumAlt = 0;
int samplesTaken = 0;
bool averagingActive = false;

// Store final results for display
double finalLatDegrees = 0;
double finalLonDegrees = 0;
double finalAltMeters = 0;
bool resultsReady = false;

// --- FORWARD DECLARATIONS (Fixes the "Not Declared" error) ---
void updateProgressDisplay(byte siv);
void finalizeAverage();

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial monitor time to open

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000); // Increase I2C speed to 400kHz for smoother OLED/GPS traffic

  // Initialize OLED
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "GPS Averaging Test");
  u8g2.drawStr(0, 24, "Initializing...");
  u8g2.sendBuffer();

  // Initialize GNSS
  // We increase the max wait time slightly for the C3
  if (myGNSS.begin(Wire) == false) {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "GPS NOT FOUND!");
    u8g2.drawStr(0, 24, "Check wiring...");
    u8g2.sendBuffer();
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); // Turn off NMEA noise on I2C, use UBX only
  myGNSS.setNavigationFrequency(UPDATE_RATE_HZ); // Set update rate (25Hz)

  Serial.println(F("GPS Initialized. Waiting for 3D Fix..."));
  
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "GPS Initialized");
  u8g2.drawStr(0, 24, "Waiting for 3D Fix...");
  u8g2.sendBuffer();
}

void loop() {
  // Check if we have a valid 3D fix before starting
  // We only check this occasionally to keep the loop tight, 
  // but myGNSS.getPVT() handles the data polling efficiently.
  
  // Note: getSIV and getFixType can result in I2C traffic. 
  // We check fix status via the PVT packet logic mostly.
  
  if (!averagingActive) {
    byte fixType = myGNSS.getFixType();
    byte siv = myGNSS.getSIV();
    
    if (fixType == 3) {
      Serial.println(F("\n3D Fix acquired! Starting average..."));
      averagingActive = true;
      resultsReady = false;
      samplesTaken = 0;
      sumLat = 0;
      sumLon = 0;
      sumAlt = 0;
    } else {
       Serial.print(F("."));
       // Update display waiting screen
       u8g2.clearBuffer();
       u8g2.drawStr(0, 10, "Waiting for 3D Fix...");
       char buf[32];
       snprintf(buf, sizeof(buf), "Fix Type: %d", fixType);
       u8g2.drawStr(0, 24, buf);
       snprintf(buf, sizeof(buf), "Satellites: %d", siv);
       u8g2.drawStr(0, 38, buf);
       u8g2.sendBuffer();
       delay(1000); 
    }
    return;
  }

  // Poll for new data. getPVT returns true only if new data is available.
  if (myGNSS.getPVT()) {
    
    int32_t currentLat = myGNSS.getLatitude();
    int32_t currentLon = myGNSS.getLongitude();
    int32_t currentAlt = myGNSS.getAltitude(); // mm above ellipsoid

    sumLat += currentLat;
    sumLon += currentLon;
    sumAlt += currentAlt;
    samplesTaken++;

    // Only update display every 5 samples to prevent I2C bottlenecking the GPS reads
    if (samplesTaken % 5 == 0) {
      updateProgressDisplay(myGNSS.getSIV());
    }

    if (samplesTaken % 10 == 0) {
      Serial.printf("Sampling: %d/%d\n", samplesTaken, SAMPLE_COUNT);
    }

    if (samplesTaken >= SAMPLE_COUNT) {
      finalizeAverage();
      averagingActive = false; // Stop
      delay(5000); // Pause before restarting
    }
  }
}

// --- Function Definitions ---

void updateProgressDisplay(byte siv) {
  u8g2.clearBuffer();
  
  u8g2.drawStr(0, 10, "GPS Averaging...");
  
  char buf[32];
  snprintf(buf, sizeof(buf), "Samples: %d/%d", samplesTaken, SAMPLE_COUNT);
  u8g2.drawStr(0, 24, buf);
  
  snprintf(buf, sizeof(buf), "Satellites: %d", siv);
  u8g2.drawStr(0, 38, buf);
  
  // Draw progress bar
  int barWidth = (samplesTaken * 100) / SAMPLE_COUNT;
  if (barWidth > 100) barWidth = 100; // safety
  u8g2.drawFrame(0, 50, 104, 10);
  u8g2.drawBox(2, 52, barWidth, 6);
  
  snprintf(buf, sizeof(buf), "%d%%", barWidth);
  u8g2.drawStr(108, 58, buf);
  
  u8g2.sendBuffer();
}

void finalizeAverage() {
  // Calculate averages
  double avgLat = (double)sumLat / (double)samplesTaken;
  double avgLon = (double)sumLon / (double)samplesTaken;
  double avgAlt = (double)sumAlt / (double)samplesTaken;

  // Convert back to degrees (Scaling factor 10^-7)
  finalLatDegrees = avgLat / 10000000.0;
  finalLonDegrees = avgLon / 10000000.0;
  finalAltMeters = avgAlt / 1000.0;
  resultsReady = true;

  Serial.println(F("\n------ AVERAGING COMPLETE ------"));
  Serial.printf("Samples: %d\n", samplesTaken);
  
  Serial.print("Avg Latitude:  ");
  Serial.println(finalLatDegrees, 9);
  
  Serial.print("Avg Longitude: ");
  Serial.println(finalLonDegrees, 9);
  
  Serial.print("Avg Altitude:  ");
  Serial.print(finalAltMeters, 3);
  Serial.println(" m");
  Serial.println(F("--------------------------------"));

  // Update OLED with final results
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "-- AVERAGE COMPLETE --");
  
  char buf[32];
  snprintf(buf, sizeof(buf), "Samples: %d", samplesTaken);
  u8g2.drawStr(0, 24, buf);
  
  char latStr[16], lonStr[16], altStr[16];
  dtostrf(finalLatDegrees, 10, 7, latStr);
  dtostrf(finalLonDegrees, 10, 7, lonStr);
  dtostrf(finalAltMeters, 6, 2, altStr);
  
  snprintf(buf, sizeof(buf), "Lat:%s", latStr);
  u8g2.drawStr(0, 38, buf);
  
  snprintf(buf, sizeof(buf), "Lon:%s", lonStr);
  u8g2.drawStr(0, 50, buf);
  
  snprintf(buf, sizeof(buf), "Alt:%s m", altStr);
  u8g2.drawStr(0, 62, buf);
  
  u8g2.sendBuffer();
}
