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

#ifndef JOB_QUEUE_HPP_
#define JOB_QUEUE_HPP_

#include <variant>
#include <numeric>
#include <vector>

#include "include/concepts.hpp"
#include "include/traits.hpp"
#include "include/task_queue.hpp"
#include "include/task_type.hpp"
#include "include/statistics.hpp"
#include "include/util.hpp"
#include "include/all_priority_types.hpp"
#include "include/managed_stop_token.hpp"
#include "include/algos/queue_selection/custom.hpp"
#include "include/algos/queue_selection/maxlen.hpp"

namespace thp {
namespace rng = std::ranges;
namespace vw = std::ranges::views;

struct jobq_stopped_ex final : std::exception {
  const char* what() const noexcept override { return "jobq_stopped_ex"; }
};

struct jobq_closed_ex final : std::exception {
  const char* what() const noexcept override { return "jobq_closed_ex"; }
};

// job queue manages several task queues
template <typename TaskQueueTupleType>
struct job_queue {
  static const auto NumQs = std::tuple_size_v<TaskQueueTupleType>;

  constexpr explicit job_queue()
  : task_qs_{create_taskqs_tuple(std::make_index_sequence<NumQs>{})}
  , all_qs_{}
  , algo_{std::make_unique<algos::queue_select::custom_algo>()}
  , bestq_{nullptr}
  {
    create_taskqs_array(task_qs_, std::make_index_sequence<NumQs>{});
  }

  constexpr job_queue<TaskQueueTupleType>& schedule_task(kncpt::ThreadPoolTask auto&& t) {
    using ThisTaskType = traits::FindTaskType<decltype(t)>::type;

    taskq_for<ThisTaskType>().push(FWD(t));
    return *this;
  }

  constexpr void close() {}
  constexpr void stop() {}

  // returns best queue at the point of request or blocks if empty
  constexpr task_queue* best_queue(statistics& stats) noexcept {
    return algo_->apply(stats);
  }

  constexpr void init_stats(statistics& stats) {
    stats.jobq.in.qs = all_qs_;
  }

protected:
  template<typename Tuple, std::size_t... I>
  constexpr void create_taskqs_array(Tuple&& tup, std::index_sequence<I...>)
  {
    ((all_qs_.push_back(std::addressof(std::get<I>(FWD(tup))))), ...);
  }

  template<std::size_t...I>
  constexpr TaskQueueTupleType create_taskqs_tuple(std::index_sequence<I...>) {
    return std::make_tuple(std::tuple_element_t<I, TaskQueueTupleType>()...);
  }

  template <typename TaskType,
            typename QueueType = priority_taskq<typename TaskType::PriorityType>
  >
  constexpr inline auto& taskq_for(void) {
    return std::get<QueueType>(task_qs_);
  }

private:
  // task queues for different task types
  TaskQueueTupleType task_qs_;
  std::vector<task_queue*> all_qs_;
  std::unique_ptr<algos::queue_select::queue_selection_algo> algo_;
  alignas(hardware_destructive_interference_size) std::atomic<task_queue*> bestq_;
};

} // namespace thp

#endif // JOBQ_HPP_
