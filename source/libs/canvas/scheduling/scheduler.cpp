#include <canvas/scheduling/scheduler.h>

#include <thread>

namespace canvas
{

    std::size_t scheduler::get_worker_count() const
    {
        return std::thread::hardware_concurrency();
    }

}
