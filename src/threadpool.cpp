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

#include <condition_variable>
#include <future>
#include <memory>
#include <thread>
#include <glog/logging.h>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <mutex>

#include "include/threadpool.hpp"
#include "include/worker_pool.hpp"
#include "include/managed_stop_token.hpp"
#include "include/algos/thread_functions/signal_handler.hpp"
#include "include/algos/thread_functions/stats_collector.hpp"

namespace thp {

threadpool::threadpool(unsigned max_threads)
  : mu_{}
  , shutdown_cv_{}
  , idle_cond_{}
  , stop_src_{}
  , etc_stop_src_{}
  , jobq_{}
  , cpu_pool_{"cpu_pool:0", max_threads}
  , managers_{"schedulers", 1}
  , scheduler_{nullptr}
  , stats_{}
  , max_threads_{max_threads}
  , tp_algo_{stats_, jobq_, cpu_pool_, managers_}
{
  std::lock_guard l{mu_};

  jobq_.init_stats(stats_);

  auto workers = cpu_pool_.start([&](managed_stop_token st) { auto f = tp_algo_.worker_fn(); f(st); });
  auto [tid] = managers_.run([&](managed_stop_token st) { auto f = tp_algo_.scheduler_fn(); f(st); });
  if (workers.empty() or (tid == std::thread::id()))
    throw std::runtime_error("couldn't start workers/managers, runtime error");

  auto [_, th] = managers_.worker_info(tid).value();
  scheduler_ = &th;
}

void threadpool::shutdown() {
  cpu_pool_.shutdown();
  managers_.shutdown();
}

void threadpool::pause() {
	jobq_.close();
	cpu_pool_.pause();
	managers_.pause();
}

void threadpool::resume() {
  managers_.resume();
  cpu_pool_.resume();
}

void threadpool::stop() {
  jobq_.close();  
  jobq_.stop();
  shutdown();
}

threadpool::~threadpool() {
  std::call_once(del_flag_, [&] { stop(); });
}

} // namespace thp
