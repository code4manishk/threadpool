#sudo dnf install gprof2dot graphviz
bazel build //examples:primes   --copt="-pg" --copt="-no-pie" --linkopt="-pg" --linkopt="-no-pie"
bazel-bin/examples/primes
gprof -a bazel-bin/examples/primes gmon.out > analysis.txt
#cat analysis.txt
gprof2dot -w -s analysis.txt | dot -Tpng -o output.png
#gprof bazel-bin/examples/reduce | gprof2dot -w -s | dot -Tpng -o output.png
