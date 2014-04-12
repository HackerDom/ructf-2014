CC=gcc
CXXFLAGS=

debug: CXXFLAGS+=-DDEBUG
debug: all

all: chat

chat: chat.c
	$(CC) $(CXXFLAGS) -o chat chat.c 

clean:
	rm -f chat
