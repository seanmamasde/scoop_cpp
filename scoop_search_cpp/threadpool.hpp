﻿#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>
#include <type_traits>

class threadpool
{
public:
    explicit threadpool(size_t = std::thread::hardware_concurrency());
    ~threadpool();

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
};

inline threadpool::threadpool(const size_t threads) : stop_(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers_.emplace_back([this]
            {
                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex_);
                        this->condition_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });
                        if (this->stop_ && this->tasks_.empty())
                            return;
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }

                    task();
                }
            });
}

template <class F, class... Args>
auto threadpool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
    using return_type = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock lock(queue_mutex_);

        if (stop_)
            throw std::runtime_error("enqueue on stopped thread_pool");

        tasks_.emplace([task] { (*task)(); });
    }
    condition_.notify_one();
    return res;
}

inline threadpool::~threadpool()
{
    {
        std::unique_lock lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    for (std::thread& worker : workers_)
        worker.join();
}
