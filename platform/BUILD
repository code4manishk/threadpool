load("//:compilation.bzl", "copt_flags", "stdc")
load("//:compilation.bzl", "link_flags")

cc_library(
  name = "th_config",
  srcs = ["thread_config.c"],
  hdrs = ["thread_config.h"] + glob(["*.hpp"]),
  visibility = ["//visibility:public"],
  linkopts = ["-lpthread", ] + link_flags,
  copts = copt_flags + stdc,
)

