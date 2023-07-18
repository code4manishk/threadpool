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

#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <ranges>
#include <thread>
#include <type_traits>
#include <vector>

#include "include/algos/partitioner/equal_size.hpp"
#include "include/concepts.hpp"
#include "include/coroutine/generator.hpp"
#include "include/managed_stop_source.hpp"
#include "include/managed_thread.hpp"
#include "include/partitioner.hpp"
#include "include/scheduling_algo.hpp"
#include "include/task_factory.hpp"
#include "include/task_type.hpp"
#include "include/util.hpp"
#include "include/worker_pool.hpp"

namespace thp {

class threadpool final {
public:
  explicit threadpool(unsigned max_threads = std::thread::hardware_concurrency());

  template <typename Fn, std::ranges::input_range R>
  generator<std::invoke_result_t<Fn, rng::range_value_t<R>>>
  constexpr map(Fn &&fn, R &&args, std::integral auto chunksize = 1u) {
    using Arg = std::ranges::range_value_t<R>;
    using Ret = std::invoke_result_t<Fn, Arg>;

    chunksize = std::clamp(chunksize, 1u, max_threads_);

    if (chunksize == 1u) {
      for (auto &&v : args)
        co_yield fn(v); // std::move_if_noexcept(submit(fn, v).get());
    }
    else {
      algos::partitioner::equal_size algo(chunksize, args.begin(), args.end());
      partitioner chunks(algo);
      std::vector<std::future<generator<Ret>>> futs;

      if constexpr (rng::sized_range<R> && requires { typename R::size; })
        futs.reserve(chunks.count());

      for (auto &&sr : chunks) { // args | vw::chunk(chunksize)
        futs.emplace_back(submit(
            [this, fn](auto &&args) -> generator<Ret> {
              for (auto &&v : args)
                co_yield fn(v);
            },
            std::move(sr)));
      }

      for (auto &&fv : futs)
        for (auto &&v : fv.get())
          co_yield std::move(v);
    }
  }

  template <typename Fn, typename... Args>
  constexpr std::future<std::invoke_result_t<Fn, Args...>>
  submit(Fn &&fn, Args &&...args) {
    using Ret = std::invoke_result_t<Fn, Args...>;
    std::packaged_task<Ret()> pt{std::bind_front(FWD(fn), FWD(args)...)};
    auto fut = pt.get_future();
    jobq_.schedule_task(simple_task{std::move(pt)});
    scheduler_->wakeup();
    return fut;
  }

  ~threadpool();

  // waits till condition of no tasks is satisfied
  void drain();

  // pause
  void pause();
  void resume();

  // quick shutdown, may not run all tasks
  void stop();

  // graceful shutdown
  void shutdown();

private:
  mutable std::mutex mu_;
  std::condition_variable_any shutdown_cv_, idle_cond_;
  managed_stop_source stop_src_, etc_stop_src_;
  // std::stop_callback<std::function<void()>> stop_cb_;
  job_queue<TaskQueueTupleType> jobq_;
  worker_pool<worker> cpu_pool_;
  worker_pool<worker> managers_;
  //worker_pool<worker> book_keepers_;
  worker *scheduler_;
  statistics stats_;
  unsigned max_threads_;
  scheduling_algo tp_algo_;
  std::once_flag del_flag_;

  TP_DELETE_COPY_ASSIGN(threadpool)
};

} // namespace thp
#endif /* THREADPOOL_HPP_ */
