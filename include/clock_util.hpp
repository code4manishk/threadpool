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

#ifndef CLOCK_UTIL_HPP
#define CLOCK_UTIL_HPP

#include <chrono>
#include <ratio>

namespace thp {
namespace util {

template <typename Clock>
class clock_util {
public:
  constexpr explicit clock_util() : points{}, s{&points[0]}, e{&points[1]}, tmp{&points[2]} {
    *s = Clock::now();
    *e = Clock::now();
  }

  constexpr decltype(auto) now() noexcept {
    *tmp = Clock::now();
    std::swap(s, e);
    std::swap(tmp, e);
    return *this;
  }

  template<typename T, typename C>
  inline constexpr std::chrono::duration<T, C> last_dur() const {
    return (*e - *s);
  }

  constexpr double get_us() noexcept {
    std::chrono::duration<double, std::micro> tot = *e - *s;
    return tot.count();
  }

  constexpr double get_ms() noexcept {
    return get_us()/1000.0f;
  }

  constexpr double get_sec() noexcept {
    return get_ms()/1000.0f;
  }

private:
  std::array<typename Clock::time_point, 3> points;
  typename Clock::time_point *s, *e, *tmp;
};

using system_clock = clock_util<std::chrono::system_clock>;
using steady_clock = clock_util<std::chrono::steady_clock>;

} // namespace util
} // namespace thp

#endif // CLOCK_UTIL_HPP
