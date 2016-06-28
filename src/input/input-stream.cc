#include "input/input-stream.h"

namespace grok {
namespace input {


std::streamsize ReadLineSource::read(char_type *s, std::streamsize n)
{
    if (prompt_) {
        if (RL_.Eof())
            return -1;
        auto buffer = RL_.Read();
        buffer_ += buffer;
        prompt_ = false;
    }
    auto remaining = buffer_.length() - pos_;

    if (((std::streamsize)remaining) > n) {
        std::copy(buffer_.begin() + pos_, buffer_.begin() + pos_ + n,
            s);
        pos_ += n;
        return n;
    } else if (remaining != 0) {
        std::copy(buffer_.begin() + pos_, buffer_.end(), s);
        auto amount = buffer_.end() - (buffer_.begin() + pos_);
        pos_ += amount;
        return amount;
    } else {
        prompt_ = true;
        return -1;
    }
}

} // input
} // grok
