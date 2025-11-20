/*
 * ESP32 SSD1306 128x32 OLED using the modern U8g2 library
 *
 * Description: An interrupt-driven hierarchical menu system for the ESP32 and SSD1306 display.
 *              The display is only updated when a button action occurs.
 *              *** MODIFIED: Long press action triggers immediately after duration is met. ***
 *              *** MODIFIED for buttons with external pull-down resistors (press = HIGH) ***
 *
 * Navigation:
 * - Button 1 (Up):    Moves the selection up.
 * - Button 2 (Down):  Moves the selection down.
 * - Button 3 (Short): Selects an item, entering a submenu or triggering an action.
 * - Button 3 (Hold):  Goes back to the previous menu level.
 *
 * REFACTOR SUMMARY:
 * - Implemented a non-blocking delay for the action screen.
 * - Centralized all button press and hold logic into the handleActions() function.
 * - Encapsulated UI state variables into a single struct.
 * - Added a preprocessor macro for conditional debug serial output.
 */

// Core Arduino and the modern U8g2 library
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// --- REFACTORED: Debug Macro ---
// Set to 0 to disable all serial output for a production build
#define DEBUG 1
#if DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif


// --- Pin Definitions ---
#define SDA 10
#define SCL 4

#define BUTTON1 9  // Up
#define BUTTON2 20  // Down
#define BUTTON3 21  // Select / Back (Hold)


// --- U8g2 Display Object Instantiation ---
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL, /* data=*/SDA);


// --- BITMAP DATA (Unchanged as per request) ---

