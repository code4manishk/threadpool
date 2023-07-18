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

#ifndef TASK_QUEUE_HPP_
#define TASK_QUEUE_HPP_

#include "include/work_queue.hpp"
#include "include/concepts.hpp"
#include "include/work_queue.hpp"

namespace thp {
namespace rng = std::ranges;

// one executable container queue
struct task_queue {
  constexpr virtual std::size_t size() const noexcept = 0;
  constexpr virtual bool empty() const noexcept = 0;
  constexpr virtual void accept(managed_thread& ) noexcept = 0;
};

// priority task queue
template<typename PriorityType>
struct priority_taskq : task_queue
{
  using TaskType = priority_task<PriorityType>;
  using Comp = std::less<TaskType>;

  void accept(managed_thread& ) noexcept {
    while(auto t = wq_.pop()) {
        t.value().execute();
    }
  }

  constexpr priority_taskq& push(TaskType x) {
    wq_.push(std::move(x));
    return *this;
  }

  template<std::input_iterator I, std::sentinel_for<I> S>
    requires std::same_as<TaskType, std::iter_value_t<I>>
  constexpr priority_taskq& insert(I s, S e) {
    wq_.insert(s, e);
    return *this;
  }

  template<std::ranges::input_range R>
    requires std::movable<R> && std::is_same_v<rng::range_value_t<R>, TaskType>
  constexpr priority_taskq& insert(R&& v) {
    wq_.insert(std::move(v));
    return *this;
  }

  constexpr inline bool empty() const noexcept override { return wq_.empty(); }
  constexpr inline size_t size() const noexcept override { return wq_.size(); }

protected:
  ds::priority_workq<TaskType, Comp> wq_;
};

} // namespace thp

#endif // TASK_QUEUE_HPP_
