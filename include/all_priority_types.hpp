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

#ifndef ALL_PRIORITY_TYPES_HPP__
#define ALL_PRIORITY_TYPES_HPP__

#include <chrono>
#include <tuple>

#include "include/task_type.hpp"
#include "include/task_queue.hpp"

namespace thp {

using AllPriority = std::tuple<void, int>;

using AllPriority2 = std::tuple<
    void,
    int,
    float,
    std::chrono::steady_clock::time_point,
    std::chrono::system_clock::time_point
>;

// compile time oneshot queue will require all 
// task types listed, not convenient, so abandon this approach
using AllOneshotTasks = std::tuple<
  simple_task
>;

template<typename T>
struct PriorityTaskQueueTuple;

template<typename...Ts>
struct PriorityTaskQueueTuple<std::tuple<Ts...>> {
  using type = std::tuple<priority_taskq<Ts>...>;
};

using PriorityTaskQueueTupleType = PriorityTaskQueueTuple<AllPriority>::type;

template<typename T>
struct TaskQueueTuple;

template<typename... Tups>
struct TaskQueueTuple<std::tuple<Tups...>> {
  using type = std::invoke_result_t<decltype(&std::tuple_cat<Tups...>), Tups...>;
};

using TaskQueueTupleType = TaskQueueTuple<std::tuple<PriorityTaskQueueTupleType>>::type;
} // namespace thp

#endif // ALL_PRIORITY_TYPES_HPP__
