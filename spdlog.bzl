spdlog_build_cmd="""
#!/bin/bash
echo $(pwd)
git clone https://github.com/gabime/spdlog.git /home/xps38/workspace/github/threadpool/external/spdlog
cd /home/xps38/workspace/github/threadpool/external/spdlog
mkdir build
cd build
cmake ../
make -j
"""

FOO_BUILD="""
cc_library(
    name = "bar",
    srcs = ["bar.a"],
    hdrs = glob(["*.h"]),
)
"""

def _spdlog_repository_impl(ctx):
  #workspace_dir = ctx.path(ctx.attr.file_in_project).dirname
  ctx.file("spdlog_build.sh", content=spdlog_build_cmd)
  #ctx.file("BUILD", content=FOO_BUILD, executable=False)
  ctx.report_progress("running spdlog build fetch and build")
  ctx.execute(["./spdlog_build.sh"])

spdlog_repo = repository_rule(
  implementation = _spdlog_repository_impl,
  environ = ["CC", "CXX", "LD_LIBRARY_PATH"],
  local = True,
)
