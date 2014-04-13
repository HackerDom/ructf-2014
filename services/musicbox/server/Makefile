CC=gcc
CFLAGS=-std=gnu1x
CXX=g++
CXXFLAGS=-Wno-unused-result -std=c++0x
LDFLAGS=-s -Wall -Wextra -Wl,--gc-sections -luuid -logg -lvorbis
OBJFILES=musicbox.o server.o logging.o networking.o store.o metadata.o
OUTPATH=bin

all: musicbox

debug: CXXFLAGS+=-DDEBUG
debug: all

musicbox: $(OBJFILES)
	$(CC) $(OBJFILES) $(LDFLAGS) -o $(OUTPATH)/musicbox

clean:
	rm -f *.o $(OUTPATH)/musicbox