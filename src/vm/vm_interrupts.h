#ifndef VM_INTERRUPTS_H_
#define VM_INTERRUPTS_H_

#include "common/queue.h"

#include "vm/context.h"
#include "vm/counter.h"

#include <vector>
#include <utility>
namespace grok {
namespace vm {

// queue for all asynchronous tasks
using CounterPair = std::pair<Counter, Counter>;
using IRQueue = Queue<CounterPair, std::vector<CounterPair>>;

}
}

#endif // vm_interrupts.h
