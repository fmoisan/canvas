#include <catch.hpp>

#include "canvas/scheduling/scheduler.hpp"

#include <atomic>
#include <thread>

namespace details
{

    template <typename predicate_t>
    void wait_until(predicate_t predicate)
    {
        while (!predicate())
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
    }
}

TEST_CASE("scheduler - has [core count] amount of workers by default")
{
    canvas::scheduler scheduler;

    CHECK(scheduler.worker_count() == std::thread::hardware_concurrency());
}

TEST_CASE("scheduler - can be constructed with custom worker count")
{
    canvas::scheduler scheduler(2);

    CHECK(scheduler.worker_count() == 2);
}

TEST_CASE("scheduler - add_task schedules tasks immediately")
{
    std::atomic<bool> called;

    canvas::scheduler scheduler;

    scheduler.add_task([&]
    {
        called = true;
    });

    details::wait_until([&]
    { return called.load(); });

    CHECK(called);
}

TEST_CASE("scheduler - multiple tasks are all ran to completion")
{
    const int task_count = 100;
    std::atomic<int> completed_count(0);

    canvas::scheduler scheduler;

    for (int i = 0; i < task_count; ++i)
    {
        scheduler.add_task([&]
        {
            ++completed_count;
        });
    }

    details::wait_until([&]
    { return completed_count == task_count; });

    CHECK(completed_count == task_count);
}

TEST_CASE("scheduler - idle_worker_count gets the number of idle threads")
{
    canvas::scheduler scheduler;
    CHECK(scheduler.idle_worker_count() == scheduler.worker_count());

    std::atomic<bool> task_blocked(false);

    scheduler.add_task([&]
    {
        task_blocked = true;

        details::wait_until([&]
        { return !task_blocked; });
    });

    details::wait_until([&]
    { return task_blocked.load(); });

    CHECK(scheduler.idle_worker_count() == scheduler.worker_count() - 1);

    task_blocked = false;
}

TEST_CASE("scheduler - all workers are idle after joining")
{
    canvas::scheduler scheduler;

    std::atomic<bool> task_blocked{true};

    for (std::size_t i = 0; i < scheduler.worker_count(); ++i)
    {
        scheduler.add_task([&]
        {
            details::wait_until([&]
            { return !task_blocked; });
        });
    }

    details::wait_until([&]
    { return scheduler.idle_worker_count() == 0; });

    task_blocked = false;

    scheduler.join();

    CHECK(scheduler.idle_worker_count() == scheduler.worker_count());
}
