#include <catch.hpp>

#include <canvas/scheduling/scheduler.hpp>

#include <atomic>
#include <condition_variable>
#include <thread>

namespace details
{

    template < typename predicate_t >
    void wait_until(predicate_t predicate)
    {
        while (!predicate())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }

}

TEST_CASE("scheduler - has [core count] amount of workers by default")
{
    canvas::scheduler scheduler;

    CHECK(scheduler.get_worker_count() == std::thread::hardware_concurrency());
}

TEST_CASE("scheduler - can be constructed with custom worker count")
{
    canvas::scheduler scheduler(2);

    CHECK(scheduler.get_worker_count() == 2);
}

TEST_CASE("scheduler - add_task schedules tasks immediately")
{
    std::atomic<bool> called;

    canvas::scheduler scheduler;

    scheduler.add_task([&]
    {
        called = true;
    });

    details::wait_until([&] { return called.load(); });

    CHECK(called);
}

TEST_CASE("scheduler - multiple tasks are all ran to completion")
{
    const int task_count = 100;
    std::atomic<int> completed_count;

    canvas::scheduler scheduler;

    for (int i = 0; i < task_count; ++i)
    {
        scheduler.add_task([&]
        {
            ++completed_count;
        });
    }

    details::wait_until([&] { return completed_count == task_count; });

    CHECK(completed_count == task_count);
}