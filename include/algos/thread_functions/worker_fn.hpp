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

#ifndef __WORKER_FN__
#define __WORKER_FN__

#include <variant>
#include <functional>
#include <optional>
#include <memory>

namespace thp {
#if 0
template<typename T>
class thread_fn {
public:
  constexpr explicit thread_fn(ThpWorkerVariant&& fn) : var{std::move(fn)} {}

  thread_fn(const thread_fn&) = delete;
  thread_fn& operator=(const thread_fn) = delete;

  thread_fn(thread_fn&&) = default;
  ~thread_fn() = default;

  constexpr void operator()() {
    auto config = std::get<T>(var);
    for(;;) {
      try {
        auto init_lambda = config->initializer();
        auto work_loop   = config->work_loop();
        auto finalizer   = config->finalizer();

        if (init_lambda) std::invoke(init_lambda.value());
        if (work_loop) std::invoke(work_loop.value(), config->stop_token());
      }
      catch(worker_config_change_except &ex) {}
      catch(worker_stop_except& ex) {
        if (finalizer) std::invoke(finalizer.value());
        break;
      }
      catch(...) {
        throw;
      }
    }
  }

private:
  ThpWorkerVariant var;
};
#endif
} // namespace thp

#endif // __WORKER_FN__
