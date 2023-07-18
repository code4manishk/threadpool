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

#ifndef REGISTER_TYPES_HPP__
#define REGISTER_TYPES_HPP__

#include <tuple>
#include <type_traits>

namespace thp {
namespace compile_time {

template<typename T, typename Tup, std::size_t I = 0>
constexpr std::size_t find() {
  //static_assert(I >= std::tuple_size_v<Tup>, "T is not in Tup");
  if constexpr (I >= std::tuple_size_v<Tup>) return I;
  else if constexpr (std::is_same_v<T, std::tuple_element_t<I, Tup>>) return I;
  else
    return find<T, Tup, I+1>();
}

template<typename T, typename Tup>
constexpr bool exists_in() {
  return find<T, Tup>() < std::tuple_size_v<Tup>;
}

} // namespace compile_time
} // namespace thp


#endif // REGISTER_TYPES_HPP__
