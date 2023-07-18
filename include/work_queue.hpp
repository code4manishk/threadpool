/* Copyright 2021 Threadpool Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef WORK_QUEUE_HPP_
#define WORK_QUEUE_HPP_

#include <deque>
#include <memory>
#include <mutex>
#include <ranges>
#include <shared_mutex>

#include "include/task_type.hpp"
#include "include/traits.hpp"
#include "platform/spinlock.hpp"

namespace thp {
namespace rng = std::ranges;

namespace ds {

// priority task queue
template <typename T, typename TaskComp>
struct priority_workq {
  using Prio = typename T::PriorityType;

  constexpr priority_workq() = default;

  constexpr priority_workq(std::deque<T> &&r) {
    std::unique_lock l(mu_);
    tasks_ = std::move(r);
  }

  constexpr priority_workq(const priority_workq &) = delete;
  constexpr priority_workq &operator=(const priority_workq &) = delete;

  constexpr priority_workq(priority_workq &&rhs) noexcept {
    std::unique_lock l(mu_, std::defer_lock), r(rhs.mu_, std::defer_lock);
    std::lock(l, r);
    std::swap(tasks_, rhs.tasks_);
  }
  constexpr priority_workq &operator=(priority_workq &&rhs) noexcept {
    std::scoped_lock l(mu_, rhs.mu_);
    if (this != std::addressof(rhs))
      tasks_ = std::move(rhs.tasks_);
    return *this;
  }

  constexpr auto pop() noexcept -> std::optional<T> {
    std::unique_lock l(mu_);
    std::optional<T> t;
    if (!tasks_.empty()) {
      if constexpr (std::is_same_v<void, Prio>) {
        t = std::move(tasks_.front());
        tasks_.pop_front();
      } else {
        rng::pop_heap(tasks_, TaskComp{});
        t = std::move(tasks_.back());
        tasks_.pop_back();
      }
    }
    return t;
  }

  constexpr priority_workq &push(T x) {
    std::unique_lock l(mu_);
    {
      tasks_.emplace_back(std::move(x));
      if constexpr (!std::is_same_v<void, Prio>) {
        rng::push_heap(tasks_, TaskComp{});
      }
    }
    return *this;
  }

  template <std::input_iterator I, std::sentinel_for<I> S>
    requires std::same_as<T, std::iter_value_t<I>>
  constexpr priority_workq &insert(I s, S e) {
    std::unique_lock l(mu_);
    tasks_.insert(rng::end(tasks_), s, e);
    if constexpr (!std::is_same_v<void, Prio>) {
      rng::make_heap(tasks_, TaskComp{});
    }
    return *this;
  }

  T &operator[](size_t idx) noexcept {
    std::unique_lock l(mu_);
    return tasks_[idx];
  }

  const T &operator[](size_t idx) const noexcept {
    std::shared_lock l(mu_);
    return tasks_[idx];
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    std::shared_lock l(mu_);
    return tasks_.empty();
  }

  [[nodiscard]] constexpr size_t size() const noexcept {
    std::shared_lock l(mu_);
    return tasks_.size();
  }

  constexpr ~priority_workq() noexcept {
    std::unique_lock l(mu_);
    tasks_.clear();
  }

protected:
  alignas(hardware_destructive_interference_size) mutable platform::spin_shared_mutex mu_;
  std::deque<T> tasks_;
};

} // namespace ds
} // namespace thp

#endif // WORK_QUEUE_HPP_
