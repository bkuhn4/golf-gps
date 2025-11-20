#include <Wire.h>
#include <SparkFunBQ27441.h>

// I2C pins for ESP32-C3
const int I2C_SDA_PIN = 10;
const int I2C_SCL_PIN = 4;

// LiPo battery capacity in mAh
const unsigned int BATTERY_CAPACITY = 1500;

// Function prototype for the I2C scanner
void scanI2C();

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nESP32-C3 BQ27411 Battery Monitor");
  Serial.println("---------------------------------");

  // Initialize I2C communication with custom pins
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // Perform an I2C scan first
  scanI2C();

  // Initialize the BQ27441
  if (!lipo.begin()) {
    Serial.println("\nError: Unable to communicate with BQ27411-G1A.");
    Serial.println("Check wiring and ensure the device is powered.");
    Serial.println("The I2C scan above should show a device at address 0x55.");
    while (1);
  }
  Serial.println("\nBQ27411-G1A connected successfully.");

  // Configure the battery capacity
  if (lipo.setCapacity(BATTERY_CAPACITY)) {
    Serial.print("Battery capacity set to: ");
    Serial.print(BATTERY_CAPACITY);
    Serial.println(" mAh");
  } else {
    Serial.println("Error: Could not set battery capacity.");
  }
  Serial.println("---------------------------------");
}

void loop() {
  // Read and print battery information every 5 seconds
  printBatteryStats();
  delay(5000);
}

/**
 * @brief Scans the I2C bus for connected devices and prints their addresses.
 */
void scanI2C() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning I2C bus...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("Scan complete.\n");
  }
}

/**
 * @brief Reads and prints the battery statistics to the Serial Monitor.
 */
void printBatteryStats() {
  // Read battery voltage
  unsigned int voltage = lipo.voltage();
  Serial.print("Battery Voltage: ");
  Serial.print(voltage);
  Serial.println(" mV");

  // Read state of charge
  unsigned int soc = lipo.soc();
  Serial.print("State of Charge: ");
  Serial.print(soc);
  Serial.println(" %");

  // **** FIX: Use the correct 'REMAIN' enum ****
  unsigned int remainingCapacity = lipo.capacity(REMAIN);
  Serial.print("Remaining Capacity: ");
  Serial.print(remainingCapacity);
  Serial.println(" mAh");

  // **** FIX: Use the correct 'AVG' enum ****
  int averageCurrent = lipo.current(AVG);
  Serial.print("Average Current: ");
  Serial.print(averageCurrent);
  Serial.println(" mA (positive is discharging, negative is charging)");

  // **** FIX: Use the correct 'BATTERY' enum. The library returns Celsius by default. ****
  //unsigned int temperature = lipo.temperature(BATTERY);
  unsigned int temperature = (lipo.temperature(BATTERY) / 10) - 273.15;
  Serial.print("Battery Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  // Read battery state of health
  unsigned int soh = lipo.soh();
  Serial.print("State of Health: ");
  Serial.print(soh & 0x00FF); // The lower byte represents SOH
  Serial.println(" %");

  Serial.println("---------------------------------");
}
