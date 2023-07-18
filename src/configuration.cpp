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

#include <iostream>
#include <memory>
#include <pthread.h>
#include <sstream>
#include <string>
#include <vector>

#include <signal.h>

#include "include/configuration.hpp"
#include "include/managed_thread.hpp"

#include "platform/thread_config.h"

namespace thp {
namespace platform {

thread_config::thread_config() { conf_ = config_get_thread_config(); }

thread_config::thread_config(const thread_config &rhs) {
  conf_ = config_get_thread_config();
  *conf_ = *rhs.conf_;
}

thread_config &thread_config::operator=(const thread_config &rhs) {
  if (!conf_)
    conf_ = config_get_thread_config();

  *conf_ = *rhs.conf_;
  return *this;
}

int thread_config::apply(managed_thread *th) {
  auto hndl = th->native_handle();
  return config_apply(hndl, conf_);
}

int thread_config::retrieve(managed_thread *th) {
  auto hndl = th->native_handle();
  return config_retrieve(hndl, conf_);
}

thread_configuration &thread_config::set_priority(int priority) {
  config_set_priority(conf_, priority);
  return *this;
}

thread_configuration &thread_config::set_policy(int policy) {
  config_set_policy(conf_, policy);
  return *this;
}

thread_configuration &thread_config::set_affinity(std::vector<unsigned> cpuid) {
  int arr[64];
  std::copy(cpuid.begin(), cpuid.end(), arr);
  config_set_affinity(conf_, arr, cpuid.size());
  return *this;
}

thread_configuration &thread_config::unblock_signals(std::vector<int> sig) {
  config_unblock_signals(conf_, sig.data(), sig.size());
  return *this;
}

const sigset_t *thread_config::get_sigset() const {
  return config_get_sigset(conf_);
}

int thread_config::cancel(managed_thread *th) {
  return config_cancel_thread(th->native_handle());
}

void thread_config::display(std::ostream& oss) {
  oss << "policy: " << conf_->policy_ << "\n"
      << "cancel_state: " << conf_->cancel_state_ << std::endl;
}

thread_config::~thread_config() { config_remove_thread_config(conf_); }

} // namespace platform
} // namespace thp
