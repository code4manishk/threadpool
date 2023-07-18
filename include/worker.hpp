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

#ifndef WORKER_HPP_
#define WORKER_HPP_

#include <atomic>

#include "include/managed_thread.hpp"
#include "include/task_queue.hpp"

namespace thp {

// one unit of worker
struct worker : managed_thread {
  using id_type = std::thread::id;

  template <typename Fn, typename... Args>
  explicit worker(const managed_stop_source &stop_src, Fn &&fn, Args &&...args)
      : taskq_{nullptr}, sema_{0}, th_{std::make_unique<platform::thread>(
                                       stop_src, FWD(fn), FWD(args)...)} {}

  worker(worker &&rhs) noexcept
      : taskq_{nullptr}, sema_{0}, th_{std::move(rhs.th_)} {
    taskq_.store(rhs.taskq_.load());
    if (rhs.sema_.try_acquire())
      sema_.release();

    rhs.taskq_.store(nullptr);
    rhs.sema_.release();
  }

  worker &operator=(worker &&rhs) noexcept {
    if (this != &rhs) {
      if (rhs.sema_.try_acquire())
        sema_.release();

      th_ = std::move(rhs.th_);
      taskq_.store(rhs.taskq_.load());
    }
    return *this;
  }

  worker &operator=(const worker &) = delete;
  worker(const worker &) = delete;

  task_queue *serve(task_queue *q) {
    taskq_.store(q, std::memory_order_release);
    return q;
  }

  task_queue *my_queue() const noexcept {
    return taskq_.load(std::memory_order_acquire);
  }

  bool joinable() noexcept override { return th_->joinable(); }
  std::thread::native_handle_type native_handle() override {
    return th_->native_handle();
  }
  std::thread::id get_id() noexcept { return th_->get_id(); }
  void join() override {
    if (th_->joinable()) {
      th_->join();
    }
  }
  bool request_stop() override { return th_->request_stop(); }
  void request_resume() override { th_->request_resume(); }
  void request_pause() override { th_->request_pause(); }
  void sleep() noexcept override { sema_.acquire(); }
  void wakeup() noexcept override { sema_.release(); }

  std::weak_ptr<thread_configuration> config() override {
    return th_->config();
  }
  ~worker() = default;

private:
  alignas(hardware_destructive_interference_size) std::atomic<task_queue *> taskq_;
  std::binary_semaphore sema_;
  std::unique_ptr<platform::thread> th_;
};

} // namespace thp

#endif /* WORKER_HPP_ */
