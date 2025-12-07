#include <Wire.h>
#include <WiFi.h>
#include <SparkFunBQ27441.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- CONFIGURATION ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define BATTERY_CAPACITY 1500 // mAh
#define SDA_PIN 10
#define SCL_PIN 4

// Safety Cutoff Voltage (LiPo batteries should not go below 3.0V)
#define CUTOFF_VOLTAGE 3000 // mV

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  
  // 1. Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // 2. Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Initializing...");
  display.display();

  // 3. Initialize BQ27411 (Using BQ27441 Library)
  // Note: If this fails, your BQ chip might have a different ID than the library expects.
  // In that case, we would need raw I2C commands, but this usually works for reading.
  if (!lipo.begin()) {
    Serial.println("Error: BQ274xx not detected. Check wiring!");
    display.println("Bat Sensor Fail!");
    display.display();
    while (1);
  }
  
  // Set capacity (Optional for draining, but good for SOC accuracy)
  lipo.setCapacity(BATTERY_CAPACITY);

  // 4. Start Power Draining Features
  Serial.println("Starting HIGH LOAD mode...");
  
  // Enable WiFi in AP mode to consume Radio power
  WiFi.mode(WIFI_AP);
  WiFi.softAP("BatteryDrainer", "12345678"); 
}

void loop() {
  // --- A. HEAVY COMPUTATION (Burn CPU Cycles) ---
  // We do some floating point math to keep the CPU warm
  float val = 100.0;
  for(int i=0; i<1000; i++) {
    val = sqrt(val * i) + cos(i); 
  }

  // --- B. READ BATTERY DATA ---
  unsigned int voltage = lipo.voltage(); // mV
  int current = lipo.current(AVG);       // mA (Negative when discharging)
  unsigned int soc = lipo.soc();         // %

  // --- C. PRINT TO SERIAL ---
  Serial.print("Volts: "); Serial.print(voltage); Serial.print(" mV | ");
  Serial.print("Curr: "); Serial.print(current); Serial.print(" mA | ");
  Serial.print("SOC: "); Serial.print(soc); Serial.println(" %");

  // --- D. UPDATE OLED ---
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("--- DISCHARGING ---");
  
  display.setCursor(0, 15);
  display.setTextSize(2);
  display.print(voltage / 1000.0, 2); 
  display.print(" V");

  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Load: "); display.print(abs(current)); display.println(" mA");
  
  display.setCursor(0, 50);
  display.print("Est: "); display.print(soc); display.println("% Rem");

  display.display();

  // --- E. SAFETY CUTOFF ---
  if (voltage <= CUTOFF_VOLTAGE && voltage > 0) {
    stopDraining();
  }

  // Small delay to let the OLED refresh readable, but not too long to keep load high
  delay(100);
}

void stopDraining() {
  Serial.println("CUTOFF VOLTAGE REACHED! SLEEPING...");
  
  display.clearDisplay();
  display.setCursor(0, 20);
  display.setTextSize(2);
  display.println("EMPTY!");
  display.setTextSize(1);
  display.println("Sleeping...");
  display.display();

  // Turn off WiFi
  WiFi.mode(WIFI_OFF);
  
  // Go to Deep Sleep (Ultra low power)
  esp_deep_sleep_start();
}