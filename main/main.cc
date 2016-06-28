#include "grok/runner.h"
#include "grok/context.h"

using namespace grok;

int main(int argc, char **argv)
{
    InitializeContext();
    ParseCommandLineOptions(argc, argv);
    return Start();
}
