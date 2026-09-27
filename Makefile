# AESDumpster Makefile (Linux builds; 'windows' cross-compiles with mingw-w64)
CXX      ?= g++
CROSS    ?= x86_64-w64-mingw32-
CXXFLAGS ?= -std=c++17 -O2 -Wall -D_FILE_OFFSET_BITS=64

SRCS     := $(wildcard src/*.cpp)

.PHONY: all debug windows clean

all: aesdumpster

aesdumpster: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

debug: CXXFLAGS += -g -D_DEBUG
debug: clean
debug: aesdumpster

# Wide entrypoint (-municode) and static linking so the exe runs standalone.
windows:
	$(CROSS)g++ $(CXXFLAGS) -municode -static $(SRCS) -o aesdumpster.exe

clean:
	rm -f aesdumpster aesdumpster.exe