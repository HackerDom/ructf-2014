CC=gcc
CFLAGS=-m32
CXX=g++
CXXFLAGS=-m32 -Wno-unused-result -std=c++0x
LDFLAGS=-m32 -s -Wl,--gc-sections -luuid
OBJFILES=Musicbox.o Server.o Store.o ClientConnection.o File.o logging.o

all: musicbox

debug: CXXFLAGS+=-DDEBUG
debug: all

musicbox: $(OBJFILES)
	$(CXX) $(LDFLAGS) $(OBJFILES) -o musicbox

clean:
	rm -f *.o musicbox