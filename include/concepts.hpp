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

#ifndef TP_CONCEPTS_HPP_
#define TP_CONCEPTS_HPP_

#include <concepts>

#include "include/executable.hpp"
#include "include/traits.hpp"

namespace thp {
namespace kncpt {

template <typename T>
concept SimpleTask = requires {
                       typename T::PriorityType;
                       requires std::same_as<typename T::PriorityType, void>;
                     };

template <typename T>
concept PriorityTask =
    requires {
      typename T::PriorityType;
      requires std::totally_ordered<typename T::PriorityType>;
    };

template <typename T>
concept ThreadPoolTask = PriorityTask<typename traits::FindTaskType<T>::type> ||
                         SimpleTask<typename traits::FindTaskType<T>::type>;

template <typename T>
concept ManageableThread =
    requires(T a) {
      typename T::id_type;

      { a.get_id() } -> std::same_as<typename T::id_type>;
      { a.joinable() } -> std::same_as<bool>;
      { a.join() } -> std::same_as<void>;
      { a.request_stop() } -> std::same_as<bool>;
    };

} // namespace kncpt
} // namespace thp

#endif // TP_CONCEPTS_HPP_
