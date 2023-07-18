#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <glog/logging.h>
#include <vector>
#include <map>
#include <ranges>
#include <locale>
#include <iomanip>
#include <execution>
#include <fstream>
#include <future>

#include "include/threadpool.hpp"
#include "include/clock_util.hpp"
#include "stl/stl_algo.hpp"

using namespace std;
using namespace thp;
using namespace chrono;

template<std::forward_iterator It>
void quick_sort(It s, It e) {
  const auto n = std::distance(s, e);
  if (n < 2) return;
  else {
    const auto p = *(e-1);
    auto m = std::stable_partition(s, e, [p](auto&& v) { return v < p; });
    if ((m == s) || (m == e)) {
      m = s+(n/2);
    }
    quick_sort(s, m);
    quick_sort(m, e);
    std::inplace_merge(s, m, e);
  }
}

int main(int argc, const char* const argv[]) {
  util::clock_util<chrono::steady_clock> cp;
  const unsigned  times = 100*1000000; // 100 million
  auto N = argc > 1 ? stoull(argv[1]) : times;
  auto workers = argc > 2 ? stoi(argv[2]) : std::thread::hardware_concurrency();
  bool use_stl = argc > 3 ? true : false;

  std::ios::sync_with_stdio(false);
  std::locale l("");
  std::locale::global(l);
  std::cout.imbue(l);

  try {
    std::random_device r;
    std::mt19937_64 e(r());
    std::uniform_int_distribution<char> dis('a', 'z');

    using value_type = typename decltype(dis)::result_type;

    thp::threadpool tp(workers);
    thp::stl_algo::api tp_algo(tp);

    std::cout << std::setw(14) << "size"
              << std::setw(14) << "time (ms)"
              << std::setw(14) << "stl(ms)"
              << std::setw(14) << "is_sorted" << std::endl;

    for(decltype(N) n = 10; n <= N; n *= 10) {
      std::vector<value_type> data;
      data.reserve(n);

      std::generate_n(std::back_inserter(data), n, [&] { return dis(e); });
      std::shuffle(data.begin(), data.end(), e);

      std::cout << std::right << std::setw(14) << data.size();

      cp.now();
      tp_algo.sort(data.begin(), data.end());
      cp.now();
      std::cout << std::setw(14) << cp.get_ms();

      if (use_stl) {
        std::shuffle(data.begin(), data.end(), e);
        cp.now();
        std::sort(std::execution::par, data.begin(), data.end());
        cp.now();

        std::cout << std::setw(14) << cp.get_ms();
        std::cout << std::setw(14) << std::boolalpha << std::ranges::is_sorted(data);
      }

      std::cerr << std::endl;
      //std::ranges::copy(data, std::ostream_iterator<std::decay_t<decltype(data.at(0))>>(std::cerr, ""));
    }
  } catch (exception &ex) {
    cerr << "main: Exception: " << ex.what() << endl;
  } catch (...) {
    cerr << "Exception: " << endl;
  }

  return 0;
}
