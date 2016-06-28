#include "input/readline.h"
#include <algorithm>

namespace grok {
namespace input {

void ReadLineBuffer::SetText(std::string txt)
{
    size_t len = rl_end;

    if (len < txt.length()) {
        rl_extend_line_buffer(txt.length() - len);
    }
    std::copy(txt.begin(), txt.end(), rl_line_buffer);
    rl_point = rl_end = txt.length();
}

std::string ReadLineBuffer::GetBuffer() const
{
    return std::string(rl_line_buffer, rl_line_buffer + rl_end);
}

std::string ReadLine::Read()
{
    char *result = readline(prompt_.c_str());
    if (result == 0) {
        eof_ = true;
        return std::string();
    }
    auto ret = std::string(result);

    if (*result) {
        add_history(result);
    }
    free(result);
    return ret;
}

void ReadLine::BindKey(int key, ReadLineFunction func)
{
    rl_bind_key(key, func);
}

}
}
