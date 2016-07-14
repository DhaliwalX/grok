#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
#include <stdexcept>

namespace grok {

class JSError : public std::runtime_error {
public:
    JSError(std::string prefix, std::string msg)
        : std::runtime_error(std::move(msg)), prefix_{ std::move(prefix) }
    { }

    const char *what() const noexcept override
    {
        static std::string result;
        result = (prefix_ + ": " + std::runtime_error::what());
        return result.c_str();
    }
private:
    std::string prefix_;
};

class SyntaxError : public JSError {
public:
    SyntaxError(std::string str = "")
        : JSError("SyntaxError", std::move(str))
    { }

    const char *what() const noexcept override
    {
        return JSError::what();
    }
};

class TypeError : public JSError {
public:
    TypeError(std::string msg = "")
        : JSError("TypeError", std::move(msg))
    { }

    const char *what() const noexcept override
    {
        return JSError::what();
    }
};

class ReferenceError : public JSError {
public:
    ReferenceError(std::string msg = "")
        : JSError("ReferenceError", std::move(msg))
    { }
    
    const char *what() const noexcept override
    {
        return JSError::what();
    }
};

}

#endif
