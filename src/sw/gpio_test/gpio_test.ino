/*
  ESP32-C3 GPIO Toggling Sketch

  This sketch sets a predefined list of GPIO pins on the ESP32-C3 to HIGH,
  then LOW, alternating every 500 milliseconds.

  It is important to only use GPIOs that are safe for general purpose output
  and are exposed on your specific ESP32-C3 board.
*/

// Array of GPIO pins to be toggled.
// These are generally safe to use on most ESP32-C3 boards.
const int gpioPins[] = {0, 1,2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 21};
const int numPins = sizeof(gpioPins) / sizeof(gpioPins[0]);

void setup() {
  // Initialize all specified GPIO pins as outputs.
  for (int i = 0; i < numPins; i++) {
    pinMode(gpioPins[i], OUTPUT);
  }
}

void loop() {
  // Set all specified GPIO pins to HIGH.
  for (int i = 0; i < numPins; i++) {
    digitalWrite(gpioPins[i], HIGH);
  }
  delay(500); // Wait for half a second.

  // Set all specified GPIO pins to LOW.
  for (int i = 0; i < numPins; i++) {
    digitalWrite(gpioPins[i], LOW);
  }
  delay(500); // Wait for half a second.
}
