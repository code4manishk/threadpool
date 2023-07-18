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

#ifndef TASK_TYPE_HPP_
#define TASK_TYPE_HPP_

#include <compare>
#include <execution>
#include <future>

#include "include/configuration.hpp"
#include "include/executable.hpp"

namespace thp {

template <typename P = void> struct priority_task {
  using PriorityType = P;

  template <typename PackagedTask>
    requires std::regular_invocable<PackagedTask> && std::movable<PackagedTask>
  explicit priority_task(PackagedTask &&pt)
      : prio_{}, pt_([fn = std::move(pt)] mutable { fn(); }) {}

  void execute() noexcept { pt_(); }

  priority_task<PriorityType> &priority(PriorityType prio) {
    prio_ = std::move(prio);
    return *this;
  }
  const PriorityType &priority() const { return prio_; }

  auto operator<=>(const priority_task<PriorityType> &rhs) const noexcept {
    return prio_ <=> rhs.prio_;
  }

protected:
  PriorityType prio_;
  std::move_only_function<void()> pt_;
};

template <> struct priority_task<void> {
  using PriorityType = void;

  template <typename PackagedTask>
    requires std::regular_invocable<PackagedTask> && std::movable<PackagedTask>
  explicit priority_task(PackagedTask &&pt)
      : pt_([fn = std::move(pt)] mutable { fn(); }) {}

  void execute() noexcept { pt_(); }

  auto operator<=>(const priority_task<void> &) const noexcept {
    return std::strong_ordering::less;
  }

protected:
  std::move_only_function<void()> pt_;
};

using simple_task = priority_task<void>;

} // namespace thp

#endif // TASK_TYPE_HPP_
