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

#ifndef CONFIGURATION_HPP__
#define CONFIGURATION_HPP__

#include <memory>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <chrono>
#include <new>
#include <thread>

#include "platform/thread_config.h"

namespace thp {
struct managed_thread;

#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
    constexpr std::size_t hardware_constructive_interference_size = 64*2;
    constexpr std::size_t hardware_destructive_interference_size = 64*2;
#endif


struct thread_configuration {
  virtual int apply(managed_thread *th) = 0;
  virtual int retrieve(managed_thread *th) = 0;
  virtual int cancel(managed_thread *th) = 0;

  virtual thread_configuration &set_priority(int priority) = 0;
  virtual thread_configuration &set_policy(int policy) = 0;
  virtual thread_configuration &set_affinity(std::vector<unsigned> cpuid) = 0;
  virtual thread_configuration &unblock_signals(std::vector<int> sig) = 0;

  virtual void display(std::ostream& oss) = 0;
  virtual const sigset_t *get_sigset() const = 0;
  virtual ~thread_configuration() = default;
};

namespace platform {
class thread_config : public thread_configuration {
public:
  explicit thread_config();
  thread_config(const thread_config &rhs);
  thread_config &operator=(const thread_config &rhs);

  thread_configuration &set_priority(int priority) override;
  thread_configuration &set_policy(int policy) override;
  thread_configuration &set_affinity(std::vector<unsigned> cpuid) override;
  thread_configuration &unblock_signals(std::vector<int> sig) override;
  const sigset_t *get_sigset() const override;

  int cancel(managed_thread *th) override;
  void display(std::ostream& oss) override;
  int apply(managed_thread *th) override;
  int retrieve(managed_thread *th) override;
  virtual ~thread_config();

private:
  struct config *conf_;
};

struct task_configuration {
  //std::weak_ptr<device_configuration> device_config_;
  std::weak_ptr<thread_configuration> thread_config_;
};

} // namespace platform

namespace configs {
  constexpr inline decltype(auto) shutdown_grace_period()    { return std::chrono::milliseconds(2000); }
  constexpr inline decltype(auto) stats_collection_period()  { return std::chrono::milliseconds(1000); }
  constexpr inline decltype(auto) schedule_request_timeout() { return std::chrono::milliseconds(10);   }
  constexpr inline decltype(auto) scheduler_tick()           { return std::chrono::microseconds(10);   }
  constexpr inline decltype(auto) per_queue_capacity()       { return 16*1024;                         }
  constexpr inline decltype(auto) queue_table_capacity()     { return 1024;                            }
  constexpr inline decltype(auto) stl_sort_cutoff()          { return 32*32*1024u;                   }
            inline decltype(auto) hardware_concurrency()     { return std::thread::hardware_concurrency(); }
} // namespace configs

} // namespace thp

#endif // CONFIGURATION_HPP__
