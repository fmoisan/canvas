#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace canvas
{
    class scheduler
    {
        class task_queue;
    public:
        using task_type = std::function<void()>;

        scheduler();
        explicit scheduler(std::size_t worker_count);

        ~scheduler();

        auto worker_count() const -> std::size_t;

        void schedule_task(task_type task);

    private:
        void create_workers(std::size_t worker_count);
        void run_tasks();

    private:
        std::unique_ptr<task_queue> m_tasks;
        std::vector<std::thread> m_workers;
    };
}
