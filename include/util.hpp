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

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <type_traits>

#include "include/traits.hpp"
#include "include/concepts.hpp"

namespace thp {

#define TP_DELETE_COPY_ASSIGN(x)  x& operator = (const x&) = delete; \
                                    x(const x&) = delete;

#define TP_DEFAULT_MOVE_ASSIGN(x)  x& operator = (x&&) = default; \
                                    x(x&&) = default;

#define FWD(x) std::forward<decltype(x)>(x)

namespace util {

inline void notify_cv(std::condition_variable_any& cv, std::size_t N) {
  if (N > 1) cv.notify_all();
  else cv.notify_one();
}

template<typename InputIter, typename OutputIter, typename Predicate>
InputIter copy_until(InputIter s, InputIter e, OutputIter o, Predicate fn) {
  while (s != e) {
    if (fn(*s))
      break;
    else [[likely]]
      *o++ = *s++;
  }
  return s;
}

template <typename Fn, typename Tup>
constexpr decltype(auto) apply_on_tuple(Fn &&fn, Tup &&tup) {
  return std::apply(
      [&](auto &&... x) {
        return std::make_tuple(
            std::invoke(std::forward<Fn>(fn), std::forward<decltype(x)>(x))...);
      },
      std::forward<Tup>(tup));
}

template <typename T, std::size_t... Idx>
auto __to_tuple_impl(T &&container, std::index_sequence<Idx...>) {
  return std::make_tuple(container.at(Idx)...);
}

template <typename T,
          typename = std::enable_if_t<traits::is_stl_container<T>::value>>
auto to_tuple(const T &data) {
  constexpr auto N = data.size();
  return __to_tuple_impl(FWD(data), std::make_index_sequence<N>());
}

template<typename C>
constexpr decltype(auto) collect_future(C&& t)
{
  using T = std::remove_cvref_t<decltype(t)>;

  if      constexpr (traits::is_smart_ptr<T>::value)   { return t->future(); }
  else if constexpr (std::is_base_of_v<executable, T>) { return t.future();  }
  else {
    static_assert("type is not supported");
  }
}

template <std::input_iterator I, std::sentinel_for<I> S>
  requires std::movable<std::iter_value_t<I>>
constexpr auto collect_future(I s, S e) {
  using TaskType = traits::FindTaskType<std::iter_value_t<I>>::type;
  using ReturnType = std::decay_t<typename TaskType::ReturnType>;

  std::deque<std::future<ReturnType>> futs;
  std::ranges::transform(s, e, std::back_inserter(futs),
                         [&](auto&& x) { return collect_future(x); });
  return futs;
}

template <std::ranges::input_range V>
  requires kncpt::ThreadPoolTask<std::ranges::range_value_t<V>>
constexpr auto collect_future(V&& v) {
  using TaskType = std::ranges::range_value_t<V>;
  using ReturnType = std::decay_t<typename TaskType::ReturnType>;

  std::deque<std::future<ReturnType>> futs;
  std::ranges::transform(v, std::back_inserter(futs),
                         [&](auto&& x) { return collect_future(x); });
  return futs;
}

template < class InputIt >
auto when_all(InputIt first, InputIt last) -> std::future<std::vector<typename std::iterator_traits<InputIt>::value_type>>
{

}

template<typename Tuple, std::size_t... I>
constexpr Tuple create_tuple(std::index_sequence<I...>) {
  return std::make_tuple(std::tuple_element_t<I, Tuple>()...); 

} 

} // namespace util
} // namespace thp

#endif
