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

#ifndef MANAGED_THREAD_HPP_
#define MANAGED_THREAD_HPP_

#include <algorithm>
#include <functional>
#include <shared_mutex>
#include <thread>

#include "include/configuration.hpp"
#include "include/managed_stop_source.hpp"
#include "include/util.hpp"

namespace thp {
// interface
struct managed_thread {

  managed_thread() noexcept {}

  managed_thread(const managed_thread &) = delete;
  managed_thread &operator=(const managed_thread &) = delete;

  managed_thread(managed_thread &&) noexcept = default;
  managed_thread &operator=(managed_thread &&) noexcept = default;

  virtual bool joinable() noexcept = 0;
  virtual std::thread::native_handle_type native_handle() = 0;
  virtual std::weak_ptr<thread_configuration> config() = 0;

  virtual void join() = 0;
  virtual bool request_stop() = 0;
  virtual void request_resume() = 0;
  virtual void request_pause() = 0;
  virtual void sleep() = 0;
  virtual void wakeup() = 0;

  virtual ~managed_thread() = default;
};

namespace platform {

// one unit of worker
struct thread : thp::managed_thread {
  using id_type = std::thread::id;

  explicit thread() noexcept
      : managed_thread(), stop_src_{}, config_{nullptr}, th_{} {}

  template <typename Fn, typename... Args>
    requires std::is_invocable_v<Fn, Args...>
  constexpr explicit thread(const managed_stop_source &stop_src, Fn &&fn,
                            Args &&...args)
      : stop_src_{stop_src}, config_{nullptr}, th_{create(FWD(fn),
                                                          FWD(args)...)} {}

  thread(thread &&) noexcept = default;
  thread &operator=(thread &&) noexcept = default;

  thread(const thread &) = delete;
  thread &operator=(const thread &) = delete;

  bool joinable() noexcept override { return th_.joinable(); }
  id_type get_id() const noexcept { return th_.get_id(); }
  std::thread::native_handle_type native_handle() override {
    return th_.native_handle();
  }

  void join() override {
    if (joinable()) {
      th_.join();
    }
  }
  bool request_stop() override { return stop_src_.request_stop(); }
  void request_resume() override { stop_src_.request_resume(); }
  void request_pause() override { stop_src_.request_pause(); }
  void sleep() override {}
  void wakeup() override {}

  void update_config(const thread_config &new_config) {
    if (!config_)
      config_ = std::make_shared<thread_config>(new_config);

    config_->apply(this);
  }

  std::weak_ptr<thread_configuration> config() override {
    if (!config_) {
      config_ = std::make_shared<thread_config>();
      config_->retrieve(this);
    }
    return config_;
  }

  virtual ~thread() {
    try {
      request_stop();
      join();
    } catch (...) {
    }
  }

protected:
  template <typename Fn, typename... Args>
  constexpr std::thread create(Fn &&fn, Args &&...args) {
    // using Tup = std::tuple<Args...>;
    // using FirstType = std::tuple_element_t<0, Tup>;

    // if constexpr (std::is_same_v<FirstType, managed_stop_token>)
    //   return std::thread(FWD(fn), stop_src_.get_managed_token(),
    //   FWD(args)...);
    // else
    return std::thread(FWD(fn), FWD(args)...);
  }

private:
  managed_stop_source stop_src_;
  std::shared_ptr<thread_configuration> config_;
  std::thread th_;
};

} // namespace platform
} // namespace thp

#endif /* MANAGED_THREAD_HPP_ */
