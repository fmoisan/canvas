#pragma once

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

        std::size_t get_worker_count() const;

        void add_task(task_type task);

    private:
        void setup_workers(std::size_t worker_count);

    private:
        class task_queue;
        static void run_tasks(std::shared_ptr<task_queue> tasks);

    private:
        std::shared_ptr<task_queue> m_tasks;

        std::vector<std::thread> m_pool;
    };
}
