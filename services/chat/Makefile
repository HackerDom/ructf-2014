CC=gcc
CXXFLAGS=

debug: CXXFLAGS+=-DDEBUG
debug: all

all: chat

chat: chat.c io.c storage.c  db.c
	$(CC) --std=c99  $(CXXFLAGS) -I/usr/local/include -L/usr/local/lib -o chat chat.c io.c storage.c db.c  -lmongoc

clean:
	rm -f chat
