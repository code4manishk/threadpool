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

#ifndef STATISTICS_HPP__
#define STATISTICS_HPP__

#include "include/configuration.hpp"
#include "include/task_queue.hpp"
//#include "include/job_queue.hpp"
#include "include/executable.hpp"

namespace thp {

struct workerpool_stats {
  std::uint16_t num_threads;
  std::uint16_t num_workers;
};

struct outputs {
  task_queue* cur_output;
};

struct inputs {
  std::vector<task_queue*> qs;
  std::uint32_t num_tasks;
  std::uint32_t load_factor;
};

struct jobq_stats {
  inputs in;
  outputs out;
};

struct statistics {
  std::chrono::system_clock::time_point ts;
  struct jobq_stats jobq;
  struct workerpool_stats pool;
};

} // namespace thp

#endif // STATISTICS_HPP__
