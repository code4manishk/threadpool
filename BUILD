load(":compilation.bzl", "cxx_flags", "link_flags")

cc_library(
  name = "lib_thp",
  srcs = glob(["src/**/*.cpp", "include/**/*.cpp"]),
  hdrs = glob(["include/**/*.hpp"]),
  copts = cxx_flags, # + ["-Iinclude/"],
  deps = ["//platform:th_config", "@glog//:glog",],
  linkopts = link_flags,
  visibility = ["//visibility:public"],
)

cc_library(
  name = "lib_algo_stl",
  srcs = glob(["stl/**/*.cpp"]),
  hdrs = glob(["stl/**/*.hpp"]),
  visibility = ["//visibility:public"],
  deps = ["//:lib_thp"],
  linkopts = ["-lpthread", ] + link_flags,
  copts = cxx_flags,
)

cc_binary(
  name = "threadpool",
  srcs = ["main.cc"],
  copts = cxx_flags,
  deps = [
          ":lib_thp",
          "@glog//:glog",
          "@yaml-cpp//:yaml-cpp",
         ],
  #malloc = "@com_google_tcmalloc//tcmalloc",
  visibility = ["//visibility:public"],
)

