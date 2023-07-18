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

#ifndef TASK_FACTORY_HPP__
#define TASK_FACTORY_HPP__

#include <type_traits>
#include <functional>

#include "include/task_type.hpp"
#include "include/all_priority_types.hpp"
#include "include/register_types.hpp"
#include "include/util.hpp"
#include "include/all_priority_types.hpp"

namespace thp {

template <typename Fn, typename...Args>
  requires std::regular_invocable<Fn,Args...>
[[nodiscard]] constexpr inline decltype(auto) make_task(Fn&& fn, Args&& ...args) {
  using Ret = std::invoke_result_t<Fn,Args...>;
  return simple_task(std::packaged_task<Ret()>{std::bind_front(FWD(fn), FWD(args)...)});
}

template<typename Prio, typename Fn, typename... Args>
  requires std::regular_invocable<Fn,Args...>
[[nodiscard]] constexpr inline decltype(auto) make_task(Fn&& fn, Args&& ...args) {
  using Ret = std::invoke_result_t<Fn,Args...>;
  static_assert(compile_time::find<Prio, AllPriority>() < std::tuple_size_v<AllPriority>, "priority type not registered");
  return priority_task<Prio>(std::packaged_task<Ret()>{std::bind_front(FWD(fn), FWD(args)...)});
}

}

#endif // TASK_FACTORY_HPP__
