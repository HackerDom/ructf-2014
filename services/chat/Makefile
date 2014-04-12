CC=gcc
CXXFLAGS=

debug: CXXFLAGS+=-DDEBUG
debug: all

all: chat

chat: chat.c io.c storage.c
	$(CC) $(CXXFLAGS) -o chat chat.c io.c storage.c

clean:
	rm -f chat
