
link_flags = ["-lpthread"]

thread_sanitizer_flags = ["-fsanitize=thread", "-pie", "-fPIE",]

warning_flags = ["-Wall", "-Wextra"]
optimize_flags = ["-O3"]
debug_flags = ["-fno-omit-frame-pointer"]
copt_flags = warning_flags + optimize_flags
stdc = ["--std=c17"]
stdcxx = ["--std=c++23", "-Wno-interference-size"]
cxx_flags = copt_flags + stdcxx + ["-fconcepts", "-fcoroutines"]
