/*
Name: Divine Goshea 
* Course-section: CS440-01
* Assignment: #7 
* Due Date: 4/30/25 11:59pm
 * Collaborators: ChatGPT for send_all and recv_all wrapper functions 
 * Resources: man pages for functions 
 * Websites: geeks for geeks, beej.us, csrc.nist, sourceware
 * Description: A client that sends a challenge request, receives a challenge,
   and send a challenge response using AES-128 bit encyption. 
 */
#include <arpa/inet.h>    // htonl, ntohl
#include <endian.h>       // htobe64, be64toh
#include <err.h>          // err, errx
#include <netdb.h>        // gethostbyname
#include <stdio.h>        // printf, fprintf
#include <stdlib.h>       // malloc, free
#include <string.h>       // memset, memcpy, strcmp
#include <sys/socket.h>   // socket, connect, send, recv
#include <netinet/in.h>   // sockaddr_in
#include <unistd.h>       // close, getlogin

#include "aes_crypto.h"

#define SERVER_PORT 11000
#define MSG_LEN     (3 + 16 + BLK_LEN + 4 + 128)

/* Wrapper functions for send & receive to ensure correct size */
static ssize_t send_all(int, const void *, size_t);
static ssize_t recv_all(int, void *, size_t);

int
main(int argc, char *argv[])
{
    if (argc != 2)
        errx(1, "usage: %s <hostname_or_IP>", argv[0]);
    const char *host = argv[1];
    char *uname = "chgosh";
    char userfield[16] = {0};
    strlcpy(userfield, uname, sizeof(userfield));

    /*  Get key based on user (mine is hardcoded) */
    unsigned char aes_key[16];
    if      (strcmp(uname, "chgosh")      == 0)
        memcpy(aes_key, "UK8Gah7ki7xohgi2", 16);
    else if (strcmp(uname, "fflintstone") == 0) 
        memcpy(aes_key, "jzT2xRDrqQkgK5pZ", 16);
    else if (strcmp(uname, "brubble")     == 0) 
        memcpy(aes_key, "Jw0vMS6vG6kka0gl", 16);
    else if (strcmp(uname, "gjetson")     == 0) 
        memcpy(aes_key, "Sy27eboHPoedN0fN", 16);
    else if (strcmp(uname, "pgriffin")    == 0) 
        memcpy(aes_key, "B8phzrQhXlbm5ij0", 16);
    else
        errx(3, "unknown user '%s' no AES key available", uname);

    /* Server setup */
    struct hostent *hp = gethostbyname(host);
    if (!hp)
        errx(4, "unknown host %s", host);
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* Open and connect socket */
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        err(5, "socket");
    if (connect(sock, (struct sockaddr *)&sin, sizeof sin) < 0)
        err(6, "connect");

    /* Send challenge request */
    printf("send request\n");
    unsigned char sendbuf[MSG_LEN];
    memset(sendbuf, 0, sizeof sendbuf);
    memcpy(sendbuf, "105", 3);          // message code
    memcpy(sendbuf + 3, userfield, 16);     // username
    if (send_all(sock, sendbuf, MSG_LEN) != MSG_LEN)
        err(7, "failed to send challenge request");

    /* Receive challenge */
    unsigned char recvbuf[MSG_LEN];
    if (recv_all(sock, recvbuf, MSG_LEN) != MSG_LEN)
        err(8, "failed to receive challenge");
    if (strncmp((char*)recvbuf, "110", 3) != 0)
    printf("receive challenge\n");

    /* Extract IV and decrypt payload */
    unsigned char iv_in[BLK_LEN];
    memcpy(iv_in, recvbuf + 3 + 16, BLK_LEN);
    uint32_t net_plen;
    memcpy(&net_plen, recvbuf + 3 + 16 + BLK_LEN, 4);
    uint32_t plen = ntohl(net_plen);
    unsigned char *encrypted = malloc(plen);
    if (!encrypted) err(11, "malloc");
    memcpy(encrypted,
           recvbuf + 3 + 16 + BLK_LEN + 4,
           plen);

    /* Decrypt challenge payload */
    unsigned char *decrypted = NULL;
    unsigned char *iv_ptr = malloc(BLK_LEN);
    if (!iv_ptr) err(12, "malloc");
    memcpy(iv_ptr, iv_in, BLK_LEN);
    int dlen = crypt_aes(encrypted, plen, &decrypted,
                         aes_key, &iv_ptr, DECRYPT);
    free(encrypted);
    free(iv_ptr);
    if (dlen <= 0)
        errx(13, "decrypt failed");

    /* Parse operation and operands */
    char op = decrypted[0];
    uint32_t lnet, rnet;
    memcpy(&lnet, decrypted + 1, 4);
    memcpy(&rnet, decrypted + 5, 4);
    uint32_t left  = ntohl(lnet);
    uint32_t right = ntohl(rnet);
    free(decrypted);

    /* Compute challenge result */
    uint64_t result;
    switch (op) {
      case '+': result = left + right; break;
      case '-': result = left - right; break;
      case '*': result = left * right; break;
      case '/': result = left / right; break;
      case '%': result = left % right; break;
      default:
        errx(17, "invalid op '%c'", op);
    }

    /* Allocate and send challenge response */
    printf("send challenge response\n");
    unsigned char resp_plain[8];
    uint64_t net_res = htobe64(result);
    memcpy(resp_plain, &net_res, 8);

    unsigned char *resp_encrypted = NULL;
    unsigned char *iv_out = malloc(BLK_LEN);
    if (!iv_out) err(18, "malloc");
    int rlen = crypt_aes(resp_plain, 8,
                         &resp_encrypted,
                         aes_key, &iv_out, ENCRYPT);

    memset(sendbuf, 0, sizeof sendbuf);
    memcpy(sendbuf, "115", 3);
    memcpy(sendbuf + 3,   userfield, 16);
    memcpy(sendbuf + 3 + 16, iv_out, BLK_LEN);
    uint32_t net_rlen = htonl(rlen);
    memcpy(sendbuf + 3 + 16 + BLK_LEN, &net_rlen, 4);
    memcpy(sendbuf + 3 + 16 + BLK_LEN + 4,
           resp_encrypted, rlen);
    free(iv_out);
    free(resp_encrypted);
    
    if (send_all(sock, sendbuf, MSG_LEN) != MSG_LEN)
        err(20, "failed to send response");

    /* Receive status code and close connection */
    unsigned char status[3 + 16]; 
    ssize_t got = recv_all(sock, status, sizeof(status)); 
    if (got != sizeof(status))
        err(21, "failed to receive status");
    char code[4] = {0};
    memcpy(code, status, 3);
    
    if (strncmp(code, "205", 3) == 0)
        printf("receive %s SUCCESS\n", uname);
    else if (strncmp(code, "210", 3) == 0)
        printf("receive %s FAILURE\n", uname);
    else
        errx(22, "unexpected status code %.3s", code);

    close(sock);
    return 0;
}

/* Used chatgpt to debug send function as was not sending correct amount
   amount of bytes */
static ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t sent = 0;
    const char *p = buf;
    while (sent < len) {
        ssize_t n = send(sockfd, p + sent, len - sent, 0);
        if (n <= 0) return n;
        sent += n;
    }
    return sent;
}
/* Used chatgpt to debug recv function  as was not receiving correct amount */
static ssize_t recv_all(int sockfd, void *buf, size_t len) {
    size_t recvd = 0;
    char *p = buf;
    while (recvd < len) {
        ssize_t n = recv(sockfd, p + recvd, len - recvd, 0);
        if (n <= 0) break;
        recvd += n;
    }
    return recvd;
}

