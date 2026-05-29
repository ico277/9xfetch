MINGW_DIR = ./mingw32/

CC      = wine $(MINGW_DIR)/bin/i686-w64-mingw32-gcc.exe
#STRIP   = wine $(MINGW_DIR)/bin/i686-w64-mingw32-strip.exe

CFLAGS  = -std=gnu99 -O2 -Wall -Wl,--subsystem,console:4.10 -march=pentium
LDFLAGS = -static -static-libgcc

TARGET  = 9xfetch.exe
SRCS    = $(wildcard src/*.c)
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
#	$(STRIP) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	wine $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)