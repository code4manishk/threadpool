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

#ifndef FIRST_AVAILABLE_HPP_
#define FIRST_AVAILABLE_HPP_

#include <thread>

#include "include/algos/queue_selection/queue_selection_algo.hpp"
#include "include/statistics.hpp"
#include "include/task_queue.hpp"

namespace thp {
namespace algos {
namespace queue_select {
struct first_available : queue_selection_algo {
  task_queue* apply(const statistics& stats) noexcept override {
    const auto& inputs = stats.jobq.in.qs;
    auto it = std::ranges::find_if_not(inputs, &task_queue::empty);
    return (it == inputs.end()) ? nullptr : *it;
  }

  // int apply(statistics& stats, std::thread::id tid) override {
  //   return 0;
  // }
};

}
} // namespace sched_algos
} // namespace thp

#endif // FIRST_AVAILABLE_HPP_
