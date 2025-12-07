#include "button_handler.h"

ButtonHandler::ButtonHandler() {
    // Initialize states to HIGH (released state for INPUT_PULLUP)
    lastB1State = HIGH;
    currentB1State = HIGH;
    lastB1DebounceTime = 0;

    lastB2State = HIGH;
    currentB2State = HIGH;
    lastB2DebounceTime = 0;

    lastB3State = HIGH;
    currentB3State = HIGH;
    lastB3DebounceTime = 0;
    b3PressStartTime = 0;
    lastLongPressActionTime = 0;
    b3IsHeld = false;
    b3LongActionTaken = false;
}

bool ButtonHandler::begin() {
    pinMode(BTN_1_PIN, INPUT_PULLUP);
    pinMode(BTN_2_PIN, INPUT_PULLUP);
    pinMode(BTN_3_PIN, INPUT_PULLUP);
    return true;
}

ButtonEvent ButtonHandler::checkButtons() {
    unsigned long currentMillis = millis();

    // -----------------------------
    // BUTTON 1 (DOWN) - Action on Release
    // -----------------------------
    int reading1 = digitalRead(BTN_1_PIN);
    if (reading1 != lastB1State) {
        lastB1DebounceTime = currentMillis;
    }

    if ((currentMillis - lastB1DebounceTime) > DEBOUNCE_DELAY) {
        if (reading1 != currentB1State) {
            currentB1State = reading1;
            // Trigger action only when transitioning from HIGH to LOW (Release)
            // Note: Logic inverted because PULLUP: LOW is pressed, HIGH is released.
            // If currentB1State becomes HIGH, it means it was LOW (pressed) and is now HIGH (released).
            if (currentB1State == HIGH) {
                return EVENT_DOWN;
            }
        }
    }
    lastB1State = reading1;

    // -----------------------------
    // BUTTON 2 (UP) - Action on Release
    // -----------------------------
    int reading2 = digitalRead(BTN_2_PIN);
    if (reading2 != lastB2State) {
        lastB2DebounceTime = currentMillis;
    }

    if ((currentMillis - lastB2DebounceTime) > DEBOUNCE_DELAY) {
        if (reading2 != currentB2State) {
            currentB2State = reading2;
            if (currentB2State == HIGH) {
                return EVENT_UP;
            }
        }
    }
    lastB2State = reading2;

    // -----------------------------
    // BUTTON 3 (SELECT / BACK)
    // -----------------------------
    int reading3 = digitalRead(BTN_3_PIN);
    if (reading3 != lastB3State) {
        lastB3DebounceTime = currentMillis;
    }

    if ((currentMillis - lastB3DebounceTime) > DEBOUNCE_DELAY) {
        if (reading3 != currentB3State) {
            currentB3State = reading3;

            if (currentB3State == LOW) {
                // PRESS START
                b3PressStartTime = currentMillis;
                b3IsHeld = true;
                b3LongActionTaken = false;
            } else {
                // RELEASE
                b3IsHeld = false;
                // Only trigger Short Action if Long Action wasn't taken
                if (!b3LongActionTaken) {
                    return EVENT_SELECT;
                }
            }
        }
    }
    lastB3State = reading3;

    // -----------------------------
    // BUTTON 3 HOLD CHECK (Running Continuously)
    // -----------------------------
    if (b3IsHeld) {
        if ((currentMillis - b3PressStartTime) > LONG_PRESS_DURATION) {
            // Initial Long Press
            if (!b3LongActionTaken) {
                b3LongActionTaken = true;
                lastLongPressActionTime = currentMillis;
                return EVENT_BACK;
            } 
            // Continuous Hold (Repeat Action)
            else if ((currentMillis - lastLongPressActionTime) > REPEAT_DELAY) {
                lastLongPressActionTime = currentMillis;
                return EVENT_BACK;
            }
        }
    }

    return EVENT_NONE;
}
