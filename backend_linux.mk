CC      = gcc
CFLAGS = -O2

_CFLAGS  = -std=gnu99 -Wall -D_9XFETCH_BACKEND_LINUX $(CFLAGS)
_LDFLAGS = $(LDFLAGS)

TARGET  = 9xfetch.out
SRCS    = $(wildcard src/*.c) $(wildcard src/linux/*.c)
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(_LDFLAGS)
#	$(STRIP) $(TARGET)

%.o: %.c
	$(CC) $(_CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)