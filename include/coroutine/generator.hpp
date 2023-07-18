#ifndef CORO_GENERATOR_HPP__
#define CORO_GENERATOR_HPP__

#include <coroutine>
#include <utility>
#include <future>

namespace thp {

template<std::movable T>
struct generator {
    // promise type
    struct promise_type {
        using value_type = T;

        constexpr inline generator<T> get_return_object() {
            auto h = std::coroutine_handle<promise_type>::from_promise(*this);
            return generator<T>{h};
        }

        constexpr std::suspend_always initial_suspend() noexcept { return {}; }
        constexpr std::suspend_always final_suspend() noexcept   { return {}; }

        constexpr void unhandled_exception() { throw; }

        template<std::convertible_to<T> From>
        constexpr std::suspend_always yield_value(From&& val) {
            result_ = std::forward<decltype(val)>(val);
            return {};
        }

        constexpr void return_void() {} // return_value not needed in generator

        constexpr value_type get_value() {
            return std::move(result_);
        }
        //constexpr void await_transform() = delete;
        private:
        value_type result_;
    };

    using handle_t = std::coroutine_handle<promise_type>;
    using promise_type = promise_type;
    using value_type = T;

    constexpr explicit generator(handle_t h) : hndl_{h} {}

    generator(const generator& t) = delete;
    generator& operator = (const generator&) = delete;

    constexpr generator(generator&& rhs) noexcept
    : hndl_{std::move(rhs.hndl_)} {
        rhs.hndl_ = {}; 
    }
    constexpr generator& operator=(generator&& rhs) noexcept {
        if (this != std::addressof(rhs)) {
            if (hndl_) hndl_.destroy();
            hndl_ = std::move(rhs.hndl_);
        }
        return *this;
    }

    constexpr ~generator() noexcept { if (hndl_) hndl_.destroy(); }
 
    // Range-based for loop support.
    struct iterator {
        // these type defs are important ?
        using value_type = typename promise_type::value_type;
        using difference_type =  std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        constexpr iterator(const handle_t coroutine)
        : coro_{coroutine}
        {}
 
        constexpr iterator& operator++() { 
            if (coro_ && !coro_.done())
              coro_.resume();
            return *this;
        }
        constexpr iterator& operator++(int) { 
            if (coro_ && !coro_.done())
              coro_.resume();
            return *this;
        }

        constexpr value_type operator * () const { 
            return coro_.promise().get_value();
        }        
        constexpr value_type operator -> () const { 
            return coro_.promise().get_value();
        }        

        constexpr bool operator==(const std::default_sentinel_t&) const { // TODO find out why only const & works 
            return !coro_ || coro_.done(); 
        }

    private:
        handle_t coro_;
    };
 
    constexpr iterator begin() {
        auto it = iterator{hndl_};
        return ++it;
    }
    constexpr std::default_sentinel_t end() { return {}; }

private:
    handle_t hndl_;
};

} // namespace thp

#endif // CORO_GENERATOR_HPP__
