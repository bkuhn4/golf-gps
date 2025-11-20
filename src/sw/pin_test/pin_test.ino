/******************************************************************************
   PROJECT:     GPIO Toggling Test for I2C Pins
   AUTHOR:      AI Assistant
   DATE:        11/5/2025
   VERSION:     1.0

   DESCRIPTION: This sketch configures the I2C pins (SCL/SDA) as standard
                digital outputs and toggles them HIGH and LOW once per
                second. This is used to verify that the GPIOs are functional
                and can be measured with a multimeter or oscilloscope.
 ******************************************************************************/

/* --- Tools Setup ---
  Board: esp32 > ESP32C3 Dev Module
*/

// --- PIN DEFINITIONS ---
// Define the I2C pins you want to test.
#define SCL_PIN 4       // The pin normally used for I2C Clock
#define SDA_PIN 10      // The pin normally used for I2C Data


// --- SETUP FUNCTION ---
// This function runs once when the board is powered on or reset.
void setup() {
  // Start serial communication for status messages.
  Serial.begin(115200);
  while (!Serial); // Wait for Serial to be ready
  Serial.println("\n--- GPIO Toggling Test ---");
  Serial.print("Toggling GPIO ");
  Serial.print(SCL_PIN);
  Serial.print(" (SCL) and GPIO ");
  Serial.print(SDA_PIN);
  Serial.println(" (SDA)");

  // Set both pins to be in OUTPUT mode.
  pinMode(SCL_PIN, OUTPUT);
  pinMode(SDA_PIN, OUTPUT);
  Serial.println("Pins set to OUTPUT mode.");
}


// --- MAIN LOOP ---
// This function runs repeatedly after setup() has finished.
void loop() {
  // Set the pins HIGH
  Serial.println("Setting pins HIGH");
  digitalWrite(SCL_PIN, HIGH);
  digitalWrite(SDA_PIN, HIGH);

  // Wait for one second (1000 milliseconds)
  delay(1000);

  // Set the pins LOW
  Serial.println("Setting pins LOW");
  digitalWrite(SCL_PIN, LOW);
  digitalWrite(SDA_PIN, LOW);

  // Wait for one second
  delay(1000);
}
