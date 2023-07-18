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

#ifndef __PARTITION_ALGO_HPP__
#define __PARTITION_ALGO_HPP__

#include <iterator>

namespace thp {
namespace algos {
namespace partitioner {

template <std::input_iterator I, std::sentinel_for<I> S>
struct partition_algo {
  using iterator = I;
  using sentinel = S;

  typedef struct algo_state {
    I start;
    S end;
    constexpr bool operator == (const algo_state& rhs) const {
      return (start == rhs.start) && (end == rhs.end);
    }
  } state_t;

  constexpr state_t next_step(const state_t& prev) { return prev; }
  constexpr virtual std::size_t count() const = 0;
};

} // namespace partitioner
} // namespace algos
} // namespace thp

#endif // __PARTITION_ALGO_HPP__
