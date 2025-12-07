#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>

// Pin Definitions
#define BTN_1_PIN 9
#define BTN_2_PIN 20
#define BTN_3_PIN 2

// Button Events
enum ButtonEvent {
    EVENT_NONE = 0,
    EVENT_UP,      // Button 2 Released
    EVENT_DOWN,    // Button 1 Released
    EVENT_SELECT,  // Button 3 Released (Short Press)
    EVENT_BACK     // Button 3 Held (Long Press)
};

class ButtonHandler {
public:
    ButtonHandler();
    bool begin();
    ButtonEvent checkButtons();

private:
    // Constants
    const unsigned long LONG_PRESS_DURATION = 400;
    const unsigned long DEBOUNCE_DELAY = 40;
    const unsigned long REPEAT_DELAY = 500; // Delay between repeated back actions

    // Button 1 State
    int lastB1State;
    int currentB1State;
    unsigned long lastB1DebounceTime;

    // Button 2 State
    int lastB2State;
    int currentB2State;
    unsigned long lastB2DebounceTime;

    // Button 3 State
    int lastB3State;
    int currentB3State;
    unsigned long lastB3DebounceTime;
    unsigned long b3PressStartTime;
    unsigned long lastLongPressActionTime; // Track last repeat action
    bool b3IsHeld;
    bool b3LongActionTaken;
};

#endif // BUTTON_HANDLER_H
