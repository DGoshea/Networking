CFLAGS  = -Wall -g
LDFLAGS = aes_crypto.o -lcrypto

.PHONY: clean all 

all: client

client: client.o aes_crypto.o
	$(CC) $(CFLAGS) -o $@ client.o aes_crypto.o -lcrypto

client.o: client.c aes_crypto.h
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f client client.o *.core *.c~

