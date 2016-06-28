#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <iostream>

namespace grok {

using Any = boost::any;
namespace BPO = boost::program_options;

class Opts {
public:
    Opts()
        : desc{ "Available options" }, positional{}, vm{},
          argc{ 0 }, argv { nullptr }
    { }
    
    Opts(int argc, char **argv) :
        desc{ "Available options" }, positional{}, vm{},
        argc{ argc }, argv { argv }
    { }

    /// ParseOptions ::= parse the options
    void ParseOptions();

    void ParseOptions(int a, char **v)
    {
        argc = a;
        argv = v;
        ParseOptions();
    }

    /// HasOption ::= returns true if variables_map has given option
    bool HasOption(std::string str);

    /// GetOptionAs ::= returns the value of option
    template <typename T>
    T GetOptionAs(std::string str)
    {
        if (!HasOption(str)) {
            throw std::runtime_error("bad option asked");
        }
        auto Val = vm[str].as<T>();
        return Val;
    }

    template <typename T>
    void AddOption(std::string name, std::string help, T value)
    {
        desc.add_options()(name.c_str(), value, help.c_str());
    }

    void AddOption(std::string name, std::string help)
    {
        desc.add_options()(name.c_str(), help.c_str());
    }

    void AddPositionalOption(std::string name, int num)
    {
        positional.add(name.c_str(), num);
    }

    void PrintHelpAndExit()
    {
        std::cout << desc << std::endl;
        exit(0);
    }
private:
    boost::program_options::options_description desc;
    boost::program_options::positional_options_description positional;
    boost::program_options::variables_map vm;
    int argc;
    char **argv;
};

}


#endif // options.h
