#include "libs/math/random.h"
#include <random>
#include <functional>
#include <limits>
#include <chrono>
#include <iostream>

#include "object/jsnumber.h"

namespace grok {
namespace libs {

class Random {
public:
    static double Gen()
    {
        return ud(engine);
    }

    static std::uniform_real_distribution<double> ud;
    static std::random_device rd;
    static std::mt19937 engine;
};

std::random_device Random::rd;
std::mt19937 Random::engine(Random::rd());
std::uniform_real_distribution<double> Random::ud(0.0, 1.0);

using namespace grok::obj;

std::shared_ptr<Handle> MathRandom(std::shared_ptr<Argument> args)
{
    return CreateJSNumber(Random::Gen());
}

} 
}
