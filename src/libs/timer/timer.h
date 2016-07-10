#ifndef TIMER_H_
#define TIMER_H_

#include <random>
#include <chrono>
#include <boost/asio.hpp>
#include "object/jsbasicobject.h"
#include "object/argument.h"
#include "grok/context.h"   // for io_service object

namespace grok {
namespace libs {

typedef std::chrono::microseconds MicroSeconds;
typedef std::chrono::milliseconds MilliSeconds;
typedef std::chrono::nanoseconds NanoSeconds;
typedef std::chrono::high_resolution_clock HighResolutionClock;
typedef std::chrono::system_clock SystemClock;
typedef std::chrono::hours Hours;
typedef std::chrono::seconds Seconds;

// template <class TimeUnit>
// class Timer {
// public:


// private:
//     int32_t flags_;
//     TimeUnit 
// };

class TimeoutHelper {
public:
    using TimeoutID = unsigned int;
    static std::shared_ptr<grok::obj::Handle>
    SetTimeout(std::shared_ptr<grok::obj::Argument> args);

    static std::map<TimeoutID, boost::asio::deadline_timer*> running_timers;
    static TimeoutID counter;
};
}
}

#endif
