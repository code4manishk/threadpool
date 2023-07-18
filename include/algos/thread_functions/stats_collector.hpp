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

#ifndef STATS_COLLECTOR_HPP__
#define STATS_COLLECTOR_HPP__

#include "include/configuration.hpp"
#include "include/worker_pool.hpp"
#include "include/managed_stop_token.hpp"

namespace thp {
namespace algos {
#if 0
struct stats_collector {
  explicit stats_collector(worker_pool<>& pool)
  : worker_pool_{pool}
  , fn([&] (managed_stop_token st, std::ostream& oss = std::cerr) {
      std::vector<unsigned> worker_utilization;
      std::mutex mu;
      std::condition_variable_any cv;
      while(true) {
        const auto state = st.current_state();
        switch(state) {
          case stop_source_state_t::stopped:
          case stop_source_state_t::paused:
          case stop_source_state_t::running:
          default:
            break;
        }
        oss << "stats_collector\n";
    #if 0
        l.unlock();

        jobq_.copy_stats(stats.jobq);

        worker_utilization.clear();
        worker_utilization.resize(stats.jobq.algo.taskq_len.size(), 0);
        auto start = std::chrono::system_clock::to_time_t(stats.jobq.ts);
        oss << "job queue stats at: " << std::asctime(std::localtime(&start));
        oss << "total tasks: " << stats.jobq.algo.num_tasks << '\n';
        //for(auto& p : stats.jobq.worker_2_q)
        //  oss << p.first << " : "  << p.second << '\n';
        for(auto& w : stats.jobq.worker_2_q) {
          ++worker_utilization[w.second];
          oss << w.second << ", ";
        }
        oss << "\nQueue Length Worker\n";
        for(unsigned i = 0; i < worker_utilization.size(); ++i)
          oss << std::setw(5) << i << " " << std::setw(6) << stats.jobq.algo.taskq_len[i]
              << " " << worker_utilization[i] << '\n';

        oss << std::endl;
        oss << "job_queue stats: " << std::endl;
    #endif
      }
    }
  )
  {}

  worker_pool<>& worker_pool_;
  std::function<void(managed_stop_token, std::ostream&)> fn;
};
#endif
} // namespace algos
} // namespace thp

#endif // STATS_COLLECTOR_HPP__
