#ifndef LETTER_H
#define LETTER_H

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

#endif
