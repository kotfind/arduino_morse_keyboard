#ifndef MORSER_DECODER_H
#define MORSER_DECODER_H

#include <Keyboard.h>

#include "layout.h"
#include "Letter.h"
#include "Streaming.h"
#include "Timer.h"

class MorseDecoder {
    public:
        MorseDecoder() {
            timer = new Timer(0);
            letter = new Letter();
        }

        void update() {
            time_t dt = timer->delta(0);
            if (button_down_) {
                if (dt > BACKSPACE_TIME) {
                    Keyboard.press(8);
                }
            } else {
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
            if (dt > BACKSPACE_TIME) {
                Keyboard.release(8);
                letter->clear();
                return;
            }

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
        const time_t STOP_TIME       = 30 * TIME_UNIT_;
        const time_t BACKSPACE_TIME  = 6  * TIME_UNIT_;
        Letter* letter;
        Timer* timer;
        bool button_down_ = 0;
};

#endif
