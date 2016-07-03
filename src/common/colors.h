#ifndef COLOR_H_
#define COLOR_H_

#include <string>
#include <cstdlib>
#include <unistd.h>

namespace Color {

enum Colors {
    fblack,
    fred,
    fgreen,
    fyellow,
    fblue,
    fmagenta,
    fcyan,
    fwhite
};

enum color_attr {
    off,
    bold,
    dim,
    underscore,
    blink,
    reverse,
    concealed,
};

static std::string color_codes[] = {
    "30",
    "31",
    "32",
    "33",
    "34",
    "35",
    "36",
    "37"
};

static std::string attr_code[] = {
    "0",
    "1",
    "2",
    "4",
    "5",
    "7",
    "8"
};

static inline std::string Color(int fcolor, int attr = 0)
{
    if (!isatty(STDOUT_FILENO))
        return "";
    return std::string("\x1b[") + color_codes[fcolor] + ";"
        + ";" + attr_code[attr] + "m";
}

static inline std::string Attr(int attr)
{
    if (!isatty(STDOUT_FILENO))
        return "";
    return std::string("\x1b[") + attr_code[attr] + "m";
}

static inline std::string Reset()
{
    if (!isatty(STDOUT_FILENO))
        return "";
    return "\x1b[0;0;0m";
}

}

#endif
