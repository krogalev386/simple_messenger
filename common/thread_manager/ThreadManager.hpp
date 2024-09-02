#pragma once

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>

#include "CircularBuffer.hpp"
#include "CrtpSingleton.hpp"

class ThreadManager : public CrtpSingleton<ThreadManager> {
    friend class CrtpSingleton<ThreadManager>;

   protected:
    ThreadManager();
    ~ThreadManager();

   public:
    void schedule_task(const std::function<void()>& task);

   private:
    static constexpr uint16_t thread_pool_size = 4;
    static constexpr uint16_t task_queue_size  = 4;

    std::thread             thread_pool[thread_pool_size];
    std::mutex              task_queue_mtx;
    std::condition_variable task_queue_cv;

    bool is_running;

    CircularBuffer<std::function<void()>, task_queue_size> task_queue;
};
