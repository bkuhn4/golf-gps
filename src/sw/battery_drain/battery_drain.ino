#include <Wire.h>
#include <WiFi.h>
#include <Preferences.h>
#include <SparkFunBQ27441.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- HARDWARE CONFIG ---
#define SDA_PIN 10
#define SCL_PIN 4 

// --- BUTTONS (Adjust pins to match your PCB) ---
// Connect buttons between Pin and GND (Active LOW)
#define BTN_LOAD_PIN  9   // Button 1: Toggle High Load
#define BTN_SLEEP_PIN 20  // Button 2: Force Sleep/Relax
#define BTN_SAVE_PIN  2   // Button 3: Save Data

// --- BATTERY SETTINGS ---
#define DESIGN_CAPACITY 1500 
#define TERMINATE_VOLTAGE 3000 // 3.0V

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Preferences preferences;

bool loadEnabled = false;
unsigned long lastButtonPress = 0;

// Helper to send GAUGE_EN (0x0021) since executeControlWord is private
bool enableGauge() {
  Wire.beginTransmission(0x55); // BQ27441 I2C Address
  Wire.write(0x00);             // Control() Command
  Wire.write(0x21);             // LSB of GAUGE_EN
  Wire.write(0x00);             // MSB of GAUGE_EN
  return Wire.endTransmission() == 0;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Setup Buttons
  pinMode(BTN_LOAD_PIN, INPUT_PULLUP);
  pinMode(BTN_SLEEP_PIN, INPUT_PULLUP);
  pinMode(BTN_SAVE_PIN, INPUT_PULLUP);

  // Setup Wakeup Source (Any button press wakes the ESP32-C3)
  // Mask is a bitmask of pins. If using pins 3,4,5:
  // Note: ESP32-C3 Deep Sleep wakeup usually works best on RTC GPIOs.
  // We will assume standard GPIO wakeup for C3:
  esp_deep_sleep_enable_gpio_wakeup(BIT(BTN_LOAD_PIN) | BIT(BTN_SLEEP_PIN) | BIT(BTN_SAVE_PIN), ESP_GPIO_WAKEUP_GPIO_LOW);

  // Init OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED Fail")); for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Init BQ27411
  if (!lipo.begin()) {
    display.setCursor(0,0);
    display.println("Gauge Not Found!");
    display.display();
    while (1);
  }

  // --- RESTORE DATA ---
  preferences.begin("bat_data", true);
  unsigned int savedCap = preferences.getUInt("learned_cap", 0);
  preferences.end();
  
  // If we have valid saved data, load it. Otherwise default.
  if (savedCap > 1000 && savedCap < 2000) {
    lipo.setCapacity(savedCap); 
  } else {
    lipo.setCapacity(DESIGN_CAPACITY);
  }
  
  // Force Learning Mode ON
  if (enableGauge()) {
    Serial.println("Gauge Enabled (Impedance Track)");
  } else {
    Serial.println("Failed to Enable Gauge");
  }

  // Print Wake Reason
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_GPIO) {
      Serial.println("Woke up via Button!");
  }
}

void loop() {
  // 1. Read Gauge
  unsigned int volts = lipo.voltage();
  int current = lipo.current(AVG);
  unsigned int soc = lipo.soc();
  unsigned int fcc = lipo.capacity(FULL);

  // 2. Handle Buttons
  if (millis() - lastButtonPress > 300) { // Simple debounce
    
    // BUTTON 1: TOGGLE LOAD
    if (digitalRead(BTN_LOAD_PIN) == LOW) {
      loadEnabled = !loadEnabled;
      if (loadEnabled) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Drainer", "12345678");
      } else {
        WiFi.mode(WIFI_OFF);
      }
      lastButtonPress = millis();
    }

    // BUTTON 2: FORCE SLEEP
    if (digitalRead(BTN_SLEEP_PIN) == LOW) {
      goToSleep();
    }

    // BUTTON 3: SAVE
    if (digitalRead(BTN_SAVE_PIN) == LOW) {
      saveData();
      lastButtonPress = millis();
    }
  }

  // 3. Update Display
  display.clearDisplay();
  display.setCursor(0,0);
  
  if (loadEnabled) display.println(">> DRAINING <<");
  else if (current > 50) display.println(">> CHARGING <<");
  else display.println("-- IDLE/READY --");

  display.setCursor(0, 15);
  display.setTextSize(2);
  display.print(volts/1000.0, 2); display.print("V");
  
  display.setTextSize(1);
  display.setCursor(80, 15);
  display.print(soc); display.println("%");

  display.setCursor(0, 35);
  display.print("I: "); display.print(current); display.println(" mA");
  display.print("FCC: "); display.print(fcc); display.println(" mAh");

  display.setCursor(0, 55);
  display.print("[1]Load [2]Slp [3]Sav");
  
  display.display();

  // 4. Auto-Cutoff Logic
  if (loadEnabled) {
     // Heavy math to generate heat/load
     double x = 10.0;
     for(int i=0; i<1000; i++) x = sqrt(x+i);
     
     // Automatic safety stop
     if (volts <= TERMINATE_VOLTAGE) {
        loadEnabled = false;
        WiFi.mode(WIFI_OFF);
        // We go to sleep automatically to allow relax
        goToSleep();
     }
  }
  
  delay(200);
}

void goToSleep() {
  display.clearDisplay();
  display.setCursor(0,20);
  display.setTextSize(2);
  display.println("SLEEPING...");
  display.setTextSize(1);
  display.println("Press Btn to Wake");
  display.display();
  delay(2000);
  
  // The setup() defined the wakeup pins.
  // Start Deep Sleep. Everything (OLED, WiFi, CPU) turns off.
  // BQ27411 stays ALIVE.
  esp_deep_sleep_start();
}

void saveData() {
  unsigned int currentCap = lipo.capacity(FULL);
  preferences.begin("bat_data", false);
  preferences.putUInt("learned_cap", currentCap);
  preferences.end();
  
  display.clearDisplay();
  display.setCursor(0,20);
  display.println("SAVED TO FLASH!");
  display.print("Val: "); display.println(currentCap);
  display.display();
  delay(2000);
}