# Workflow
A high throughput threadpool implementation with configurable task queues, queue selection, task scheduling, and data partitioning options.

# Basics
  Consists of tasks queues and configurable workerpool. Worker threads can be configured with platform specific parameters, and can be started to run as part of specific executor pool e.g. worker pool of cuda threads (todo).
  Scheduling algorithms can be extended to allow best selection of queues for workers to work on. In future, these functions will be coroutines which could be migrated across threads. 
  Tasks are spread across task queues based on task's type, e.g. simple_task, priority_task. These queues are configured with either task-steal mode or oneshot mode.

## Scheduling algorithm

  One can add or extend existing algorithms to allow for proper queue selections, currently its maxlen algorithm which selects queue with largest numers of tasks.
  Scheduling is currently oneshot, and new algorithm can be added to allow for waiting versions for long awaiting tasks.

# Task type 
  There are two class of tasks, simple_task and priority_task. Priority tasks are arranged by its priority and executed accordingly,
  subjected to scheduling delays of the platform.
 ```
  e.g. simple_task
       priority_task<PriorityType>
```
  There are convenient APIs to create task, e.g.
```
  auto p0 = thp::make_tasks(print_prime, 42); // simple_task<int>
  auto p1 = thp::make_task<float>(print_prime, 42).priority(4.2f); // priority_task<int, float>
```

# Build
  Threadpool uses bazel to build workspace and maintain external depenencies, e.g. gtest, spdlog, and requires c++20 support.
  Additionally, see benchmarks in examples directory.

```
  git clone https://github.com/manish-kumar1/threadpool.git
  cd threadpool
  bazel build examples:all
  bazel run examples:sort
```
  
# example Usage
  simple usage example with ranges:<br>
```
  namespace vw = std::ranges::views;

  auto gen = tp.map(is_prime, vw::iota(2, n));
  auto primes = gen
                | vw::filter([](auto&& o) { return o.has_value(); })
                | vw::transform([](auto&& o) { return o.value(); });

```
  There are multiple examples of usage and benchmark in examples directory for reference.

