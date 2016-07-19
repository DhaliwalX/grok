#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <boost/pool/pool_alloc.hpp>

namespace grok {

template <typename T>
using FastPoolAllocator = boost::fast_pool_allocator<T>;

}

#endif
