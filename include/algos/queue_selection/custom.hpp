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

#ifndef FAIRSHARE_HPP_
#define FAIRSHARE_HPP_

#include "include/algos/queue_selection/queue_selection_algo.hpp"
#include "include/statistics.hpp"

namespace thp {
namespace algos {
namespace queue_select {

struct custom_algo : queue_selection_algo {
  task_queue* curq_ = {nullptr};
  constexpr task_queue* apply(const statistics& stats) noexcept override {
    const auto& inputs = stats.jobq.in.qs;
    if (!curq_) {
      curq_ = inputs[0];
    } else if (curq_->empty()) {
      // choose maxlen
      curq_ = *std::ranges::max_element(inputs, {}, &task_queue::size);
    }
    return curq_->empty() ? nullptr : curq_; 
  }

};

} // namespace queue_selection
} // namespace sched_algos
} // namespace thp

#endif // FAIRSHARE_HPP_
