#include <catch.hpp>

#include <canvas/scheduling/scheduler.h>

#include <thread>

TEST_CASE("Scheduler - Spawns with correct thread count")
{
    canvas::scheduler scheduler;

    CHECK(std::thread::hardware_concurrency() == scheduler.get_worker_count());
}
