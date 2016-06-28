#include "options.h"
#include <iostream>

namespace grok {

void Opts::ParseOptions()
{
    try {
        auto parser = boost::program_options::command_line_parser(argc, argv);
        parser.options(desc);
        parser.positional(positional);

        boost::program_options::store(parser.run(), vm);
        boost::program_options::notify(vm);
    } catch (std::exception &e) {
        std::cerr << e.what()
            << ". See -h for usage." << std::endl;
        exit(-1);
    }
}

bool Opts::HasOption(std::string str)
{
    return vm.count(str);
}

}
