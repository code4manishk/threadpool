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

#ifndef MANAGED_STOP_TOKEN__
#define MANAGED_STOP_TOKEN__

#include <stop_token>
#include <memory>

#include "include/managed_stop_source.hpp"

namespace thp {

struct managed_stop_token : std::stop_token {
  explicit managed_stop_token(managed_stop_source& src)
  : std::stop_token{src.get_token()}
  , source{src.impl->clone()}
  {
    //std::cerr << "managed_stop_token(): " << source.get() << "," << src.get() << ", " << source.use_count() << std::endl;
  }

  managed_stop_token(const managed_stop_token&) noexcept = default;
  managed_stop_token& operator = (const managed_stop_token&) noexcept = default;

  managed_stop_token(managed_stop_token&& rhs) noexcept = default;
  managed_stop_token& operator = (managed_stop_token&& rhs) noexcept = default;

  friend std::ostream& operator << (std::ostream& oss, const managed_stop_token& tok) {
    oss << "managed_stop_token: " << tok.source.get() << "::" << tok.source.use_count();
    return oss;
  }

  [[nodiscard]]
  decltype(auto) current_state() const noexcept { return source->current_state(); }

  // [[nodiscard]]
  // bool stop_requested() const { return st.stop_requested(); }

  virtual ~managed_stop_token() = default;

protected:
  std::shared_ptr<managed_stop_source_impl> source;
};

} // namespace thp

#endif // MANAGED_STOP_TOKEN__
