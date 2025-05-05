/*
 * Resources: man pages, stack overflow
 * Websites: stackoverflow.com
 * Description: This client simply creates a socket, connects, and sends and 
   receives a message
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char *server_ip;
    int port;
    char *buffer = NULL;
    size_t len = 0;
    ssize_t read_len;
    char recv_buffer[1024]; // recv needs a fixed buffer??

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server_ip = argv[1];
    port = atoi(argv[2]);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter a message: ");
    read_len = getline(&buffer, &len, stdin);
    if (read_len == -1) {
        perror("Failed to read input");
        free(buffer);
        close(sock);
        exit(EXIT_FAILURE);
    }

    buffer[strcspn(buffer, "\n")] = 0;

    if (send(sock, buffer, strlen(buffer), 0) == -1) {
        perror("Send failed");
    }

    read_len = recv(sock, recv_buffer, sizeof(recv_buffer) - 1, 0);
    if (read_len > 0) {
        recv_buffer[read_len] = '\0';
        printf("Server response: %s\n", recv_buffer);
    }
    else {
        perror("Receive failed");
    }

    free(buffer);
    close(sock);
    return 0;
}

