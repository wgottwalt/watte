Watte - weird and trivially tiny editor

This is an attempt to write a very small editor with no ugly dependencis (well,
except these libs/compilers which are part of nearly every Linux or BSD
distribution), very small binary size and a small footprint.

The first working attempt is Watte and only depends on ncurses and a C++17
compiler (can be changed quite easly to only need C++11).

To compile it I usually run this:
g++ -std=c++17 -W -Wall -Wextra -Os -s -o watte watte.cxx -lncurses
