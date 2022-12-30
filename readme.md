# Simple-forksrv-cov
This is the simplified forkserver and coverage intrument using SanitizeCoverage.
It is originated from AFL's LLVM mode.
I hope it will be useful to someone trying to implement something similar.

## Usage
```Bash
$ clang -c compiler-rt.c
$ ls
compiler-rt.o ...
$ clang -g -fsanitize-coverage=trace-pc-guard compiler-rt.o target.c -o target
$ ls
target compiler-rt.o ...
$ clang -o fuzz-tem fuzz-tem.c
$ ls
fuzz-tem target compiler-rt.o ...
$ ./fuzz-tem ./target
```