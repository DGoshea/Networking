/*  
 * Resources: man pages, w3 schools, stack overflow, geeksforgeeks
 * Description: This server acts as a daemon and echoes back in
   all uppercase or lowercase. This server is configured from a conf file.
*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>


#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "conf_parser.h"

#define BUFFER_SIZE 1024

int server_socket;
int port;
char mode[10];
volatile sig_atomic_t cleanup_exit = 0;

/* Function prototypes */
void load_server_config(void);
void signal_handler(int signal);
void run_server(void);

/* Main */
int main(void) {
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    load_server_config();
    run_server();
    return 0;
}

/*Implementations */
void load_server_config(void) {
    if (!load_config(&port, mode, sizeof(mode))) {
        fprintf(stderr, "Failed to load config.\n");
        port = MIN_PORT;
        strlcpy(mode, "upper", sizeof(mode));
    }
    
    /* Validate port */
    if (port < MIN_PORT || port > MAX_PORT) {
        fprintf(stderr, "Port %d is outside range (%d-%d). Using default.\n", 
                port, MIN_PORT, MAX_PORT);
        port = MIN_PORT;
    }
    
    printf("Config loaded: Port=%d, Mode=%s\n", port, mode);
}

void signal_handler(int signal) {
    if (signal == SIGHUP) {
        printf("Reloading configuration...\n");
        load_server_config();
    } else if (signal == SIGTERM) {
        printf("Shutting down server...\n");
        cleanup_exit = 1;
    }
}

void run_server(void) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    fd_set master_set, read_fds;
    int max_fd, new_socket;
    char buffer[BUFFER_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr *)&server_addr, 
    sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    FD_ZERO(&master_set);
    FD_SET(server_socket, &master_set);
    max_fd = server_socket;
    
    /* Use atomic variable to use select to avoid blocking on accept */
    while (!cleanup_exit) { 
        read_fds = master_set;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            if(cleanup_exit) break; //If signal is sent kill 
            perror("Select failed");
            exit(EXIT_FAILURE);
        }
        
        /* Try to accept */
        if (FD_ISSET(server_socket, &read_fds)) {
            new_socket = accept(server_socket, (struct sockaddr *)&client_addr,
             &client_len);
            if (new_socket >= 0) {
                FD_SET(new_socket, &master_set);
                if (new_socket > max_fd) max_fd = new_socket;
            } 
            else {
                perror("Accept failed");
            }
        }
        
        /* Read message and apply casing */
        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_fds) && i != server_socket) {
                int bytes_read = read(i, buffer, BUFFER_SIZE - 1);
                if (bytes_read <= 0) {
                    close(i);
                    FD_CLR(i, &master_set);
                }
                else { //If successful
                    buffer[bytes_read] = '\0';
                    /* Case Insensitive comparison needed */
                    int is_upper = strcasecmp(mode, "upper") == 0; 
                    for (int j = 0; j < bytes_read; j++) {
                        buffer[j] = is_upper ? toupper(buffer[j]) 
                        : tolower(buffer[j]);
                    }
                    /* Send back to client */
                    write(i, buffer, bytes_read); 
                }
            }
        }
    }

    close(server_socket);
}

