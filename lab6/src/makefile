CC=gcc
CFLAGS= -I.

all : server client

server : server.c library.o library.h
	$(CC) -pthread -o server server.c library.o $(CFLAGS)

client : client.c library.o library.h
	$(CC) -pthread -o client client.c library.o $(CFLAGS)

library.o : library.h
	$(CC) -o library.o -c library.c $(CFLAGS)

clean :
	rm server client library.o