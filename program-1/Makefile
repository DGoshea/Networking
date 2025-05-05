# Makefile for simplex server and client  
CFLAGS=-Wall -g 

.PHONY: clean all

all: server client 

server: server.c
	${CC} ${CFLAGS} -o $@ server.c

client: client.c
	${CC} ${CFLAGS} -o $@ client.c

clean: 
	@rm -f server client *.core *.c~
