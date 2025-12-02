/******************************************************************************
   PROJECT:     Pocket Caddy
   FILE:        main.ino
   AUTHOR:      Brady Kuhn, Bryan York
   DATE:        11/22/2025
   VERSION:     0.1

   DESCRIPTION: Main application file for the Pocket Caddy device.
                Initializes all components and enters the main event loop.
 ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>

// Headers
#include "button_handler.h"
#include "oled.h"
#include "gps.h"
#include "temp_humid.h"
#include "sd_card.h"
#include "battery_fuel_gauge.h"
#include "menu_handler.h"
#include "track_shot.h"
#include "app_controller.h"

// --- GLOBAL OBJECTS ---
ButtonHandler btnHandler;
OledDisplay display;
GpsHandler gps;
TempHumidSensor tempSensor;
SdCardHandler sdCard;
BatteryFuelGauge battery;
MenuHandler menu;
TrackShot trackShot;
AppController app;

// --- SETUP FUNCTION ---
void setup() {
  Serial.begin(115200);
  // while (!Serial); // Removed blocking wait for Serial
  Serial.println("\n--- Pocket Caddy ---");

  // Initialize Components
  if (!btnHandler.begin()) Serial.println("Button Handler Init Failed");
  
  // Initialize I2C Bus (Centralized)
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  if (!display.begin()) Serial.println("Display Init Failed");
  
  // Initialize I2C Bus
  if (!tempSensor.begin()) Serial.println("Temp Sensor Init Failed");
  if (!battery.begin()) Serial.println("Battery Init Failed");
  if (!gps.begin()) Serial.println("GPS Init Failed");
  if (!sdCard.begin()) Serial.println("SD Card Init Failed");

  // Initialize Modules
  trackShot.begin(&gps);
  app.begin(&sdCard, &gps, &tempSensor, &battery, &trackShot);
  menu.begin(&display.u8g2, &app);

  Serial.println("\n--- Initialization complete. Entering live loop. ---");
}

// --- MAIN LOOP ---

void loop() {
  // Check Buttons (Non-blocking check)
  ButtonEvent event = btnHandler.checkButtons();
  if (event != EVENT_NONE) {
      Serial.print("Button Event: ");
      Serial.println(event);
  }

  // Handle Menu Input
  menu.handleInput(event);
  
  // Update App Logic
  app.update(&menu);

  // Update Menu (Drawing & Timers)
  menu.update();

  delay(10); // Small delay to prevent CPU hogging
}
