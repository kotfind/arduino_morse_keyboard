#ifndef TIMER_H
#define TIMER_H

using time_t = unsigned long;

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

#endif
