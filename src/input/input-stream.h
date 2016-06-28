#ifndef INPUT_STREAM_H_
#define INPUT_STREAM_H_

#include "readline.h"

#include <algorithm>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

namespace grok {
namespace input {

/// class ReadLineSource ::= a character source takes input from readline
class ReadLineSource {
public:
    using char_type = char;
    using category = boost::iostreams::source_tag;
    using size_type = std::string::size_type;

    ReadLineSource(ReadLine RL) :
        RL_{ RL }, buffer_{ }, pos_{ 0 }, lines_{ 0 }, prompt_{ false }
    { }

    std::streamsize read(char_type *s, std::streamsize n);

    std::string& container() { return buffer_; }
private:
    ReadLine RL_;
    std::string buffer_;
    size_type pos_;
    size_type lines_;
    bool prompt_;
};

using ReadLineStream = boost::iostreams::stream<ReadLineSource>;

}
}

#endif // input-stream.h