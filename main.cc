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

#include <iostream>
#include <string>
#include <chrono>
#include <numeric>
#include <fstream>
#include <forward_list>
#include <ranges>
#include <iterator>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <glog/stl_logging.h>

#include "spdlog/spdlog.h"

#include "include/partitioner.hpp"
#include "include/task_type.hpp"
#include "include/threadpool.hpp"

using namespace std;
using namespace std::chrono;
namespace rng = std::ranges;
namespace vw = std::ranges::views;

auto delay_fn = [] (auto&& x) {
  auto y = steady_clock::now();
  return duration_cast<microseconds>(y-x).count();
};

auto throw_fn = [](auto&& x) {
  auto y = steady_clock::now();
  std::ostringstream oss;
  auto v = duration_cast<microseconds>(y-x).count();
  oss << "throw_fn: exception after: " << v << "us\n";
  if (v > 100) throw std::runtime_error(oss.str());
  return v;
};

template<typename T>
void print_stats(T& vals, std::ostream& oss) {
  try {
    auto n = vals.size();
    vector<long int> data(n, 0);
    rng::transform(vals, data.begin(), [](auto&& f) { return f.get(); });
    uint64_t total = std::accumulate(data.cbegin(), data.cend(), uint64_t(0));
    auto [mn, mx] = rng::minmax_element(data);
    oss << "# mean: " << total/n << " us\n";
    oss << "# min:  " << *mn << '\n';
    oss << "# max:  " << *mx << '\n';
    oss << "# at:   " << distance(data.begin(), mn) << ", " << distance(data.begin(), mx) << '\n';
    rng::copy(data, ostream_iterator<long int>(oss, "\n"));
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

void system_async(size_t n, std::ostream& oss) {
  std::vector<std::future<long int>> vals;
  vals.reserve(n);

  for(size_t i = 0; i < n; ++i) {
    vals.emplace_back(std::async(std::launch::async, throw_fn, steady_clock::now()));
  }
  print_stats(vals, oss);
}

std::optional<size_t> is_prime(size_t n) {
  if (n < 2) return std::nullopt;

  auto m = sqrt(n);
  for(auto i = 2; i <= m; ++i)
    if (n%i == 0) return {};

  return {n};
}

void tp_schedule(size_t n, size_t w, std::ostream& oss) {
   thp::threadpool tp(w);
   std::vector<std::future<long int>> futs;
   futs.reserve(n);

   std::generate_n(std::back_inserter(futs), n, [&] {
     return tp.submit(delay_fn, steady_clock::now());
   });

   print_stats(futs, oss);
}

void tp_map(int n, size_t w, unsigned chunk) {
  thp::threadpool tp(w);
  //auto sq = [] (std::integral auto x) { return x*x; };
  //auto printx = [](auto&& x) { cout << x << '\n'; };

  //auto gen_sq = tp.map(sq, vw::iota(0, n));
  //rng::for_each(gen_sq.begin(), gen_sq.end(), printx);
  //for(auto it = gen_sq.begin(); it != gen_sq.end(); ++it) printx(*it);

  //std::forward_list<int> data{0,1,3,4,5,6,7};
  //rng::copy(vw::iota(0,25), std::insert_iterator(l, l.begin()));
  std::cout << "primes till " << n << " with chunksize " << chunk << "\n";
  // works with range algorithms
  //auto gen = tp.map(is_prime, vw::iota(2, n), chunk);
  //auto primes = gen
  //              | vw::filter([](auto&& o) { return o.has_value(); })
  //              | vw::transform([](auto&& o) { return o.value(); });
  //rng::copy(primes, std::ostream_iterator<int>(cout, "\n"));

  for(auto&& v : tp.map(is_prime, vw::iota(0, n), chunk)) {
    if (v) std::cout << *v << '\n';
  }
}

int collect_stats(int argc, const char* const argv[])
{
  try {
    auto n = argc > 1 ? stoi(argv[1]) : 1024;
    auto w = argc > 2 ? stoi(argv[2]) : std::thread::hardware_concurrency();
    auto filename = argc > 3 ? std::string(argv[3]) : std::string("data.txt");
    auto write_file = argc > 4 ? stoi(argv[4]) : 1;

    std::stringstream oss;

    std::cout << "# system_async(" << n << ", " << w << ")\n";
    system_async(n, oss);

    oss << "# tp_schedule(" << n << ", " << w << ")\n";
    tp_schedule(n, w, oss);
  
    copy(istream_iterator<string>(oss), istream_iterator<string>(),
         ostream_iterator<string>(cout, "\n"));

    if (write_file) {
      std::cout << oss.str();
      std::fstream f(filename, ios::out);
      if (!f.is_open()) {
        spdlog::info("failed to open {}", filename);
      } else {
        f << oss.str();
        f.close();
      }
    }
  } catch(std::exception& e) {
    std::cerr << (e.what());
  }

  return 0;
}

int main(int argc, const char* const argv[]) {
  try {
    auto n = argc > 1 ? stoi(argv[1]) : 42;
    auto w = argc > 2 ? stoi(argv[2]) : std::thread::hardware_concurrency();
    auto c = argc > 3 ? stoi(argv[3]) : 37;
    tp_map(n, w, c);
  } catch(std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}
