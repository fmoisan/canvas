#include "canvas/scheduling/scheduler.hpp"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <thread>

namespace canvas
{
    class scheduler::task_queue
    {
    public:
        auto cancelled() const -> bool { return m_cancelled; }

        void cancel()
        {
            m_cancelled = true;
            m_available.notify_all();
        }

        void add_task(task_type task)
        {
            {
                std::unique_lock<std::mutex> lock{m_mutex};
                m_tasks.emplace_back(task);
            }

            m_available.notify_one();
        }

        auto wait_for_next_task() -> task_type
        {
            std::unique_lock<std::mutex> lock{m_mutex};
            m_available.wait(lock, [&] { return !m_tasks.empty() || m_cancelled; });

            if (!m_cancelled) {
                auto task = std::move(m_tasks.front());
                m_tasks.pop_front();
                return task;
            }

            return task_type{};
        }

    private:
        std::mutex m_mutex;
        std::condition_variable m_available;

        std::atomic<bool> m_cancelled{false};

        std::deque<task_type> m_tasks;
    };

    class scheduler::active_worker_scope
    {
    public:
        active_worker_scope(scheduler* scheduler)
            : m_scheduler{scheduler}
        {
            std::unique_lock<std::mutex> lock{m_scheduler->m_task_completed_mutex};
            ++m_scheduler->m_active_worker_count;
        }

        ~active_worker_scope()
        {
            {
                std::unique_lock<std::mutex> lock{m_scheduler->m_task_completed_mutex};
                --m_scheduler->m_active_worker_count;
            }

            m_scheduler->m_task_completed_condition.notify_one();
        }

    private:
        scheduler* m_scheduler;
    };

    scheduler::scheduler()
        : m_tasks{std::make_unique<task_queue>()}
        , m_active_worker_count{0}
    {
        create_workers(std::thread::hardware_concurrency());
    }

    scheduler::scheduler(std::size_t worker_count)
        : m_tasks{std::make_unique<task_queue>()}
        , m_active_worker_count{0}
    {
        create_workers(worker_count);
    }

    scheduler::~scheduler()
    {
        m_tasks->cancel();

        for (auto& worker : m_workers) {
            worker.join();
        }
    }

    auto scheduler::worker_count() const -> std::size_t
    {
        return m_workers.size();
    }

    auto scheduler::idle_worker_count() const -> std::size_t
    {
        return worker_count() - m_active_worker_count;
    }

    void scheduler::add_task(task_type task)
    {
        m_tasks->add_task(task);
    }

    void scheduler::join()
    {
        std::unique_lock<std::mutex> lock{m_task_completed_mutex};

        m_task_completed_condition.wait(lock, [this] {
            return m_active_worker_count == 0;
        });
    }

    void scheduler::create_workers(std::size_t worker_count)
    {
        m_workers.reserve(worker_count);

        for (std::size_t i = 0; i < worker_count; ++i) {
            m_workers.emplace_back(&scheduler::run_tasks, this);
        }
    }

    void scheduler::run_tasks()
    {
        while (!m_tasks->cancelled()) {
            if (auto task = m_tasks->wait_for_next_task()) {
                active_worker_scope scope{this};
                task();
            }
        }
    }
}
