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

#ifndef TP_STL_ALGO_HPP__
#define TP_STL_ALGO_HPP__

#include <algorithm>
#include <ranges>
#include <functional>
#include <execution>

#include "include/threadpool.hpp"
#include "include/partitioner.hpp"
#include "include/algos/partitioner/equal_size.hpp"

namespace thp {
namespace stl_algo {

struct api {
protected:
    threadpool& __impl_tp;

public:
    constexpr explicit api(threadpool& tp) : __impl_tp{tp} {}

    template <std::random_access_iterator I, std::sentinel_for<I> S,
            typename Comp = std::ranges::less,
            typename Proj = std::identity>
    requires std::sortable<I, Comp, Proj>
    constexpr decltype(auto) sort(I start, S end, Comp cmp = {}, Proj prj = {}) {
      using RangeData = std::ranges::subrange<I, S>;
      using Ret = std::tuple<I, S>;
      using tp_sort_func = std::function<Ret(RangeData, Comp, Proj)>;

      tp_sort_func tp_sort = [&](RangeData data, Comp cmp, Proj prj) {
        const auto len = std::ranges::size(data);
        if (len <= configs::stl_sort_cutoff()) {
          std::ranges::sort(data, cmp, prj);
        } else {
          // level merge of sorted ranges
          auto level_merge_func = [this](auto&& fut_vec) mutable
          {
            auto one_merge = [](auto&& f1, auto&& f2) mutable {
                auto [s1, e1] = f1.get();
                auto [s2, e2] = f2.get();
                //assert(e1 == s2);
                std::ranges::inplace_merge(s1, s2, e2);
                return std::make_tuple(s1, e2);
            };

            std::vector<std::future<Ret>> futs;
            const auto m = fut_vec.size()/2;
            for(unsigned i = 0; i < 2*m; i += 2) {
              auto f1 = std::move(fut_vec[i]);
              auto f2 = std::move(fut_vec[i+1]);
              futs.emplace_back(__impl_tp.submit(one_merge, std::move(f1), std::move(f2)));
            }
            if (fut_vec.size()%2 != 0) futs.emplace_back(std::move(fut_vec.back()));
            return futs;
          };

          std::vector<std::future<Ret>> futs;
          algos::partitioner::equal_size<I,S> algo(configs::stl_sort_cutoff(), data.begin(), data.end());
          partitioner chunks(algo);
          for(auto&& sr : chunks)
            futs.emplace_back(__impl_tp.submit(tp_sort, sr, cmp, prj));

          while (futs.size() > 1) {
            futs = level_merge_func(std::move(futs));
          }
          futs.front().wait();
        }
        return std::make_tuple(data.begin(), data.end());
      };
      return tp_sort(RangeData(FWD(start), FWD(end)), cmp, prj);
    }

  template <
    std::input_iterator I, std::sentinel_for<I> S,
    typename T,
    typename BinaryOp,
    typename UnaryOp = std::identity,
    typename ParitionAlgo = algos::partitioner::equal_size<I,S>
  >
  requires std::movable<T>
  constexpr decltype(auto) transform_reduce(I s, S e,
                                  T init,
                                  BinaryOp rdc_fn, UnaryOp tr_fn,
                                  ParitionAlgo algo) {
    auto transform_reduce_fn = [=](auto&& subrng) {
      return std::transform_reduce(subrng.begin(), subrng.end(), init, rdc_fn, tr_fn);
    };
    // schedule subranges
    auto f = __impl_tp.submit([=, this] () mutable {
        std::vector<std::future<T>> futs;
        partitioner partitions(algo);
        for(auto&& sr : partitions) {
          futs.emplace_back(__impl_tp.submit(transform_reduce_fn, sr));
        }
        return futs;
      }
    );
    // accumulate
    return __impl_tp.submit([=] (auto&& futs) mutable {
      return std::transform_reduce(futs.begin(), futs.end(), init, rdc_fn,
               [](auto&& fut) mutable { return fut.get(); });
      }, std::move(f.get()));
  }

  template<std::input_iterator I, std::sentinel_for<I> S, typename Fn>
  constexpr decltype(auto) for_each(I s, S e, Fn fn) {
    return std::ranges::for_each(s, e, [fn, this](auto&& x) { return __impl_tp.submit(fn, x); });
  }

  // parallel algorithm, for benchmarks see examples/reduce.cpp
  template <
    std::input_iterator I, std::sentinel_for<I> S,
    typename T,
    typename BinaryOp,
    typename PartAlgo = algos::partitioner::equal_size<I,S>
  >
  requires std::movable<T>
  constexpr decltype(auto) reduce(I s, S e, T init, BinaryOp rdc_fn, PartAlgo part_algo) {
    return transform_reduce(s, e, std::move(init),
                  std::forward<BinaryOp>(rdc_fn),
                  std::identity(),
                  std::forward<PartAlgo>(part_algo));
  }

};
} // stl_algo
} // thp
#endif // TP_STL_ALGO_HPP__
