#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <glog/logging.h>
#include <vector>
#include <map>
#include <ranges>
#include <cmath>
#include <iterator>
#include <sstream>
#include <bitset>
#include <cfenv>
#include <cassert>

#include "include/threadpool.hpp"
#include "include/clock_util.hpp"
#include "include/coroutine/task.hpp"

using namespace std;
using namespace thp;
using namespace chrono;
namespace rng = std::ranges;
namespace vw = std::ranges::views;

template<std::integral T, std::size_t N>
struct PrimeBits {
  T start, end;
  bitset<N> bits;

  template<typename V>
  struct iterator {
    V start;
    size_t pos;
    const bitset<N>& data;

    constexpr explicit iterator(const bitset<N>& d, V s, size_t p = 0)
    : start{s}
    , pos{p}
    , data{d}
    {}

    constexpr iterator& operator ++() { ++pos; return *this; }
    constexpr iterator& operator++(size_t) { pos++; return *this; }
    constexpr decltype(auto) operator * () { return data[pos]; }
    friend bool operator == (iterator a, iterator b) {
      return (a.pos == b.pos) && (addressof(a.data) == addressof(b.data)) && (a.start == b.start);
    }
  };

public:
    constexpr explicit PrimeBits() = default;
    constexpr explicit PrimeBits(T s, T e)
    : start{s}
    , end{e}
    , bits()
    {
      bits.set();
      for(auto i = e; i < s+N; ++i)
        bits.set(i-start, false);
    }

    constexpr PrimeBits(PrimeBits&&) noexcept = default;
    constexpr PrimeBits& operator = (PrimeBits&&) noexcept = default;
    constexpr PrimeBits& operator = (const PrimeBits&) = delete;
    constexpr PrimeBits(const PrimeBits&) = delete;
    constexpr ~PrimeBits() = default;

    constexpr decltype(auto) count() const noexcept { return bits.count(); }

    template<typename OIter>
    constexpr decltype(auto) copy_to(OIter iter) const {
      auto val = views::iota(start, end) | views::filter([&](auto&& x) { return test(x-start); });
      return rng::copy(val, iter);
    }

    constexpr void cross_non_primes(const T p) {
      uint64_t i = p;
      i *= p;

      if (start > p)
        i = (start % p == 0) ? start : p*(1 + start/p);

      for(auto j = i-start; j < N; j += p)
        bits[j] = false;
    }

    constexpr decltype(auto) set(size_t pos)        { return bits.set(pos);  }
    constexpr decltype(auto) test(size_t pos) const { return bits.test(pos); }

    friend ostream& operator << (ostream& oss, const PrimeBits<T,N>& pb) {
      oss << '[' << pb.start << ' ' << pb.end << ") :" << pb.count();
      return oss;
    }
};

template<std::integral T, std::size_t N>
constexpr PrimeBits<T, N> find_prime(const vector<uint32_t>& small_primes, const T start, const T end) {

  PrimeBits<T, N> primes{start, end};

  const auto sq = static_cast<T>(1+sqrt(end));
  auto less_than = [](const auto pp) { return [pp] (auto x) { return x < pp; }; };

  rng::for_each(small_primes | vw::take_while(less_than(sq)), [&](auto&& p) { primes.cross_non_primes(p); });

  return primes;
}

constexpr auto seq_prime(const integral auto N) {
    using T = decay_t<decltype(N)>;
    vector<T> prime_vec;
    //prime_vec.reserve(ceil((1.25506f*N)/log(N)));

    auto completely_divides = [](const auto pp) { return [pp](auto x) { return pp%x == 0; }; };
    auto primes = [&] (const T n) {
        auto factors = prime_vec | views::take_while([sq = static_cast<T>(sqrt(n))+1](auto&& p) { return p < sq; });
        return rng::none_of(factors , completely_divides(n));
    };
    auto prime_nums = views::iota(2u, N+1) | views::filter(primes);
    rng::copy(prime_nums, back_inserter(prime_vec));
    return prime_vec;
}

template<std::integral T, std::size_t N>
thp::generator<PrimeBits<T, N>> lazy_prime(thp::threadpool& tp, const T n) {
  using ReturnType = PrimeBits<T, N>;

  static_assert(N < numeric_limits<uint32_t>::max());

  uint64_t nn(n), max32 = numeric_limits<uint32_t>::max();
  auto step = N;
  auto smalls = seq_prime(static_cast<uint32_t>(1+sqrt(max32)));
  auto sieve_bits = find_prime<T, N>(cref(smalls), 2, step);

  if (nn >= max32) {
    // increase seive
    auto bigs = smalls;
    const auto seive_limits = static_cast<T>(sqrt(numeric_limits<uint64_t>::max()));
    vector<std::future<ReturnType>> futs;
    for(T i = 1+smalls.back(); i <= seive_limits; i += step)
      futs.emplace_back(tp.submit(find_prime<T, N>, cref(smalls), i, min(i+step, seive_limits)));

    rng::for_each(futs, [&](auto&& fut) { fut.get().copy_to(back_inserter(bigs)); });

    exchange(smalls, move(bigs));
  }

  co_yield move(sieve_bits);
  vector<std::future<ReturnType>> futs;
  for (T i = step; i < n; i += step)
    futs.emplace_back(tp.submit(find_prime<T, N>, cref(smalls), i, min(i+step, n)));

  for(auto&& v : futs)
    co_yield move(v.get());
}

int main(int argc, const char* const argv[]) {
  constexpr const unsigned long N = 256*8*1024u;
  auto n = argc > 1 ? stoul(argv[1]) : 1000*1000*1000u;
  auto workers = argc > 2 ? stoi(argv[2]) : thread::hardware_concurrency();

  n = max(n, N);

  util::clock_util<chrono::high_resolution_clock> cp;
  ostream& oss = cout;
  auto lcl = locale("");
  locale::global(lcl);
  oss.imbue(lcl);
  unsigned num_primes = 0;

  thp::threadpool tp(workers);
  cp.now();
  for(auto&& prime_bits : lazy_prime<decltype(n), N>(tp, n))
      num_primes += prime_bits.count();
  cp.now();
  oss << "total primes: (" << n << ") : " << num_primes << "\n";
  oss << "thp: " << cp.get_ms() << " ms" << endl;
  return 0;
}
