/******************************************************************************
   PROJECT:     SD Card Read/Write Test for ESP32-C3
   FILE:        sd_test_esp32.ino
   AUTHOR:      Gemini (Revised)
   DATE:        11/5/2025
   VERSION:     1.1
   DESCRIPTION: A simple test to write to and read from an SD card on an ESP32,
                explicitly defining the SPI pins.
 ******************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// --- PIN DEFINITIONS ---
#define SD_CS_PIN    5      // SD Card Chip Select
#define SPI_MOSI_PIN 6      // SPI MOSI (Master Out, Slave In) -> SD Card DI/CMD
#define SPI_MISO_PIN 7      // SPI MISO (Master In, Slave Out) -> SD Card DO/DAT0
#define SPI_SCK_PIN  8      // SPI SCK -> SD Card CLK

// Create a new SPI object for the SD card.
// The ESP32-C3 uses the FSPI bus.
SPIClass spi_sd(FSPI);

File myFile;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- SD Card Read/Write Test (ESP32-C3) ---");

  // Initialize the SPI bus with your custom pins
  spi_sd.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SD_CS_PIN);

  // Now, initialize the SD card with the custom SPI object.
  // The first argument is the CS pin, the second is the SPI object.
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN, spi_sd)) {
    Serial.println("Initialization failed!");
    Serial.println("Things to check:");
    Serial.println("* Is the card properly inserted and formatted (FAT32/FAT16)?");
    Serial.println("* Are the pull-up resistors (10k) installed on all SPI lines?");
    Serial.println("* Is the wiring correct (MISO->DO, MOSI->DI)?");
    Serial.println("* Is the power supply clean (decoupling capacitor present)?");
    while (1); // Halt execution
  }
  Serial.println("Initialization done.");

  // --- WRITE TEST ---
  myFile = SD.open("/test.txt", FILE_WRITE);

  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("This is a test message.");
    myFile.println("If you can read this, the write test was successful.");
    myFile.close();
    Serial.println(" done.");
  } else {
    Serial.println("Error opening test.txt for writing.");
    return;
  }

  // --- READ TEST ---
  myFile = SD.open("/test.txt");
  if (myFile) {
    Serial.println("\nContents of test.txt:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
    Serial.println("\nRead test complete.");
  } else {
    Serial.println("Error opening test.txt for reading.");
    return;
  }

  Serial.println("\n--- Test Finished ---");
}

void loop() {
  // Nothing to do here
}
