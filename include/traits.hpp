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

#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <array>
#include <chrono>
#include <deque>
#include <forward_list>
#include <list>
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>
#include <tuple>

#include "include/task_type.hpp"

namespace thp {
namespace traits {

template <typename T, template <typename...> class Template>
struct is_specialization : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

// smart pointers
  template <typename T> struct is_unique_ptr : std::false_type {};
  template <typename T> struct is_shared_ptr : std::false_type {};

  template <typename T> struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};
  template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

  template <typename T> inline constexpr bool is_unique_ptr_v = is_unique_ptr<T>::value;
  template <typename T> inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

  template <typename T>
  struct is_smart_ptr
  : std::integral_constant<bool, std::disjunction_v<is_unique_ptr<T>, is_shared_ptr<T>>> {};

template <typename T> struct is_timepoint : std::false_type {};

template <typename T>
struct is_timepoint<std::chrono::time_point<T>> : std::true_type {};

template<typename T>
struct is_reference_wrapper : std::false_type {};

template<typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type{};

  template <typename... T> struct is_vector : std::false_type {};
  template <typename... T> struct is_list : std::false_type {};
  template <typename... T> struct is_deque : std::false_type {};

  template <typename... T> struct is_vector<std::vector<T...>> : std::true_type {};
  template <typename... T> struct is_list<std::list<T...>> : std::true_type {};
  template <typename... T> struct is_deque<std::deque<T...>> : std::true_type {};

  template <typename... T>
  struct is_stl_container
  : std::integral_constant<
      bool, 
      std::disjunction_v<is_vector<T...>, is_list<T...>, is_deque<T...>>
    >
  {};

template<typename> struct is_tuple : std::false_type {};
template<typename... T> struct is_tuple<std::tuple<T...>> : std::true_type {};

#if 0
template <typename T> struct is_simple_task : std::false_type{};
template <typename T, typename P> struct is_priority_task : std::false_type{};

template <typename T> struct is_simple_task<simple_task<T>> : std::true_type{};
template <typename T, typename P> struct is_priority_task<priority_task<T,P>> : std::true_type{};
#endif

template<typename P>
struct FindTaskType_Impl;

template<typename P>
struct FindTaskType {
  typedef FindTaskType_Impl<std::remove_cvref_t<P>>::type type;
};

template<typename P>
using FindTaskType_t = typename FindTaskType<P>::type;

template<typename P>
struct FindTaskType_Impl<priority_task<P>> {
  typedef priority_task<P> type;
};

template<>
struct FindTaskType_Impl<simple_task> {
  typedef simple_task type;
};

/*
template<typename T, typename P>
struct FindTaskType_Impl<time_task<T,P>> {
  typedef time_task<T,P> type;
};

template<std::size_t N, typename T, typename P>
struct FindTaskType_Impl< time_series_task<N, T, P> > {
  typedef time_series_task<N,T,P> type;
};
*/

template<typename T>
struct FindTaskType_Impl<std::unique_ptr<T>> : FindTaskType_Impl<T> {};

template<typename T>
struct FindTaskType_Impl<std::shared_ptr<T>> : FindTaskType_Impl<T> {};

template<typename T>
struct FindTaskType_Impl<std::vector<T>> : FindTaskType_Impl<T> {};

} // namespace traits
} // namespace thp

#endif // TRAITS_HPP
