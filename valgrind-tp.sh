bazel build examples:sort
#valgrind -s --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./bazel-bin/examples/sort
valgrind -s --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./bazel-bin/threadpool
#bazel run //examples:primes --copt="-g" --run_under='valgrind --tool=callgrind --callgrind-out-file=tp-callgrind --trace-children=yes'
