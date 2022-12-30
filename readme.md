# Simple-forksrv-cov
This is the simplified forkserver and coverage intrument using SanitizeCoverage.
It is originated from AFL's LLVM mode.
I hope it will be useful to someone trying to implement something similar.

## Usage
```Shell
$ make
$ ls
compiler-rt.o rs-clang-fast ...
$ ./rs-clang-fast target.c -o target
```