
ifeq "$(origin CC)" "default"
	CC = clang
	CXX = clang++
endif

PROGS = .compiler-rt.o .rt-clang-fast

all: $(PROGS)

.compiler-rt.o:
	$(CC) -c compiler-rt.c

.rt-clang-fast:
	$(CC) rs-clang-fast.c -o rs-clang-fast

clean:
	rm compiler-rt.o rs-clang-fast