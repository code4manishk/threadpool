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

#include <chrono>

#include "gtest/gtest.h"
#include "include/task_type.hpp"
#include "include/task_queue.hpp"
#include "include/task_factory.hpp"
#include "include/work_queue.hpp"

namespace {

using namespace std;

unsigned factorial(unsigned n){
  if (n < 2) return 1;
  else return n*factorial(n-1);
}

TEST(PriorityTaskTest, order) {
  using namespace std::chrono_literals;
  using namespace std::chrono;

  auto t1 = thp::make_task<int>(factorial, 10);
  auto t2 = thp::make_task<int>(factorial, 0);
  auto t3 = thp::make_task<int>(factorial, 10);

  t1.priority(10);
  t2.priority(0);
  t3.priority(10);

  EXPECT_TRUE(t2 < t1);
  EXPECT_FALSE(t2 > t1);
  EXPECT_FALSE(t3 < t1);
}

TEST(WorkQueue, order) {
  //thp::ds::priority_workq<thp::priority_task<unsigned int, int>, int, std::less<thp::priority_task<unsigned int, int>>> q;
  thp::ds::priority_workq<thp::priority_task<int>, std::less<thp::priority_task<int>>> q;
  auto t1 = thp::make_task<int>(factorial, 1);
  auto t2 = thp::make_task<int>(factorial, 2);
  auto t3 = thp::make_task<int>(factorial, 3);

  t1.priority(1);
  t2.priority(2);
  t3.priority(3);

  q.push(move(t1));
  q.push(move(t2));
  q.push(move(t3));

  auto x = q.pop();
  if (x) {
    EXPECT_EQ(x.value().priority(), 3);
  }
}

// TEST(PriorityTaskTest, qorder) {
//   auto t1 = thp::simple_task<int>(factorial, 1);
//   auto t2 = thp::simple_task<int>(factorial, 2);
//   auto t3 = thp::simple_task<int>(factorial, 3);
//   auto f1 = t1.future();
//   auto f2 = t2.future();
//   auto f3 = t3.future();
//   t1.priority(1);
//   t2.priority(2);
//   t3.priority(3);

//   EXPECT_EQ(t3.priority(), 3);

//   thp::priority_taskq<int> q;
//   q.insert(move(t1),move(t2),move(t3));

//   EXPECT_EQ(q.size(), 3);

//   auto x1 = std::shared_ptr<thp::executable>(q.pop());
//   auto x2 = std::shared_ptr<thp::executable>(q.pop());
//   auto x3 = std::shared_ptr<thp::executable>(q.pop());
  
//   auto w1 = std::dynamic_pointer_cast<thp::comparable_task<int>>(x1);
//   auto w2 = std::dynamic_pointer_cast<thp::comparable_task<int>>(x2);
//   auto w3 = std::dynamic_pointer_cast<thp::comparable_task<int>>(x3);

//   EXPECT_EQ(w1->priority(), 3);
//   EXPECT_EQ(w2->priority(), 2);
//   EXPECT_EQ(w3->priority(), 1);

//   EXPECT_EQ(w1->priority(), 3);
//   w1->execute();
//   EXPECT_TRUE(f3.valid());
//   EXPECT_EQ(6, f3.get());
// }

} // namespace
