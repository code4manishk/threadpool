#ifndef CORO_TASK_HPP__
#define CORO_TASK_HPP__

#include <coroutine>
#include <utility>
#include <iostream>
#include <future>

#include "include/util.hpp"

// TODO: introduce concepts for coroutine related function and promise data structure
namespace thp {
namespace coro {

struct continuation_final_awaiter;

template<typename PromiseType>
struct awaiter;

template<typename T, typename PromiseType, typename AwaiterType>
struct task;

// default promise type for coroutine requested interface
// contains customizable initial and final suspend functions as templates
template<std::movable T, typename InitialSuspend = std::suspend_always, typename FinalSuspend = std::suspend_always>
struct default_promise {
    using ResultType = T;
    using PromiseType = default_promise<T, InitialSuspend, FinalSuspend>;
    using AwaiterType = awaiter<PromiseType>;
    using TaskType = task<T, PromiseType, AwaiterType>;

    constexpr decltype(auto) get_return_object() {
        auto h = std::coroutine_handle<PromiseType>::from_promise(*this);
        return TaskType(h);
    }

    constexpr InitialSuspend initial_suspend()      { return {}; }
    constexpr FinalSuspend final_suspend() noexcept { return {}; }

    constexpr void unhandled_exception() { throw; }

    constexpr std::suspend_always return_value(T val) {
        result = std::move(val);
        return {};
    }

    constexpr std::suspend_always return_value(const T& val) const {
        result = val;
        return {};
    }
 
    ResultType result;
};

template<std::movable T, typename InitialSuspend = std::suspend_always, typename FinalSuspend = continuation_final_awaiter>
struct continuation_promise {
    using ResultType = T;
    using PromiseType = continuation_promise<ResultType, InitialSuspend, FinalSuspend>;
    using AwaiterType = awaiter<PromiseType>;
    using TaskType = task<ResultType, PromiseType, AwaiterType>;

    constexpr auto get_return_object() {
        auto h = std::coroutine_handle<PromiseType>::from_promise(*this);
        return TaskType(h);
    }
   constexpr InitialSuspend initial_suspend() {
        return {};
    }
    constexpr FinalSuspend final_suspend() noexcept { return {}; }

    constexpr void unhandled_exception() { throw; }

    template<std::convertible_to<ResultType> From>
    constexpr std::suspend_always return_value(From &&val) {
        result = std::move(FWD(val));
        return {};
    }

    ResultType result = {};
    std::coroutine_handle<> previous = std::noop_coroutine();
};

template<typename PromiseType>
struct awaiter {
    constexpr bool await_ready() noexcept  { return false; }
    constexpr typename PromiseType::ResultType await_resume() noexcept { return std::move(coro.promise().result); }
    constexpr std::coroutine_handle<> await_suspend(auto&& h) noexcept {
        if constexpr (requires {coro.promise().previous;}) {
            coro.promise().previous = h;
        }
        return coro;
    }

    std::coroutine_handle<PromiseType> coro;
};

struct continuation_final_awaiter {
    constexpr bool await_ready() noexcept  { return false; }
    constexpr void await_resume() noexcept {}

    template<typename PromiseType>
    constexpr std::coroutine_handle<> await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
        auto previous = h.promise().previous;
        if (previous)
            return previous;
        else
            return std::noop_coroutine();
    }
};

template<typename T, typename PromiseType = default_promise<T>, typename AwaiterType = awaiter<PromiseType>>
struct task {
    using promise_type = PromiseType;
    using handle = std::coroutine_handle<promise_type>;

    constexpr task(handle h) : coro(h) {}
    task(task&& t) = delete;
    ~task() { coro.destroy(); }
 
    AwaiterType operator co_await() { return {coro}; }

    T operator()() {
        coro.resume();
        return std::move(coro.promise().result);
    }

private:
    handle coro;
};

} // namespace coro
} // namespace thp

#endif // CORO_TASK_HPP__
