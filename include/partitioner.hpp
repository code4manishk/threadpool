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

#ifndef __PARTITIONER_HPP__
#define __PARTITIONER_HPP__

#include <iterator>
#include <algorithm>
#include <ranges>

#include "include/algos/partitioner/partition_algo.hpp"

namespace thp {

// partitioner : is a generator of views over a range, like split_view
// algo: is a strategy to generate partition views
template <typename PartitionAlgoType>
class partitioner {
  PartitionAlgoType algo;

  class partition_iterator {
    PartitionAlgoType& m_algo;
    typename PartitionAlgoType::state_t m_state;

    public:
    constexpr explicit partition_iterator(PartitionAlgoType& algo, typename PartitionAlgoType::state_t state)
    : m_algo{algo}
    , m_state{state}
    {}

    constexpr partition_iterator& operator ++ (int) {
      m_state = m_algo.next_step(m_state);
      return *this;
    }
    constexpr partition_iterator& operator ++ () {
      m_state = m_algo.next_step(m_state);
      return *this;
    }

    constexpr auto operator * () { return std::ranges::subrange(m_state.start, m_state.end); }

    friend constexpr bool operator == (const partition_iterator& a, const partition_iterator& b) {
      return a.m_state == b.m_state;
    }
  };

  public:
  constexpr explicit partitioner(PartitionAlgoType part_algo)
  : algo{std::move(part_algo)}
  {}
  constexpr auto begin() { return partition_iterator(algo, algo.begin()); }
  constexpr auto end()   { return partition_iterator(algo, algo.end()); }
  constexpr std::size_t count() const { return algo.count(); }
};

} // namespace thp
#endif // __PARTITIONER_HPP__
