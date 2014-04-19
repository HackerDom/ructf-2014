CC=gcc
CXXFLAGS=
SOURCES=chat.c io.c db.c

debug: CXXFLAGS+=-DDEBUG
debug: all

all: chat

chat: $(SOURCES)
	$(CC) $(SOURCES) --std=gnu99  $(CXXFLAGS) -I/usr/local/include -L/usr/local/lib -o chat -lmongoc

clean:
	rm -f chat
