#include <canvas/scheduling/scheduler.hpp>

#include <atomic>
#include <condition_variable>
#include <thread>

namespace canvas
{
    class scheduler::task_queue
    {
    public:
        bool cancelled() const
        {
            return m_cancelled;
        }

        void cancel()
        {
            m_cancelled = true;
            m_available.notify_all();
        }

        void add_task(scheduler::task_type task)
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_tasks.emplace_back(task);
            }

            m_available.notify_one();
        }

        task_type wait_for_next_task()
        {
            task_type task{};

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_available.wait(lock, [&]
                { return !m_tasks.empty() || m_cancelled; });

                if (!m_tasks.empty() && !m_cancelled)
                {
                    auto it = m_tasks.begin();
                    task = *it;
                    m_tasks.erase(it);
                }
            }

            return task;
        }

    private:
        std::mutex m_mutex;
        std::condition_variable m_available;

        std::atomic<bool> m_cancelled{ false };

        std::vector<scheduler::task_type> m_tasks;
    };

    scheduler::scheduler()
        : m_tasks(std::make_shared<task_queue>())
    {
        setup_workers(std::thread::hardware_concurrency());
    }

    scheduler::scheduler(std::size_t worker_count)
        : m_tasks(std::make_shared<task_queue>())
    {
        setup_workers(worker_count);
    }

    scheduler::~scheduler()
    {
        m_tasks->cancel();

        for (auto& worker : m_workers)
        {
            worker.join();
        }
    }

    std::size_t scheduler::get_worker_count() const
    {
        return m_workers.size();
    }

    void scheduler::add_task(task_type task)
    {
        m_tasks->add_task(task);
    }

    void scheduler::setup_workers(std::size_t worker_count)
    {
        m_workers.reserve(worker_count);

        for (std::size_t i = 0; i < worker_count; ++i)
        {
            m_workers.emplace_back(run_tasks, m_tasks);
        }
    }

    void scheduler::run_tasks(std::shared_ptr<task_queue> tasks)
    {
        while (!tasks->cancelled())
        {
            if (auto task = tasks->wait_for_next_task())
            {
                task();
            }
        }
    }
}
