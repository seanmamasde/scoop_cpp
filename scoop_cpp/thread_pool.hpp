#pragma once

#include <coroutine>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>

struct task_promise;

class [[nodiscard]] task
{
public:
    using promise_type = task_promise;

    explicit task(const std::coroutine_handle<task_promise> handle)
        : handle_(handle)
    {
    }

private:
    std::coroutine_handle<task_promise> handle_;
};

struct task_promise
{
    task get_return_object() noexcept
    {
        return task{ std::coroutine_handle<task_promise>::from_promise(*this) };
    }

    [[nodiscard]] static std::suspend_never initial_suspend() noexcept { return {}; }
    [[nodiscard]] static std::suspend_never final_suspend() noexcept { return {}; }

    static void return_void() noexcept {}

    [[noreturn]] static void unhandled_exception() noexcept
    {
        std::cerr << "Unhandled exception caught...\n";
        std::terminate();
    }
};

class thread_pool
{
public:
    explicit thread_pool(const std::size_t thread_count = std::thread::hardware_concurrency())
    {
        for (std::size_t i = 0; i < thread_count; ++i)
        {
            std::thread worker_thread([this]() { this->thread_loop(); });
            threads_.push_back(std::move(worker_thread));
        }
    }

    ~thread_pool() { shutdown(); }

    // disable copy
    thread_pool(const thread_pool&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;

    // enable move
    thread_pool(thread_pool&&) noexcept = default;
    thread_pool& operator=(thread_pool&&) noexcept = default;


    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
    {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            [f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable
            {
                return std::invoke(std::move(f), std::move(args)...);
            });

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (stop_thread_)
            {
                throw std::runtime_error("enqueue on stopped thread_pool");
            }

            task_queue_.emplace([task]() { (*task)(); });
        }
        cond_.notify_one();
        return res;
    }

    auto schedule()
    {
        struct awaiter
        {
            thread_pool* tp;

            static constexpr bool await_ready() noexcept { return false; }
            static constexpr void await_resume() noexcept {}
            void await_suspend(std::coroutine_handle<> coro) const noexcept
            {
                tp->enqueue_task(coro);
            }
        };
        return awaiter{ this };
    }

private:
    std::list<std::thread> threads_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<std::function<void()>> task_queue_;

    bool stop_thread_ = false;

    void thread_loop()
    {
        while (!stop_thread_)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [this] { return stop_thread_ || !task_queue_.empty(); });

            if (stop_thread_ && task_queue_.empty())
            {
                break;
            }

            auto task = std::move(task_queue_.front());
            task_queue_.pop();
            lock.unlock();
            task();
        }
    }

    void enqueue_task(std::coroutine_handle<> coro) noexcept
    {
        std::unique_lock<std::mutex> lock(mutex_);
        task_queue_.emplace([coro]() { coro.resume(); });
        cond_.notify_one();
    }

    void shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_thread_ = true;
        }
        cond_.notify_all();
        for (auto& thread : threads_)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }
};
