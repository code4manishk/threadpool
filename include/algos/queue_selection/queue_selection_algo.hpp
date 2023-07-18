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

#ifndef QUEUE_SELECTION_ALGOS_HPP__
#define QUEUE_SELECTION_ALGOS_HPP__

#include <limits>

#include "include/managed_thread.hpp"
#include "include/statistics.hpp"

namespace thp {
namespace algos {
namespace queue_select {

enum names : uint8_t {
  eFirstAvailable = 0,
  eMaxLen = 1,
  eFairShare = 2,

  eInvalid = std::numeric_limits<uint8_t>::max()
};

// algo interface
struct queue_selection_algo {
  constexpr virtual bool ok(statistics &) noexcept { return false; }
  constexpr virtual task_queue *apply(const statistics &) noexcept = 0;
};

} // namespace queue_select
} // namespace algos
} // namespace thp

#endif // QUEUE_SELECTION_ALGOS_HPP__
