load("//:compilation.bzl", "cxx_flags", "link_flags")

cc_binary(
  name = "primes",
  srcs  = ["primes.cc"],
  copts = cxx_flags,
  deps = ["//:lib_thp"],
  #malloc = "@com_google_tcmalloc//tcmalloc",
  linkopts = link_flags,
  visibility = ["//visibility:public"],
)

#cc_binary(
#  name = "usage",
#  srcs  = ["usage.cpp"],
#  copts = cxx_flags,
#  deps = ["//:lib_thp"],
#  linkopts = link_flags,
#  visibility = ["//visibility:public"],
#)

cc_binary(
  name = "reduce",
  srcs  = ["reduce.cpp"],
  copts = cxx_flags,
  deps = ["//:lib_algo_stl"],
  linkopts = link_flags,
  #malloc = "@com_google_tcmalloc//tcmalloc",
  visibility = ["//visibility:public"],
  #linkstatic = True,
)

cc_binary(
  name = "word_count",
  srcs  = ["wc.cpp"],
  copts = cxx_flags,
  deps = ["//:lib_algo_stl"],
  #malloc = "@com_google_tcmalloc//tcmalloc",
  linkopts = link_flags + ["-lstdc++fs"],
  visibility = ["//visibility:public"],
)

cc_binary(
  name = "sort",
  srcs = ["sort.cpp"],
  copts = cxx_flags + ["-fopenmp"],
  deps = ["//:lib_algo_stl"],
  linkopts = link_flags,
  #malloc = "@com_google_tcmalloc//tcmalloc",
  visibility = ["//visibility:public"],
)
