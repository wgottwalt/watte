Watte - weird and trivially tiny editor

This is an attempt to write a very small editor with no ugly dependencis (well,
except these libs/compilers which are part of nearly every Linux or BSD
distribution), very small binary size and a small footprint.

version1:

The first working attempt is Watte and only depends on ncurses and a C++17
compiler (can be changed quite easily to only need C++11).

To compile it I usually run this:
g++ -std=c++17 -flto -W -Wall -Wextra -Os -s -o watte watte.cxx -lncurses

You can also use the good old gnu make:
make

Or even compile it using Zig:
zig build --release=small
zig build run --summary all (to run it directly via Zig)

version2: (not complete yet)

This version is version1 but completely done in C11. This is ment for static
compiling. You should use lib musl for that to get some really impressive
results (small size for a static binary).

To compile it I usually run this:
musl-gcc -std=c11 -flto -W -Wall -Wextra -Os -s -o watte watte.c
