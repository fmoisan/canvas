#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

namespace std
{
    class thread;
}

namespace canvas
{
    class scheduler
    {
    public:
        using task_type = std::function<void()>;

        scheduler();
        explicit scheduler(std::size_t worker_count);

        ~scheduler();

        std::size_t worker_count() const;
        std::size_t idle_worker_count() const;

        void add_task(task_type task);

    private:
        void setup_workers(std::size_t worker_count);

    private:
        class task_queue;
        void run_tasks();

    private:
        std::unique_ptr<task_queue> m_tasks;
        std::vector<std::thread> m_workers;

        std::atomic<std::size_t> m_activeWorkers;
    };
}
