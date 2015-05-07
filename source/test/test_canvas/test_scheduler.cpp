#include <catch.hpp>

#include "canvas/scheduling/scheduler.hpp"

#include <atomic>
#include <cstdlib>
#include <thread>

namespace details
{
    template <typename predicate_t>
    void wait_until(predicate_t predicate)
    {
        int elapsed_milliseconds = 0;
        while (!predicate()) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            if (++elapsed_milliseconds > 1000000)
                std::exit(42);
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
    canvas::scheduler scheduler{2};

    CHECK(scheduler.worker_count() == 2);
}

TEST_CASE("scheduler - schedule_task schedules tasks immediately")
{
    std::atomic<bool> called{false};

    canvas::scheduler scheduler;

    scheduler.schedule_task([&] { called = true; });

    details::wait_until([&] { return called.load(); });

    CHECK(called);
}

TEST_CASE("scheduler - multiple tasks are all ran to completion")
{
    const int task_count = 100;
    std::atomic<int> completed_count{0};

    canvas::scheduler scheduler;

    for (int i = 0; i < task_count; ++i) {
        scheduler.schedule_task([&] { ++completed_count; });
    }

    details::wait_until([&] { return completed_count == task_count; });

    CHECK(completed_count == task_count);
}

TEST_CASE("scheduler - tasks can schedule other tasks")
{
    const int generating_task_count{100};
    const int sub_task_count{100};

    std::atomic<int> completed_count{0};

    canvas::scheduler scheduler;

    for (auto i = 0; i < generating_task_count; ++i) {
        scheduler.schedule_task([&] {
            for (auto i = 0; i < sub_task_count; ++i)
                scheduler.schedule_task([&completed_count] { ++completed_count; });
        });
    }

    auto const total_task_count = generating_task_count * sub_task_count;
    details::wait_until([&] { return completed_count == total_task_count; });

    CHECK(completed_count == total_task_count);
}
