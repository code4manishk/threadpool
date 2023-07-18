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

#ifndef ONESHOT_HPP__
#define ONESHOT_HPP__

#include "include/all_priority_types.hpp"
#include "include/job_queue.hpp"
#include "include/managed_stop_token.hpp"
#include "include/statistics.hpp"
#include "include/worker.hpp"
#include "include/worker_pool.hpp"
#include "include/clock_util.hpp"

namespace thp {
namespace algos {
namespace scheduler {

struct oneshot {
  explicit oneshot(statistics &stats, job_queue<TaskQueueTupleType> &jobq,
                   worker_pool<worker> &pool, worker_pool<worker> &managers)
      : stats_{stats}, jobq_{jobq}, worker_pool_{pool}
      , managers_pool_{managers}
  {
    scheduler_fn_ = [&](managed_stop_token st) noexcept {
      auto [_, me] = managers_pool_.worker_info(std::this_thread::get_id()).value();

      while (true) {
        const auto state = st.current_state();
        switch (state) {
        case stop_source_state_t::stopped:
          return;
          break;
        case stop_source_state_t::running: {
          if (auto q = jobq_.best_queue(stats_)) {
            while (!q->empty()) {
              auto &w = worker_pool_.free_worker(stats_);
              w.serve(q);
              w.wakeup();
            }
          }
          else {
            me.sleep(); // no task
          }
        }
        break;

        default:
          break;
        }
      }
    };

    worker_fn_ = [&](managed_stop_token st) noexcept {
      task_queue *q = stats_.jobq.in.qs.front();
      auto [idx, me] = worker_pool_.worker_info(std::this_thread::get_id()).value();
      worker_pool_.not_working(idx);

      while (true) {
        const auto state = st.current_state();
        switch (state) {
        case stop_source_state_t::stopped:
          return;
          break;
        case stop_source_state_t::paused:
          worker_pool_.not_working(idx);
          me.sleep();
          break;
        case stop_source_state_t::running:
          if (nullptr != (q = me.my_queue())) {
            q->accept(me);
            worker_pool_.not_working(idx);
            me.sleep();
          }
          break;
        default:
          break;
        }
      }
    };
  }

  statistics &stats_;
  job_queue<TaskQueueTupleType> &jobq_;
  worker_pool<worker> &worker_pool_;
  worker_pool<worker> &managers_pool_;

  std::function<void(managed_stop_token)> scheduler_fn_, worker_fn_;
};

} // namespace scheduler
} // namespace algos
} // namespace thp

#endif // ONESHOT_HPP__
