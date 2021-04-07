CC = gcc

CFLAGS = -std=c11 -flto -fPIC -W -Wall -Wextra -Os -s
LDFLAGS = -lncurses

OBJ = string.o stringlist.o support.o watte2.o

all: watte2

watte2: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJ) watte2
