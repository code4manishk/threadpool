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

#ifndef WAITING_HPP__
#define WAITING_HPP__

#include "include/algos/queue_selection/first_available.hpp"
#include "include/all_priority_types.hpp"
#include "include/job_queue.hpp"
#include "include/managed_stop_token.hpp"
#include "include/statistics.hpp"
#include "include/worker.hpp"
#include "include/worker_pool.hpp"

namespace thp {
namespace algos {
namespace scheduler {

struct waiting {
  explicit waiting(statistics &stats, job_queue<TaskQueueTupleType> &jobq,
                   worker_pool<worker> &pool, worker_pool<worker> &managers)
      : stats_{stats}, jobq_{jobq}, worker_pool_{pool}
      , managers_pool_{managers} {
    scheduler_fn_ = [&](managed_stop_token st) {
      task_queue *q = nullptr;
      std::condition_variable_any no_task_cond;
      std::shared_mutex mu;
      algos::queue_select::first_available qs;

      while (true) {
        const auto state = st.current_state();
        switch (state) {
        case stop_source_state_t::stopped:
          return;
          break;
        case stop_source_state_t::running:
          if (nullptr != (q = jobq_.best_queue(stats_))) {
            auto &w = worker_pool_.free_worker(stats_);

            w.serve(q);
            w.wakeup();
            // std::cerr << "scheduler_fn: " << q->size() << ", free: " <<
            // w.get_id() << '\n';
          } else {
            // go into polling mode
            std::unique_lock l(mu);
            do {
              if (st.stop_requested())
                break;
            } while (no_task_cond.wait_for(l, st, std::chrono::microseconds(50),
                                           [&] { return !qs.apply(stats_); }));
          }
          break;

        default:
          break;
        }
      }
    };

    worker_fn_ = [&](managed_stop_token st) {
      const auto id = std::this_thread::get_id();
      auto [idx, me] = worker_pool_.worker_info(id).value();
      // auto& me = worker_pool_.thread(idx);
      task_queue *q = stats_.jobq.in.qs.front();
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
          q = q ? q : me.my_queue();
          // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
          if (q) {
            // std::cerr << "scheduler_fn: " << q->size() << ", free: " <<
            // w.get_id() << '\n'; worker_pool_.working(id);
            q->accept(me);
            q = nullptr;
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

#endif // WAITING_HPP__
