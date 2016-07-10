#ifndef RANDOM_H_
#define RANDOM_H_

#include "object/argument.h"

namespace grok {
namespace libs {

extern std::shared_ptr<grok::obj::Handle>
MathRandom(std::shared_ptr<grok::obj::Argument> args);

}
}

#endif
