#pragma once

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>

#include "CircularBuffer.hpp"
#include "DynamicCrtpSingleton.hpp"

class ThreadManager : public DynamicCrtpSingleton<ThreadManager> {
    friend class DynamicCrtpSingleton<ThreadManager>;

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
