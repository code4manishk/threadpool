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

#ifndef __EQUALSIZE_PARTALGO__
#define __EQUALSIZE_PARTALGO__

#include <iterator>
#include <algorithm>

#include "include/algos/partitioner/partition_algo.hpp"

namespace thp {
namespace algos {
namespace partitioner {

template <std::forward_iterator I, std::sentinel_for<I> S>
class equal_size : public partition_algo<I,S> {
public:
  typedef struct state {
    I start;
    S end;
    std::size_t step;
    constexpr bool operator == (const state& rhs) const {
      return (step == rhs.step);
    }
  } state_t;

  constexpr explicit equal_size(std::iter_difference_t<I> max_part_size, I s, S e)
  : partition_algo<I,S>{}
  , original{s, e, 0u}
  , partition_size{max_part_size}
  {
    const auto len = std::ranges::distance(s, e);
    partition_count = len/max_part_size;
    if (len % max_part_size != 0u) ++partition_count;
    partition_count = std::max({partition_count, 1lu});
  }

  constexpr std::size_t count() const override { return partition_count; }

  constexpr state_t next_step(const state_t& prev) {
    if (prev.step >= partition_count)
      return {original.end, original.end, partition_count+1};
    else if (prev.step+1 == partition_count)
      return {prev.end, original.end, prev.step+1};
    else
      return {prev.end, std::ranges::next(prev.end, partition_size), prev.step+1};
  }

  constexpr state_t begin() {
    return next_step(state_t{original.start, original.start, 0u});
  }

  constexpr state_t end() {
    return {original.end, original.end, 1+partition_count};
  }

  protected:
  state_t original;
  std::iter_difference_t<I> partition_size;
  std::size_t partition_count;
};

} // namespace partitioner
} // namespace algos
} // namespace thp

#endif // __EQUALSIZ_PARTALGO__
