#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
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

        void join();

    private:
        void setup_workers(std::size_t worker_count);

    private:
        class active_worker_scope;
        class task_queue;

        void run_tasks();

    private:
        std::unique_ptr<task_queue> m_tasks;
        std::vector<std::thread> m_workers;

        std::mutex m_task_completed_mutex;
        std::condition_variable m_task_completed_condition;
        std::size_t m_active_worker_count;
    };
}
