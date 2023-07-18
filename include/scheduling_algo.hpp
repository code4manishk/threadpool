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

#ifndef SCHEDULER_HPP__
#define SCHEDULER_HPP__

#include <variant>

#include "include/traits.hpp"
#include "include/algos/scheduling/oneshot.hpp"
#include "include/algos/scheduling/waiting.hpp"

namespace thp {

namespace sch = algos::scheduler;

struct scheduling_algo {
    template<typename...Args>
    constexpr explicit scheduling_algo(Args&&... args)
    : active_algo_{std::in_place_type<sch::oneshot>, FWD(args)...} {}

    template<typename C>
    constexpr scheduling_algo& operator = (C&& t) {
        active_algo_ = std::move(t);
        return *this;
    }

    decltype(auto) scheduler_fn() {
        return std::visit([](auto&& v) noexcept { return v.scheduler_fn_; }, active_algo_);
    }

    decltype(auto) worker_fn() noexcept {
        return std::visit([](auto&& v) { return v.worker_fn_; }, active_algo_);
    }

protected:
    std::variant<sch::oneshot, sch::waiting> active_algo_;
};

}

#endif // SCHEDULER_HPP__
