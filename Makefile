TARGETS = server client
OBJECTS = server.o client.o utils.o msg.o
LOADLIBES := -lpthread
CC=gcc
CFLAGS+=-Wall

all: $(TARGETS)

server: server.o utils.o utils.h msg.o msg.h

server.o: server.c utils.h msg.h

client: client.o utils.o utils.h msg.o msg.h

client.o: client.c utils.h msg.h

utils.o: utils.c utils.h msg.h

msg.o: msg.c msg.h utils.h

.PHONY: clean
clean:
	rm -f *.o *~
