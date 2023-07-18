workspace(
  name = "threadpool"
)

load("@bazel_tools//tools/build_defs/repo:git.bzl",  "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
#load("@bazel_skylib//lib:version.bzl", "versions")

git_repository(
     name = "gtest",
     remote = "https://github.com/google/googletest",
     branch = "v1.12.x",
)

http_archive(
    name = "com_github_gflags_gflags",
    strip_prefix = "gflags-2.2.2",
    urls = [
        "https://mirror.bazel.build/github.com/gflags/gflags/archive/v2.2.2.tar.gz",
        "https://github.com/gflags/gflags/archive/v2.2.2.tar.gz",
    ],
    sha256= "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
)

http_archive(
    name = "rules_cc",
    urls = ["https://github.com/bazelbuild/rules_cc/releases/download/0.0.1/rules_cc-0.0.1.tar.gz"],
    sha256 = "4dccbfd22c0def164c8f47458bd50e0c7148f3d92002cdb459c2a96a68498241",
)

http_archive(
    name = "com_google_googletest",  # 2021-07-09T13:28:13Z
    sha256 = "12ef65654dc01ab40f6f33f9d02c04f2097d2cd9fbe48dc6001b29543583b0ad",
    strip_prefix = "googletest-8d51ffdfab10b3fba636ae69bc03da4b54f8c235",
    # Keep this URL in sync with ABSL_GOOGLETEST_COMMIT in ci/cmake_common.sh.
    urls = ["https://github.com/google/googletest/archive/8d51ffdfab10b3fba636ae69bc03da4b54f8c235.zip"],
)

# Google benchmark.
http_archive(
    name = "com_github_google_benchmark",  # 2021-09-20T09:19:51Z
    sha256 = "62e2f2e6d8a744d67e4bbc212fcfd06647080de4253c97ad5c6749e09faf2cb0",
    strip_prefix = "benchmark-0baacde3618ca617da95375e0af13ce1baadea47",
    urls = ["https://github.com/google/benchmark/archive/0baacde3618ca617da95375e0af13ce1baadea47.zip"],
)

# Bazel platform rules.
http_archive(
    name = "platforms",
    sha256 = "b601beaf841244de5c5a50d2b2eddd34839788000fa1be4260ce6603ca0d8eb7",
    strip_prefix = "platforms-98939346da932eef0b54cf808622f5bb0928f00b",
    urls = ["https://github.com/bazelbuild/platforms/archive/98939346da932eef0b54cf808622f5bb0928f00b.zip"],
)

http_archive(
  name = "com_google_absl",
  urls = ["https://github.com/abseil/abseil-cpp/archive/0c6302fe427963ec5c471d3ee660120682ab15f7.zip"],  # 2020-02-11T18:50:53Z
  strip_prefix = "abseil-cpp-0c6302fe427963ec5c471d3ee660120682ab15f7",
  sha256 = "acf23b75324e97152f804f1d7a4961ff7336de6d01ba9506b7c009d22defe091",
)

git_repository(
  name = "glog",
  commit = "96a2f23dca4cc7180821ca5f32e526314395d26a",
  remote = "https://github.com/google/glog.git",
  shallow_since = "1553223106 +0900",
)

git_repository(
  name = "yaml-cpp",
  remote = "https://github.com/jbeder/yaml-cpp.git",
  commit = "b2f89386d8f88655e47c4be0c719073dd6308a21",
  shallow_since = "1583935156 -0500",
)

new_local_repository(
  name = "spdlog",
  path = "external/spdlog/",
  build_file = "external/spdlog.BUILD"
)

git_repository(
  name = "com_google_tcmalloc",
  remote = "https://github.com/google/tcmalloc.git",
  branch = "master",
)

# Google benchmark.
http_archive(
    name = "com_github_google_benchmark",
    urls = ["https://github.com/google/benchmark/archive/0baacde3618ca617da95375e0af13ce1baadea47.zip"],
    strip_prefix = "benchmark-0baacde3618ca617da95375e0af13ce1baadea47",
    sha256 = "62e2f2e6d8a744d67e4bbc212fcfd06647080de4253c97ad5c6749e09faf2cb0",
)

# C++ rules for Bazel.
http_archive(
    name = "rules_cc",  # 2021-05-14T14:51:14Z
    urls = ["https://github.com/bazelbuild/rules_cc/archive/68cb652a71e7e7e2858c50593e5a9e3b94e5b9a9.zip"],
    strip_prefix = "rules_cc-68cb652a71e7e7e2858c50593e5a9e3b94e5b9a9",
    sha256 = "1e19e9a3bc3d4ee91d7fcad00653485ee6c798efbbf9588d40b34cbfbded143d",
)

# Python rules
#
# This is explicitly added to workaround
# https://github.com/bazelbuild/rules_python/issues/437.
http_archive(
    name = "rules_python",
    urls = ["https://github.com/bazelbuild/rules_python/releases/download/0.1.0/rules_python-0.1.0.tar.gz"],
    sha256 = "b6d46438523a3ec0f3cead544190ee13223a52f6a6765a29eae7b7cc24cc83a0",
)

# Proto rules for Bazel and Protobuf
http_archive(
    name = "com_google_protobuf",
    urls = ["https://github.com/protocolbuffers/protobuf/archive/13d559beb6967033a467a7517c35d8ad970f8afb.zip"],
    strip_prefix = "protobuf-13d559beb6967033a467a7517c35d8ad970f8afb",
    sha256 = "9ca59193fcfe52c54e4c2b4584770acd1a6528fc35efad363f8513c224490c50",
)
load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")
protobuf_deps()

http_archive(
    name = "rules_proto",
    sha256 = "66bfdf8782796239d3875d37e7de19b1d94301e8972b3cbd2446b332429b4df1",
    strip_prefix = "rules_proto-4.0.0",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_proto/archive/refs/tags/4.0.0.tar.gz",
        "https://github.com/bazelbuild/rules_proto/archive/refs/tags/4.0.0.tar.gz",
    ],
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

# Fuzzing
http_archive(
    name = "rules_fuzzing",
    sha256 = "a5734cb42b1b69395c57e0bbd32ade394d5c3d6afbfe782b24816a96da24660d",
    strip_prefix = "rules_fuzzing-0.1.1",
    urls = ["https://github.com/bazelbuild/rules_fuzzing/archive/v0.1.1.zip"],
)

# Protobuf
load("@rules_fuzzing//fuzzing:repositories.bzl", "rules_fuzzing_dependencies")

rules_fuzzing_dependencies()

load("@rules_fuzzing//fuzzing:init.bzl", "rules_fuzzing_init")

rules_fuzzing_init()

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

_RULES_BOOST_COMMIT = "652b21e35e4eeed5579e696da0facbe8dba52b1f"

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "c1b8b2adc3b4201683cf94dda7eef3fc0f4f4c0ea5caa3ed3feffe07e1fb5b15",
    strip_prefix = "rules_boost-%s" % _RULES_BOOST_COMMIT,
    urls = [
        "https://github.com/nelhage/rules_boost/archive/%s.tar.gz" % _RULES_BOOST_COMMIT,
    ],
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()
