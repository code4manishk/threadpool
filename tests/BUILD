load("//:compilation.bzl", "cxx_flags", "link_flags")

cc_test(
  name = "task_types",
  srcs = ["priority_task_test.cpp"],
  deps = [
        "//:lib_thp",
        "@gtest//:gtest",
        "@gtest//:gtest_main",
  ],
  copts = cxx_flags,
  linkopts = link_flags,
  visibility = ["//visibility:__subpackages__"], 
)

cc_test(
  name = "coroutines",
  srcs = ["coroutines_test.cpp"],
  deps = [
        "//:lib_thp",
        "@gtest//:gtest",
        "@gtest//:gtest_main",
  ],
  copts = cxx_flags,
  linkopts = link_flags,
)
