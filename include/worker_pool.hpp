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

#ifndef WORKER_POOL_HPP_
#define WORKER_POOL_HPP_

#include <algorithm>
#include <cassert>
#include <climits>
#include <mutex>
#include <ranges>
#include <semaphore>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "include/concepts.hpp"
#include "include/configuration.hpp"
#include "include/coroutine/generator.hpp"
#include "include/managed_thread.hpp"
#include "include/statistics.hpp"
#include "include/worker.hpp"

namespace thp {
namespace vw = std::ranges::views;

template <kncpt::ManageableThread WorkerType>
struct worker_pool {
  explicit worker_pool(std::string_view name, unsigned n)
      : mu_{}, free_workers_{0}, threads_{}, workers_{}, max_workers_{n},
        name_{name}, device_name_{"cpu"}, stop_src_{}, cond_{}
  {
    if (n > MaxIndex)
      throw std::logic_error("requested worker pool size is greater");
  }

  template <typename... Fn> decltype(auto) run(Fn &&...fn) {
    std::unique_lock l(mu_);
    return std::make_tuple(start_worker(FWD(fn))...);
  }

  template <typename Fn>
  auto start(Fn &&fn) -> std::vector<std::thread::id> {
    std::unique_lock l(mu_);
    std::vector<std::thread::id> ids;
    rng::transform(vw::iota(0u, max_workers_), std::back_inserter(ids), [&](unsigned) {
                  return start_worker(FWD(fn)); });
    std::erase(ids, std::thread::id{});
    return ids;
  }

  void not_working(const unsigned idx) noexcept {
    auto old_val = free_workers_.load(std::memory_order_relaxed);
    auto new_val = old_val | (1ll << idx);
    while (!free_workers_.compare_exchange_weak(
        old_val, new_val, std::memory_order_release, std::memory_order_relaxed));

    free_workers_.notify_one();
  }

  WorkerType &free_worker(statistics &) noexcept {
    auto idx = 0;
    std::int64_t old_val = 0;
    do {
      free_workers_.wait(0ll, std::memory_order_acquire);
      old_val = free_workers_.load(std::memory_order_relaxed);
      idx = __builtin_ffsll(old_val);
    } while (idx == 0ll);

    auto new_val = old_val & (~(1ll << (idx - 1)));
    while (!free_workers_.compare_exchange_weak(
        old_val, new_val, std::memory_order_release, std::memory_order_relaxed));

    return std::ref(threads_[idx - 1]);
  }

  std::optional<std::tuple<unsigned int, thp::worker&>>
  worker_info(const std::thread::id &id) noexcept {
    std::shared_lock l(mu_);
    auto it = workers_.find(id);
    if (it != workers_.end())
      return std::make_tuple(it->second, std::ref(threads_[it->second]));

    return std::nullopt;
  }

  std::weak_ptr<thread_configuration> worker_config(std::thread::id id) {
    std::shared_lock l(mu_);
    return threads_[workers_[id]].config();
  }

  void pause() {}
  void resume() {}

  void shutdown() {
    if (stop_src_.request_stop()) {
      wait();
    }
  }

  void wait() {
    for (auto &&w : workers_ | vw::values) {
      threads_[w].wakeup();
      threads_[w].join();
    }
  }

  ~worker_pool() { shutdown(); }

  void wakeup_all() noexcept {
    for (auto &&w : threads_) {
      w.wakeup();
    }
  }

protected:
  template <typename F>
  std::thread::id start_worker(F &&f) {
    try {
      static unsigned int idx = 0;
      if (idx < MaxIndex) {
        WorkerType w(stop_src_, FWD(f), stop_src_.get_managed_token());
        auto id = w.get_id();
        workers_.emplace(id, idx++);
        threads_.emplace_back(std::move(w));
        return id;
      }
    }
    catch(std::exception& e) {}

    return std::thread::id{};
  }

private:

  using BITVEC = std::int_fast64_t;
  static const unsigned MaxIndex = CHAR_BIT * sizeof(BITVEC);

  alignas(std::hardware_destructive_interference_size) mutable platform::
      spin_shared_mutex mu_;
  alignas(std::hardware_destructive_interference_size)
      std::atomic<BITVEC> free_workers_;
  std::vector<WorkerType> threads_;
  std::unordered_map<std::thread::id, unsigned int> workers_; // std::flatmap
  unsigned max_workers_;
  std::string name_;
  std::string device_name_;
  managed_stop_source stop_src_;
  std::condition_variable_any cond_;
  // std::stop_callback<std::function<void(std::jthread_id)> stop_cb_;
};

} // namespace thp

#endif // WORKER_POOL_HPP_
