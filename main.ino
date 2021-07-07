/** Circuit description
 * button on pin 8 and GND
 */

#include "MorseDecoder.h"

const int BTN_PIN = 8;
MorseDecoder* decoder;

void setup() {
    Serial.begin(9600);
    Keyboard.begin();

    pinMode(BTN_PIN, INPUT_PULLUP);

    decoder = new MorseDecoder();
}

void loop() {
    static bool old_val = 0;
    bool new_val = digitalRead(BTN_PIN) == LOW;
    if (old_val != new_val) {
        if (new_val) {
            decoder->pressed();
        } else {
            decoder->released();
        }
        old_val = new_val;
    }
    decoder->update();
    delay(10);
}
