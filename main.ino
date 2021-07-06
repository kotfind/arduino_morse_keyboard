/** Circuit description
 * button on pin 8 and GND
 */

#include <Keyboard.h>

using time_t = unsigned long;

// Serial and Keyboard streaming syntax
template<class T>
Print& operator<<(Print &obj, T arg) {
    obj.print(arg);
    return obj;
} 

class Timer {
    public:
        Timer() : Timer(millis()) {}

        Timer(const time_t& time) {
            last_time_ = time;
        }

        time_t time() {
            return millis();
        }

        time_t delta(bool update = 1) {
            time_t now_time = time();
            time_t delta_time = now_time - last_time_;
            if (update) {
                last_time_ = now_time;
            }
            return delta_time;
        }

    private:
        time_t last_time_;
};

struct Letter {
    int buffer_;
    int buffer_n_;
    char symbol_;

    Letter() : Letter(0, 0, 0) {}

    Letter(int buffer, int buffer_n, char symbol) {
        buffer_ = buffer;
        buffer_n_ = buffer_n;
        symbol_ = symbol;
    }

    void clear() {
        buffer_ = 0;
        buffer_n_ = 0;
    }
};

bool operator==(const Letter& lhs, const Letter& rhs) {
    return lhs.buffer_ == rhs.buffer_ && lhs.buffer_n_ == rhs.buffer_n_;
}

Letter patterns[] = {
    // Letters
    {0b01  , 2, 'a'},
    {0b1000, 4, 'b'},
    {0b1010, 4, 'c'},
    {0b100 , 3, 'd'},
    {0b0   , 1, 'e'},
    {0b0010, 4, 'f'},
    {0b110 , 3, 'g'},
    {0b0000, 4, 'h'},
    {0b00  , 2, 'i'},
    {0b0111, 4, 'j'},
    {0b101 , 3, 'k'},
    {0b0100, 4, 'l'},
    {0b11  , 2, 'm'},
    {0b10  , 2, 'n'},
    {0b111 , 3, 'o'},
    {0b0110, 4, 'p'},
    {0b1101, 4, 'q'},
    {0b010 , 3, 'r'},
    {0b000 , 3, 's'},
    {0b1   , 1, 't'},
    {0b001 , 3, 'u'},
    {0b0001, 4, 'v'},
    {0b011 , 3, 'w'},
    {0b1000, 4, 'x'},
    {0b1011, 4, 'y'},
    {0b1100, 4, 'z'},

    // Numbers
    {0b01111, 5, '1'},
    {0b00111, 5, '2'},
    {0b00011, 5, '3'},
    {0b00001, 5, '4'},
    {0b00000, 5, '5'},
    {0b10000, 5, '6'},
    {0b11000, 5, '7'},
    {0b11100, 5, '8'},
    {0b11110, 5, '9'},
    {0b11111, 5, '0'},

    // Symbols
    {0b010101, 6, '.'},
    {0b110011, 6, ','},
    {0b001100, 6, '?'},

    // Enter
    {0b00101, 5, '\n'},
};

class MorseDecoder {
    public:
        MorseDecoder() {
            timer = new Timer(0);
            letter = new Letter();
        }

        void update() {
            time_t dt = timer->delta(0);
            // Was released during dt
            if (!button_down_) {
                if (dt > STOP_TIME) {
                    letter->clear();
                } else if (dt > END_SYMBOL_TIME) {
                    endSymbol();
                }
            }
        }

        void pressed() {
            button_down_ = 1;
            time_t dt = timer->delta();
            if (dt > END_WORD_TIME && dt < STOP_TIME) {
                endWord();
            }
        }

        void released() {
            button_down_ = 0;
            time_t dt = timer->delta();
            // Was pressed during dt
            if (dt < (DOT_TIME + DASH_TIME) / 2) {
                symbol(0);
            } else {
                symbol(1);
            }
        }

    private:
        void symbol(bool dash) {
            if (letter->buffer_n_ >= 32) {
                Serial << "ERROR: Too long sequence. Letter cleared.\n";
                letter->clear();
            }
            letter->buffer_ <<= 1;
            letter->buffer_ |= dash;
            ++letter->buffer_n_;
        }

        void endWord() {
            endSymbol();
            Serial << "/\n";
            Keyboard << ' ';
        }

        void endSymbol() {
            if (letter->buffer_n_ == 0) return;
            for (int i = 0; i < letter->buffer_n_; ++i) {
                Serial << (letter->buffer_ & bit(i) ? '-' : '.');
            }

            bool found = 0;
            for (const Letter& pattern : patterns) {
                if (pattern == *letter) {
                    Serial << pattern.symbol_ << '\n';
                    Keyboard << pattern.symbol_;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Serial << "ERROR: Unknown symbol\n";
            }
            letter->clear();
        }

        const time_t TIME_UNIT_ = 120;
        const time_t DOT_TIME        = 1  * TIME_UNIT_;
        const time_t DASH_TIME       = 3  * TIME_UNIT_;
        const time_t END_SYMBOL_TIME = 3  * TIME_UNIT_;
        const time_t END_WORD_TIME   = 7  * TIME_UNIT_;
        const time_t STOP_TIME       = 100 * TIME_UNIT_;
        Letter* letter;
        Timer* timer;
        bool button_down_ = 0;
};

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
