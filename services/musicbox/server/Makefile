CC=gcc
CFLAGS=-std=gnu1x
LDFLAGS=-s -Wall -Wextra -Wl,--gc-sections -luuid -logg -lvorbis
OBJFILES=musicbox.o server.o logging.o networking.o store.o metadata.o
OUTPATH=bin

all: musicbox

debug: CFLAGS += -DDEBUG
debug: all

static: LDFLAGS += -static
static: all

musicbox: $(OBJFILES)
	$(CC) $(OBJFILES) $(LDFLAGS) -o $(OUTPATH)/musicbox

clean:
	rm -f *.o $(OUTPATH)/musicbox
