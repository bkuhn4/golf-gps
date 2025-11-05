/******************************************************************************
   PROJECT:     Minimal OLED Test with GPIO Control
   FILE:        oled_test_with_gpio.ino
   AUTHOR:      Simplified from Pocket Caddy Hardware Test
   DATE:        11/5/2025
   VERSION:     1.1

   DESCRIPTION: This sketch initializes a 128x32 SSD1306 OLED display,
                prints a confirmation message to both the screen and the
                Serial Monitor, and sets GPIO3 to a LOW output state.
 ******************************************************************************/

/* --- Tools Setup ---
  Board: esp32 > ESP32C3 Dev Module
*/

#include <Arduino.h>
#include <Wire.h>       // Needed for I2C communication
#include <U8g2lib.h>    // The OLED Graphics Library

// --- PIN DEFINITIONS ---

// Define the I2C pins used for the OLED display
#define SCL_PIN 4       // I2C Clock
#define SDA_PIN 10      // I2C Data

// Define the GPIO pin to be pulled low
#define GPIO3_PIN 3     // The pin to set as output low

// --- GLOBAL OBJECTS ---

// U8g2 constructor for a 128x32 SSD1306 OLED using hardware I2C.
// It specifies the screen controller, size, rotation, and I2C pins.
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL_PIN, /* data=*/SDA_PIN);


// --- SETUP FUNCTION ---
// This function runs once when the board is powered on or reset.
void setup() {
  // Start serial communication at 115200 baud for debugging.
  Serial.begin(115200);
  delay(2000); // Wait for 2 seconds to allow the Serial Monitor to connect.
  Serial.println("\n--- OLED Test with GPIO3 Control ---");


  // Configure GPIO3 as an output and pull it low.
  pinMode(GPIO3_PIN, OUTPUT);
  digitalWrite(GPIO3_PIN, LOW);
  Serial.println("GPIO3 has been set to OUTPUT LOW.");


  // Initialize the I2C bus with the defined SDA and SCL pins.
  Serial.println("Initializing I2C bus...");
  Wire.begin(SDA_PIN, SCL_PIN);


  // Initialize the OLED display.
  Serial.print("Initializing OLED display... ");
  if (!u8g2.begin()) {
    // If the display fails to initialize, print an error and stop execution.
    Serial.println("FAILED. Check I2C wiring.");
    while (1); // Halt the program.
  }
  Serial.println("OK");


  // --- Display a message on the OLED ---

  // Clear the internal memory/buffer of the display.
  u8g2.clearBuffer();

  // Set the font that will be used to draw text.
  u8g2.setFont(u8g2_font_ncenB10_tr); // A nice, readable 10-pixel high font

  // Set the cursor position (x, y) where the text will start.
  u8g2.setCursor(22, 22);

  // Print the desired text to the internal buffer.
  u8g2.print("OLED OK");

  // Send the contents of the internal buffer to the display.
  // Nothing will appear on the screen until this command is called.
  u8g2.sendBuffer();

  Serial.println("Message 'OLED OK' sent to the display.");
  Serial.println("Setup complete.");
}

// --- MAIN LOOP ---
// This function runs repeatedly after setup() has finished.
// It is empty because we only need to write to the screen once.
void loop() {
  // Nothing to do here. The setup is complete.
}