// 'battery_icon', 16x16px
const unsigned char epd_bitmap_battery_icon[] PROGMEM = {
	0x00, 0x00, 0xc0, 0x03, 0xc0, 0x03, 0xf0, 0x0f, 0x10, 0x08, 0xd0, 0x0b, 0xd0, 0x0b, 0x10, 0x08,
	0xd0, 0x0b, 0xd0, 0x0b, 0x10, 0x08, 0xd0, 0x0b, 0xd0, 0x0b, 0x10, 0x08, 0xf0, 0x0f, 0x00, 0x00
};
// 'folder_icon', 16x16px
const unsigned char epd_bitmap_folder_icon[] PROGMEM = {
	0x00, 0x00, 0x3f, 0x00, 0xe1, 0x3f, 0x01, 0x20, 0x01, 0x20, 0x01, 0xff, 0x81, 0x80, 0x7d, 0x80,
	0x05, 0x80, 0x03, 0x40, 0x03, 0x40, 0x03, 0x20, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'gps_icon', 16x16px
const unsigned char epd_bitmap_gps_icon[] PROGMEM = {
	0x38, 0x00, 0x04, 0x00, 0x32, 0x00, 0x09, 0x30, 0x25, 0x48, 0x15, 0x84, 0x80, 0x83, 0xc0, 0x43,
	0xc0, 0x27, 0xc0, 0x1f, 0x20, 0x1f, 0x10, 0x1e, 0x08, 0x0e, 0x08, 0x01, 0x90, 0x00, 0x60, 0x00
};
// 'location_icon', 16x16px
const unsigned char epd_bitmap_location_icon[] PROGMEM = {
	0xc0, 0x07, 0x60, 0x0c, 0xb0, 0x1b, 0xd0, 0x17, 0xd0, 0x17, 0xd0, 0x17, 0xf0, 0x1f, 0xe0, 0x0f,
	0xc0, 0x07, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00
};
// 'placeholder_icon', 16x16px
const unsigned char epd_bitmap_placeholder_icon[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0xf0, 0x07, 0xf0, 0x0f, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0xf0, 0x0f,
	0xf0, 0x07, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'settings_icon', 16x16px
const unsigned char epd_bitmap_settings_icon[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x90, 0x09, 0xb8, 0x1d, 0xf0, 0x0f, 0x60, 0x06, 0x3c, 0x3c,
	0x3c, 0x3c, 0x60, 0x06, 0xf0, 0x0f, 0xb8, 0x1d, 0x90, 0x09, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00
};
// 'track_shot_icon', 16x16px
const unsigned char epd_bitmap_track_shot_icon[] PROGMEM = {
	0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x06, 0x60, 0x06, 0x60,
	0x00, 0x00, 0x03, 0xc0, 0x03, 0xc0, 0x00, 0x00, 0x88, 0x88, 0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00
};
// 'hierarchy_marker', 8x8px
const unsigned char epd_bitmap_hierarchy_marker[] PROGMEM = {
	0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'scroll_bar_knob', 8x8px
const unsigned char epd_bitmap_scroll_bar_knob[] PROGMEM = {
	0x00, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0x00, 0x00
};
// 'down_arrow', 16x8px
const unsigned char epd_bitmap_down_arrow[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x1f, 0xe0, 0x07, 0x80, 0x01, 0x00, 0x00
};
// 'up_arrow', 16x8px
const unsigned char epd_bitmap_up_arrow[] PROGMEM = {
	0x00, 0x00, 0x80, 0x01, 0xe0, 0x07, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'scroll_bar', 8x32px
const unsigned char epd_bitmap_scroll_bar[] PROGMEM = {
	0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
	0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00
};
// 'item_outline', 128x32px
const unsigned char epd_bitmap_item_outline[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07,
	0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// --- MENU DEFINITION ---

struct MenuItem {
	const char* title;
	const unsigned char* icon;
	int8_t parent;
	uint8_t firstChild;
	uint8_t numChildren;
	bool isAction;
};

// Define the entire menu structure in a single array
MenuItem menuItems[] = {
	// Index 0: Main Menu (Root)
	{ "Main Menu", nullptr, -1, 1, 3, false },
	// Children of Main Menu (Parent: 0)
	{ "Track Shot", epd_bitmap_track_shot_icon, 0, 4, 1, false },
	{ "Shot Log", epd_bitmap_folder_icon, 0, 5, 3, false },
	{ "Settings", epd_bitmap_settings_icon, 0, 8, 2, false },

	// Index 4: Track Shot Submenu
	{ "Place Point", epd_bitmap_placeholder_icon, 1, 0, 0, true },

	// Index 5: Shot Log Submenu
	{ "Club 1", epd_bitmap_placeholder_icon, 2, 0, 0, true },
	{ "Club 2", epd_bitmap_placeholder_icon, 2, 0, 0, true },
	{ "Club 3", epd_bitmap_placeholder_icon, 2, 0, 0, true },

	// Index 8: Settings Submenu
	{ "Reset GPS", epd_bitmap_gps_icon, 3, 0, 0, true },
	{ "Battery", epd_bitmap_battery_icon, 3, 0, 0, true }
};


// --- REFACTORED: State Management Variables ---
// Grouped UI state into a single struct
struct UIState {
	int currentMenuIndex;
	int currentSelection;
};
UIState ui = { 0, 0 };  // Initialize to root menu

// A non-blocking timer for the action screen
unsigned long actionScreenVisibleUntil = 0;


// --- Interrupt and State Flags ---
volatile bool button1Pressed = false;
volatile bool button2Pressed = false;
volatile bool button3StateChanged = false;
volatile bool displayNeedsUpdate = true;  // Start with true to draw initial screen


// --- Button state tracking ---
const unsigned long LONG_PRESS_DURATION = 700;
unsigned long button3PressStartTime = 0;
bool button3IsHeld = false;
bool longPressActionTaken = false;


// --- Debouncing ---
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;


// --- Function Prototypes ---
void handleActions();
void drawMenu();
void drawActionScreen(const char* actionText);
int getMenuDepth();


// --- Interrupt Service Routines (ISRs) ---
void IRAM_ATTR handleButton1Interrupt() {
	if ((millis() - lastDebounceTime) > debounceDelay) {
		button1Pressed = true;
		lastDebounceTime = millis();
	}
}

void IRAM_ATTR handleButton2Interrupt() {
	if ((millis() - lastDebounceTime) > debounceDelay) {
		button2Pressed = true;
		lastDebounceTime = millis();
	}
}

void IRAM_ATTR handleButton3Interrupt() {
	if ((millis() - lastDebounceTime) > debounceDelay) {
		button3StateChanged = true;
		lastDebounceTime = millis();
	}
}


void setup(void) {
	Serial.begin(115200);
	DEBUG_PRINTLN("\n--- System Setup ---");

	DEBUG_PRINTLN("Initializing pins for external pull-down resistors...");
	pinMode(BUTTON1, INPUT);
	pinMode(BUTTON2, INPUT);
	pinMode(BUTTON3, INPUT);

	DEBUG_PRINTLN("Attaching interrupts...");
	attachInterrupt(digitalPinToInterrupt(BUTTON1), handleButton1Interrupt, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON2), handleButton2Interrupt, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON3), handleButton3Interrupt, CHANGE);

	DEBUG_PRINTLN("Initializing display...");
	u8g2.begin();

	DEBUG_PRINTLN("Setup complete. Waiting for interrupts.");
}

void loop(void) {
	// Handle all button actions based on flags set by ISRs
	handleActions();

	// --- REFACTORED: Non-blocking delay for action screen ---
	// Check if the action screen's visibility duration has expired
	if (actionScreenVisibleUntil > 0 && millis() >= actionScreenVisibleUntil) {
		displayNeedsUpdate = true;     // Trigger a redraw of the menu
		actionScreenVisibleUntil = 0;  // Reset the timer
	}

	// Only update the display if something has changed
	if (displayNeedsUpdate) {
		DEBUG_PRINTLN("\n----- Display Update Triggered -----");
		// Do not draw the menu if an action screen is meant to be visible
		if (actionScreenVisibleUntil == 0) {
			drawMenu();
		}
		displayNeedsUpdate = false;  // Reset the flag
	}
}

void handleActions() {
	// --- Button 1 (Up) ---
	if (button1Pressed) {
		DEBUG_PRINTLN("handleActions(): Button 1 (Up) event.");
		ui.currentSelection--;
		if (ui.currentSelection < 0) {
			ui.currentSelection = menuItems[ui.currentMenuIndex].numChildren - 1;
		}
		displayNeedsUpdate = true;
		DEBUG_PRINT("  > ui.currentSelection changed to: ");
		DEBUG_PRINTLN(ui.currentSelection);
		button1Pressed = false;  // Reset the flag
	}

	// --- Button 2 (Down) ---
	if (button2Pressed) {
		DEBUG_PRINTLN("handleActions(): Button 2 (Down) event.");
		ui.currentSelection++;
		if (ui.currentSelection >= menuItems[ui.currentMenuIndex].numChildren) {
			ui.currentSelection = 0;
		}
		displayNeedsUpdate = true;
		DEBUG_PRINT("  > ui.currentSelection changed to: ");
		DEBUG_PRINTLN(ui.currentSelection);
		button2Pressed = false;  // Reset the flag
	}

	// --- REFACTORED: Centralized Button 3 Logic ---
	// Continuously check for a long press while the button is held
	if (button3IsHeld && !longPressActionTaken && (millis() - button3PressStartTime > LONG_PRESS_DURATION)) {
		DEBUG_PRINTLN("handleActions(): Button 3 LONG PRESS detected.");

		// --- LONG PRESS ACTION ---
		if (menuItems[ui.currentMenuIndex].parent != -1) {
			DEBUG_PRINTLN("  > Navigating back to parent menu.");
			ui.currentMenuIndex = menuItems[ui.currentMenuIndex].parent;
			ui.currentSelection = 0;
			displayNeedsUpdate = true;
			DEBUG_PRINT("  > ui.currentMenuIndex changed to: ");
			DEBUG_PRINTLN(ui.currentMenuIndex);
		} else {
			DEBUG_PRINTLN("  > Already at root menu. No action taken.");
		}
		longPressActionTaken = true;  // Ensure this action only runs once per press
	}

	// Handle state change from the ISR
	if (button3StateChanged) {
		if (digitalRead(BUTTON3) == HIGH) {  // Button was just PRESSED
			DEBUG_PRINTLN("handleActions(): Button 3 press detected. Starting timer.");
			button3PressStartTime = millis();
			button3IsHeld = true;
			longPressActionTaken = false;  // Reset for the new press
		} else {                         // Button was just RELEASED
			DEBUG_PRINTLN("handleActions(): Button 3 released.");
			button3IsHeld = false;

			// --- SHORT PRESS ACTION ---
			// Only act if the long press action wasn't already triggered
			if (!longPressActionTaken) {
				DEBUG_PRINTLN("  > It was a SHORT PRESS.");
				int selectedItemIndex = menuItems[ui.currentMenuIndex].firstChild + ui.currentSelection;

				if (menuItems[selectedItemIndex].isAction) {
					DEBUG_PRINT("    Triggering ACTION: ");
					DEBUG_PRINTLN(menuItems[selectedItemIndex].title);
					drawActionScreen(menuItems[selectedItemIndex].title);
					// REFACTORED: Use non-blocking timer instead of delay()
					actionScreenVisibleUntil = millis() + 1500;
				} else if (menuItems[selectedItemIndex].numChildren > 0) {
					DEBUG_PRINT("    Entering SUBMENU: ");
					DEBUG_PRINTLN(menuItems[selectedItemIndex].title);
					ui.currentMenuIndex = selectedItemIndex;
					ui.currentSelection = 0;
					displayNeedsUpdate = true;
					DEBUG_PRINT("    ui.currentMenuIndex changed to: ");
					DEBUG_PRINTLN(ui.currentMenuIndex);
				}
			} else {
				DEBUG_PRINTLN("  > It was a LONG PRESS release. No action needed.");
			}
		}
		button3StateChanged = false;  // Reset the flag
	}
}

int getMenuDepth() {
	DEBUG_PRINTLN("getMenuDepth(): Calculating menu depth...");
	int depth = 0;
	int tempIndex = ui.currentMenuIndex;
	while (menuItems[tempIndex].parent != -1) {
		tempIndex = menuItems[tempIndex].parent;
		depth++;
	}
	DEBUG_PRINT("  > Calculated depth: ");
	DEBUG_PRINTLN(depth);
	return depth;
}

void drawMenu() {
	DEBUG_PRINTLN("drawMenu(): Redrawing menu screen.");
	u8g2.clearBuffer();
	u8g2.setBitmapMode(1);

	MenuItem currentMenu = menuItems[ui.currentMenuIndex];
	int selectedItemIndex = currentMenu.firstChild + ui.currentSelection;
	MenuItem selectedItem = menuItems[selectedItemIndex];

	DEBUG_PRINT("  > ui.currentMenuIndex: ");
	DEBUG_PRINTLN(ui.currentMenuIndex);
	DEBUG_PRINT("  > ui.currentSelection: ");
	DEBUG_PRINTLN(ui.currentSelection);
	DEBUG_PRINT("  > Drawing item: ");
	DEBUG_PRINTLN(selectedItem.title);

	// NOTE: As per your request, the original bitmap for the item outline has been preserved.
	// For memory optimization, this could be replaced with u8g2.drawRFrame(0, 0, 128, 32, 4);
	u8g2.drawXBM(0, 0, 128, 32, epd_bitmap_item_outline);

	int menuDepth = getMenuDepth();
	for (int i = 0; i < menuDepth; i++) {
		u8g2.drawXBM(i * 8, 0, 8, 8, epd_bitmap_hierarchy_marker);
	}

	u8g2.drawXBM(4, 8, 16, 16, selectedItem.icon);

	u8g2.setFont(u8g2_font_7x14B_tf);
	u8g2.drawStr(23, 20, selectedItem.title);

	if (currentMenu.numChildren > 1) {
		DEBUG_PRINTLN("  > Drawing scroll bar.");
		u8g2.drawXBM(120, 0, 8, 32, epd_bitmap_scroll_bar);

		if (ui.currentSelection > 0) {
			u8g2.drawXBM(54, 0, 16, 8, epd_bitmap_up_arrow);
		}

		if (ui.currentSelection < currentMenu.numChildren - 1) {
			u8g2.drawXBM(54, 24, 16, 8, epd_bitmap_down_arrow);
		}

		int knobY = map(ui.currentSelection, 0, currentMenu.numChildren - 1, 0, 24);
		u8g2.drawXBM(120, knobY, 8, 8, epd_bitmap_scroll_bar_knob);
	}

	DEBUG_PRINTLN("  > Sending buffer to display.");
	u8g2.sendBuffer();
}

void drawActionScreen(const char* actionText) {
	DEBUG_PRINTLN("drawActionScreen(): Displaying action screen.");
	DEBUG_PRINT("  > Action Text: ");
	DEBUG_PRINTLN(actionText);

	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_7x14B_tf);

	char buffer[32];
	strcpy(buffer, "Action: ");
	strcat(buffer, actionText);

	u8g2.drawStr(23, 20, buffer);

	DEBUG_PRINTLN("  > Sending buffer to display.");
	u8g2.sendBuffer();
}
