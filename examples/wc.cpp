#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <set>
#include <vector>
#include <execution>
#include <sstream>
#include <glog/logging.h>

#include "include/threadpool.hpp"
#include "include/partitioner.hpp"
#include "include/clock_util.hpp"
#include "stl/stl_algo.hpp"

namespace fs = std::filesystem;
namespace rng = std::ranges;
namespace vw = std::ranges::views;

auto word_map = [](const std::string& file_path) {
    std::ifstream ifs{file_path};
    return std::accumulate(std::istream_iterator<std::string>(ifs),
                           std::istream_iterator<std::string>(),
                           std::unordered_map<std::string, unsigned>{},
                           [](auto&& mp, auto&& s) {
                                   mp[s] += 1;
                                   return std::move(mp);
                           });
};

template<std::input_iterator I, std::sentinel_for<I> S, 
        typename Size,
        typename Comp = std::greater<typename I::value_type>
>
decltype(auto) most_common(I s, S e, const Size n, Comp comp = Comp{})
{
  //using value_type = std::decay_t<typename Iter::value_type>;
  //static_assert(std::is_copy_assignable_v<value_type>, "data type is not copy assignable");
  using value_type = std::pair<std::string, unsigned>;

  std::vector<value_type> ans;
  ans.reserve(n);

  auto N = std::min(Size(std::distance(s, e)), n);

  std::copy_n(s, N, std::back_inserter(ans));
  std::make_heap(ans.begin(), ans.end(), comp);

  std::for_each(std::next(s, N), e, [&](const auto& p) mutable {
    if (comp(p, ans.front())) {
      std::pop_heap(ans.begin(), ans.end(), comp);
      ans.back() = p;
      std::push_heap(ans.begin(), ans.end(), comp);
    }
  });

  std::sort_heap(ans.begin(), ans.end(), comp);
  return ans;
}

int main(int argc, const char* const argv[])
{
  try {
    // Initialize Google's logging library.
    //google::InitGoogleLogging(argv[0]);

    if (argc < 4) {
      std::cerr << "usage: word_count /path/to/directory top_n file_extensions...\n"
                << "e.g. word_count /tmp/ 10 .txt .cpp .py" << std::endl;
      return 1;
    }

    thp::util::system_clock cu;
    fs::path dir_path(argv[1]);
    unsigned topn = std::stoi(argv[2]);
    std::set<std::string> ext{argv+3, argv+argc};

    auto matches = [&] (auto&& entry) {
      return entry.exists() && entry.is_regular_file()
           && (ext.contains(entry.path().extension()));
    };
    auto path = [] (auto&& e) { return e.path(); };

    auto table_update = [](auto&& tot, auto&& val) {
                           tot.merge(std::move(val));
                           return std::move(tot); 
                        };

    auto pair_comp = [](auto&& a, auto&& b) { return a.second > b.second; };

    std::vector<std::string> file_paths;
    std::vector<fs::directory_entry> entries{fs::recursive_directory_iterator(dir_path), fs::recursive_directory_iterator()};
    rng::copy(entries|vw::filter(matches)|vw::transform(path), std::back_inserter(file_paths));

    auto print_result = [&](const std::string& profile,
                            const std::unordered_map<std::string,unsigned>& ans,
                            std::ostream& oss = std::cerr) {
        auto top_n = most_common(ans.begin(), ans.end(), topn, pair_comp);
        oss << profile << "(" << file_paths.size() << "): " << cu.get_ms() << " ms" << ", ans = " << ans.size() << std::endl;
        for (auto&& [k, v] : top_n)
            oss << v << " : " << k << std::endl;
    };

    {
      thp::threadpool tp;
      thp::stl_algo::api tp_algo(tp);
      cu.now();
      auto total_files = file_paths.size();
      auto N = std::thread::hardware_concurrency();
      auto per_worker_size = total_files%N == 0 ? total_files/N : (total_files/(N-1));
      auto data_partitioner = thp::algos::partitioner::equal_size(per_worker_size, file_paths.begin(), file_paths.end());
      auto f = tp_algo.transform_reduce(file_paths.begin(), file_paths.end(),
                                     std::unordered_map<std::string, unsigned>{},
                                     table_update,
                                     word_map,
                                     data_partitioner
                                    );
      auto ans = f.get();
      cu.now();
      print_result("thp", ans);
    }
    {
      cu.now();
      auto ans = std::transform_reduce(std::execution::par,
                            file_paths.begin(), file_paths.end(),
                            std::unordered_map<std::string, unsigned>{},
                            table_update,
                            word_map);
      cu.now();
      print_result("std", ans);
    }
  } catch(std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  return 0;
}

