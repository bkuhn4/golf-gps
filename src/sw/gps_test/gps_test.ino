#include <Wire.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// I2C Pins for ESP32-C3
#define I2C_SDA 10
#define I2C_SCL 4

// OLED Screen Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// GPS Object
SFE_UBLOX_GNSS myGNSS;

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect
  Serial.println("NEO-M9N Test Script");

  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED Initialized");
  display.display();

  // Initialize GPS
  if (myGNSS.begin() == false) {
    Serial.println(F("u-blox GNSS module not detected. Please check wiring."));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("GPS not found!");
    display.display();
    while (1);
  }
  Serial.println("u-blox GNSS module detected.");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("GPS Detected");
  display.println("Waiting for Lock...");
  display.display();

  // Set I2C as the communication port
  myGNSS.setI2COutput(COM_TYPE_UBX);
  // Save the configuration
  myGNSS.saveConfiguration();
}

void loop() {
  // Query the GPS module for new data
  if (myGNSS.getPVT()) {
    long latitude = myGNSS.getLatitude();
    long longitude = myGNSS.getLongitude();
    byte siv = myGNSS.getSIV();

    // Print to Serial Monitor
    Serial.print("Lat: ");
    Serial.print(latitude / 10000000.0, 6);
    Serial.print(" Lon: ");
    Serial.print(longitude / 10000000.0, 6);
    Serial.print(" Sats: ");
    Serial.println(siv);

    // Update OLED Display
    display.clearDisplay();
    display.setCursor(0, 0);

    display.print("Lat: ");
    display.println(latitude / 10000000.0, 6);

    display.print("Lon: ");
    display.println(longitude / 10000000.0, 6);

    display.print("Sats: ");
    display.println(siv);

    display.display();
  }

  delay(1000); // Update every second
}
