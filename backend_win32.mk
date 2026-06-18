MINGW_DIR = ./mingw32/

CC      = wine $(MINGW_DIR)/bin/i686-w64-mingw32-gcc.exe
#STRIP   = wine $(MINGW_DIR)/bin/i686-w64-mingw32-strip.exe

_CFLAGS  = -std=gnu99 -O2 -Wall -Wl,--subsystem,console:4.10 -march=pentium -D_9XFETCH_BACKEND_WIN32 $(CFLAGS)
_LDFLAGS = -static -static-libgcc $(LDFLAGS)

TARGET  = 9xfetch.exe
SRCS    = $(wildcard src/*.c) $(wildcard src/win32/*.c)
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS) | $(MINGW_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(_LDFLAGS)
#	$(STRIP) $(TARGET)

%.o: %.c | $(MINGW_DIR)
	$(CC) $(_CFLAGS) -c $< -o $@

$(MINGW_DIR):
	@echo "Downloading mingw..."
	./mingw.sh

run: $(TARGET)
	wine $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)