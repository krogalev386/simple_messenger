#include "ThreadManager.hpp"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>

#include "Logger.hpp"

ThreadManager::ThreadManager() : is_running(true) {
    auto task_queue_handling = [this] {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(task_queue_mtx);
                task_queue_cv.wait(lock, [this] {
                    return (!task_queue.is_empty() or !is_running);
                });

                if (!is_running and task_queue.is_empty()) {
                    return;
                }

                task = task_queue.pop().value();
            }
            task();
        }
    };

    for (auto& thread : thread_pool) {
        thread = std::thread(task_queue_handling);
        LOG("Thread %u is spawned", thread.get_id());
    };
};

ThreadManager::~ThreadManager() {
    {
        std::unique_lock<std::mutex> lock(task_queue_mtx);
        is_running = false;
    }
    task_queue_cv.notify_all();
    for (auto& thread : thread_pool) {
        LOG("Thread %u is terminated", thread.get_id());
        thread.join();
    }
};

void ThreadManager::schedule_task(const std::function<void()>& new_task) {
    {
        task_queue_mtx.lock();
        while (task_queue.is_full()) {
            // let the queue to drain
            task_queue_mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            task_queue_mtx.lock();
        }
        task_queue.push(new_task);
        task_queue_mtx.unlock();
    }
    task_queue_cv.notify_all();
    LOG("A task is scheduled");
};
