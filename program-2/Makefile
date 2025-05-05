#Makefile for server and client
CFLAGS=-Wall -Wextra -g

.PHONY: clean all

all: server client

# Link server using the provided conf_parser.o
server: server.o conf_parser.o
	$(CC) $(CFLAGS) -o $@ $^

client: client.o
	$(CC) $(CFLAGS) -o $@ $^

# Compile server.c with dependency on conf_parser.h
server.o: server.c conf_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

client.o: client.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f server client server.o client.o *.core *~ 2>/dev/null || true

# Don't delete the provided conf_parser.o during clean
.PRECIOUS: conf_parser.o
