# EE 333 Pocket Caddy



## Description
Pocket Caddy is a handheld golf GPS and shot tracking device designed to help golfers track their performance and environmental conditions. It utilizes a high-precision u-blox GNSS module to measure shot distances and logs them to a micro SD card along with real-time temperature and humidity data. The device features an OLED display for easy navigation, a battery fuel gauge for power management, and a robust menu system for managing user profiles and reviewing shot history.

## Features
- **GPS Distance Tracking:** Calculates precise distances between shot start and end points using the Vincenty formula.
- **Shot Logging:** Records club used, distance, temperature, humidity, date, and time to an SD card.
- **Environmental Monitoring:** Real-time display of temperature and humidity using the SHT31 sensor.
- **Battery Management:** Monitors battery voltage, state of charge (SOC), and current consumption.
- **User Profiles:** Supports multiple user profiles for organizing shot data.
- **OLED Interface:** Clear, high-contrast menu system navigated via physical buttons.

## Hardware

The Pocket Caddy hardware is built around the ESP32-C3 microcontroller and includes the following components:

*   **Microcontroller:** Espressif ESP32-C3-WROOM-02
*   **GNSS Module:** u-blox NEO-M9N (High-precision GPS/GLONASS/Galileo)
*   **GNSS Antenna:** Taoglas SGGP.25.4.A.02
*   **Display:** 0.91" OLED I2C Display (128x32 pixels)
*   **Sensors:**
    *   Sensirion SHT30-DIS (Temperature & Humidity)
    *   TI BQ27411-G1 (LiPo Fuel Gauge)
*   **Storage:** Micro SD Card Slot
*   **Power:**
    *   USB-C Connector (Charging & Data)
    *   LiPo Battery Connector
    *   Coin Cell Battery Backup (for GNSS hot start)

## Dependencies

To compile the firmware, you will need to install the following libraries via the Arduino Library Manager:

*   **SparkFun u-blox GNSS Arduino Library** by SparkFun Electronics
*   **SparkFun BQ27441 LiPo Fuel Gauge Arduino Library** by SparkFun Electronics
*   **Adafruit SHT31 Library** by Adafruit
*   **U8g2** by olikraus

## Programming the Device

### First-Time Programming (Boot Mode)
When programming the device for the first time (or if it becomes unresponsive), you need to manually enter the bootloader mode:
1.  Hold down the **BOOT** button.
2.  While holding BOOT, press and release the **RESET** button.
3.  Release the **BOOT** button.
4.  The device is now in boot mode and ready to accept firmware.

### Arduino IDE Settings
To ensure the device communicates correctly over USB for serial monitoring and subsequent uploads:
1.  **Install Board Support:** Ensure the **esp32** board library by Espressif Systems is installed. If not, go to `Tools > Board > Boards Manager` and search `"esp32"`.
2.  **Select Board:** Go to `Tools > Board > esp32 > ESP32C3 Dev Module`.
3.  **USB CDC On Boot:** To avoid having to manually enter boot mode each time, go to `Tools > USB CDC On Boot > Enabled`.
4.  Select the correct COM port. `Tools > Port > COM#`.

## Usage

### Controls
The device is operated using three buttons:
-   **Button 1 (Up):** Navigate up in menus.
-   **Button 2 (Down):** Navigate down in menus.
-   **Button 3 (Select/Back):**
    -   **Short Press:** Select item / Confirm action.
    -   **Long Press:** Go back to the previous menu.

### Main Menu Options
-   **Track Shot:** Start a new shot tracking session. Mark your start point, go to your ball, mark the end point, and select the club used.
-   **Sensor View:** Displays raw sensor data including GPS coordinates (Lat/Lon), Satellite count (SIV), Temperature, Humidity, and Battery stats.
-   **Shot Log:** Review historical shot data organized by club.
-   **Settings:**
    -   **Units:** Toggle between Imperial (Yards/Fahrenheit) and Metric (Meters/Celsius).
    -   **Profiles:** Create, Rename, or Remove user profiles.

## Authors and acknowledgment
-   **Brady Kuhn**: Hardware | Software
-   **Bryan York**

## Project status
Active Development - Version 0.1
