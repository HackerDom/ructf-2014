CC=gcc
CFLAGS=-std=c11
CXX=g++
CXXFLAGS=-Wno-unused-result -std=c++0x
LDFLAGS=-s -Wall -Wextra -Wl,--gc-sections -luuid
OBJFILES=musicbox.o server.o logging.o
OUTPATH=./bin

all: musicbox

debug: CXXFLAGS+=-DDEBUG
debug: all

musicbox: $(OBJFILES)
	$(CC) $(OBJFILES) $(LDFLAGS) -o $(OUTPATH)/musicbox

clean:
	rm -f *.o $(OUTPATH)/musicbox