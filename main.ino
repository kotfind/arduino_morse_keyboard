using time_t = unsigned long;

// Serial streaming syntax
template<class T>
Print& operator<<(Print &obj, T arg) {
    obj.print(arg);
    return obj;
} 

class Timer {
    public:
        Timer() {
            last_time_ = millis();
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
    {0b000, 3, 's'},
    {0b111, 3, 'o'},
};

class MorseDecoder {
    public:
        MorseDecoder() {
            timer = new Timer();
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
            letter->buffer_ |= bit(letter->buffer_n_++) * dash;
        }

        void endWord() {
            endSymbol();
            Serial << "/\n";
        }

        void endSymbol() {
            if (letter->buffer_n_ == 0) return;
            // TODO: DEBUG
            // for (int i = 0; i < letter->buffer_n_; ++i) {
            //     Serial << (letter->buffer_ & bit(i) ? '-' : '.');
            // }
            // Serial << '\n';
            bool found = 0;
            for (const Letter& pattern : patterns) {
                if (pattern == *letter) {
                    Serial << pattern.symbol_ << '\n';
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Serial << "ERROR: Unknown symbol\n";
            }
            letter->clear();
        }

        const time_t TIME_UNIT_ = 150;
        const time_t DOT_TIME        = 1  * TIME_UNIT_;
        const time_t DASH_TIME       = 3  * TIME_UNIT_;
        const time_t END_SYMBOL_TIME = 3  * TIME_UNIT_;
        const time_t END_WORD_TIME   = 7  * TIME_UNIT_;
        const time_t STOP_TIME       = 15 * TIME_UNIT_;
        Letter* letter;
        Timer* timer;
        bool button_down_ = 0;
};

const int BTN_PIN = 8;
MorseDecoder* decoder;

void setup() {
    Serial.begin(9600);

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
