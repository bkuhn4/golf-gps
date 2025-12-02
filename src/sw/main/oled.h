#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#define SCL_PIN 4
#define SDA_PIN 10

class OledDisplay {
public:
    OledDisplay();
    bool begin();
    
    // Expose the u8g2 object directly so other classes can draw on it
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
};

#endif // OLED_H
