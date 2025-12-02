#include "oled.h"

// Constructor: Initialize the u8g2 object in the initialization list
OledDisplay::OledDisplay() : u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL_PIN, /* data=*/SDA_PIN) {
}

bool OledDisplay::begin() {
    // Initialize I2C
    // Wire.begin(SDA_PIN, SCL_PIN); // Moved to main.ino
    // Wire.setClock(100000);
    // Wire.setTimeout(25);

    Serial.print("Initializing OLED Display... ");
    if (!u8g2.begin()) {
        Serial.println("FAILED. Halting.");
        return false;
    }
    Serial.println("OK");

    // Initial Screen
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(25, 28, "Initializing");
    u8g2.drawStr(15, 42, "Pocket Caddy...");
    u8g2.sendBuffer();
    return true;
}
