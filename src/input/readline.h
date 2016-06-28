#ifndef READLINE_H_
#define READLINE_H_

#include <readline/readline.h>
#include <readline/history.h>
#include <string>

namespace grok {
namespace input {

/// ReadLineBuffer ::= class to manage `rl_line_buffer`
class ReadLineBuffer {
public:
    ReadLineBuffer() = default;

    /// SetText ::= set the buffer
    void SetText(std::string txt);

    /// GetText ::= get the buffer, remember can't modify the buffer
    /// directly
    std::string GetBuffer() const;
};

/// ReadLine ::= a simple C++ wrapper for gnu readline
class ReadLine {
public:
    using ReadLineFunction = rl_command_func_t;

    ReadLine(std::string prompt)
        : prompt_{ std::move(prompt) }, eof_{ false }
    { }

    ~ReadLine() = default;

    /// Read ::= read a line
    std::string Read();

    /// BindKey ::= bind a key to function
    void BindKey(int key, ReadLineFunction func);

    bool Eof() const { return eof_; }
private:

    std::string prompt_;
    bool eof_;
};

}   // input
}   // grok

#endif // readline.h
