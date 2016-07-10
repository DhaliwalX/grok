#include "libs/timer/timer.h"
#include "object/function.h"
#include "vm/context.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#include <sys/types.h>

namespace grok {
namespace libs {

std::map<TimeoutHelper::TimeoutID, boost::asio::deadline_timer*> 
TimeoutHelper::running_timers;
TimeoutHelper::TimeoutID TimeoutHelper::counter = 0;

using namespace grok::obj;
using namespace grok::vm;
static int a = 0;

void TimeoutCallback(std::shared_ptr<Function> fn,
    std::shared_ptr<Argument> args, TimeoutHelper::TimeoutID)
{
    std::cout << "Error occurred" << std::endl;
    auto vm = GetGlobalVMContext()->GetVM();
    if (vm == nullptr)
        std::cout << "vm was nullptr";
    CallJSFunction(fn, args, vm);
}

using namespace boost::posix_time;

std::shared_ptr<Handle>
TimeoutHelper::SetTimeout(std::shared_ptr<Argument> args)
{
    grok::Context *ctx = grok::GetContext();
    boost::asio::io_service *io = ctx->GetIOService();

    microseconds us = microseconds(0);
    auto ms_handle = args->GetProperty("time");
    auto fn_handle = args->GetProperty("fn");

    if (!IsFunction(fn_handle)) {
        throw std::runtime_error("setTimeout: not a function");
    }
    auto fn = fn_handle->as<Function>();

    std::shared_ptr<Object> num = TryForNumber(ms_handle);
    us = boost::posix_time::microseconds(
                (long int)(num->as<JSDouble>()->GetNumber() * 1000));
    auto args_to_pass = std::make_shared<Argument>();

    auto timer = new boost::asio::deadline_timer(*io, us);

    timer->async_wait([fn, args_to_pass]
        (const boost::system::error_code & err) {
        a++;
        if (err) {
            throw std::runtime_error("fatal: timer ran into problem");
            return;
        }

        auto vm = grok::vm::CreateVM(GetGlobalVMContext());
        if (vm == nullptr)
            std::cout << "vm was nullptr";
        CallJSFunction(fn, args_to_pass, vm.get());        
    });

    running_timers.insert({ counter++, timer});
    // TimeoutCallback(fn, args_to_pass, counter);
    return CreateJSNumber(counter);
}

}
}

