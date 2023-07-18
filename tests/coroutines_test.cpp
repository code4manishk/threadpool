#include "gtest/gtest.h"
#include "include/coroutine/task.hpp"

namespace {

template<typename R>
using coro_task = thp::coro::task<R, thp::coro::continuation_promise<int>>;

coro_task<int> get_int() {
  co_return 42;
}

coro_task<int> add_fun(int x) {
  auto v1 = get_int();
  auto v2 = get_int();

  co_return (x + co_await v1 + co_await v2);
}

TEST(CoroutinesTasks, continucation_calls) {
  auto t = add_fun(-42);
  EXPECT_EQ(t(), 42);
}

TEST(CoroutineTasks, single_call) {
  auto t = get_int();
  EXPECT_EQ(t(), 42);
}

}
