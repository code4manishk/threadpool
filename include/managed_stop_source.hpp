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

#ifndef MANAGED_STOP_SOURCE_HPP__
#define MANAGED_STOP_SOURCE_HPP__

#include <condition_variable>
#include <semaphore>
#include <shared_mutex>
#include <stop_token>
#include <unordered_map>

namespace thp {

typedef enum {
  running = 0,
  paused,
  waiting,
  stopped,
  cancelled,
} stop_source_state_t;

// fwd declaration
struct managed_stop_token;

// implementation class
struct managed_stop_source_impl
    : std::enable_shared_from_this<managed_stop_source_impl> {
  [[nodiscard]] static std::shared_ptr<managed_stop_source_impl> create() {
    return std::shared_ptr<managed_stop_source_impl>(
        new managed_stop_source_impl());
  }

  std::shared_ptr<managed_stop_source_impl> clone() {
    return shared_from_this();
  }

  void request_pause() noexcept {
    state.store(stop_source_state_t::paused, std::memory_order_release);
  }

  void request_resume() noexcept {
    state.store(stop_source_state_t::running, std::memory_order_release);
  }

  void request_stop() noexcept {
    state.store(stop_source_state_t::stopped, std::memory_order_release);
  }

  stop_source_state_t current_state() const noexcept {
    return state.load(std::memory_order_acquire);
  }

#if 0
  void pause() {
	  std::unique_lock l(mu);
	  cond.wait(l, [&] { return !paused; });
  }

  void block(const std::thread::id& id) {
    std::unique_lock l(mu);
    if (!sema.contains(id))
      sema.emplace(id, std::make_unique<std::binary_semaphore>(0));
    sema[id]->acquire();
  }

  void unblock(const std::thread::id& id) {
    std::unique_lock l(mu);
    sema[id]->release();
  }
#endif

private:
  managed_stop_source_impl() = default;

  std::atomic<stop_source_state_t> state;
};

struct managed_stop_source : std::stop_source {
  friend class managed_stop_token;

  explicit managed_stop_source()
      : std::stop_source{}, impl{managed_stop_source_impl::create()} {
    // std::cerr << "managed_stop_source(): " << impl.get() << std::endl;
  }

  managed_stop_source(const managed_stop_source &rhs)
      : std::stop_source{rhs}, impl{rhs.impl->clone()} {}

  managed_stop_source &operator=(const managed_stop_source &rhs) {
    if (this != &rhs)
      impl = rhs.impl->clone();
    return *this;
  }

  managed_stop_source(managed_stop_source &&rhs) noexcept = default;
  managed_stop_source &operator=(managed_stop_source &&rhs) noexcept = default;

  friend std::ostream &operator<<(std::ostream &oss,
                                  const managed_stop_source &src) {
    oss << "managed_stop_source: " << &src << "::" << src.impl.get()
        << "::" << src.impl.use_count();
    return oss;
  }

  [[nodiscard]] managed_stop_token get_managed_token() noexcept;

  void request_pause() noexcept { impl->request_pause(); }
  void request_resume() noexcept { impl->request_resume(); }
  bool request_stop() const noexcept {
    bool ret = false;
    if (std::stop_source::request_stop()) {
      impl->request_stop();
      ret = true;
    }
    return ret;
  }
  virtual ~managed_stop_source() = default;

protected:
  std::shared_ptr<managed_stop_source_impl> impl;
};

} // namespace thp

#endif // MANAGED_STOP_SOURCE_HPP__
