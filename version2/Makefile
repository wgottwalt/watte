CC = gcc

CFLAGS = -std=c11 -flto -fPIC -W -Wall -Wextra -Os -s
LDFLAGS = -lncurses

TARGET = watte

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	$(RM) $(TARGET)
