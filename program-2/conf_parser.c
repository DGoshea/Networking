/*  Divine Goshea
 * Course-section: CS440-01
 * Assignment: #5
 * Due Date: 3/12
 * Collaborators: ChatGPT for comments and function protype and 
   implementation separation
 * Resources: man pages, w3 schools, stack overflow, geeksforgeeks
 * Description: This config implementation file implements the config_parser.h
   function and returns true if we were able to get a port and mode. 
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "conf_parser.h"

#define CONFIG_FILE "server.conf"
#define LINE_BUFFER_SIZE 256
const int   MIN_PORT = 11000; 
const int   MAX_PORT = 11999; 

bool load_config(int* port, char* mode, size_t mode_size) {
    bool port_found = false;
    bool mode_found = false;
    char line[LINE_BUFFER_SIZE];

    FILE* config_file = fopen(CONFIG_FILE, "r");
    if (config_file == NULL) {
        return false;
    }
    
    while (fgets(line, sizeof(line), config_file)) {
        /* Remove newlines*/
        char *newline = strchr(line, '\n');
        if (newline) *newline = 0;
        
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == 0) {
            continue;
        }
        
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");
        
        if (key != NULL && value != NULL) {
            if (strcmp(key, "PORT") == 0) {
                *port = atoi(value);
                port_found = true;
            } 
            else if (strcmp(key, "MODE") == 0) {
                if (strcmp(value, "upper") == 0 || strcmp(value, "lower") == 0) {
                    strncpy(mode, value, mode_size - 1);
                    mode[mode_size - 1] = '\0';
                    mode_found = true;
                }
            }
        }
    }
    
    fclose(config_file);
    return port_found && mode_found;
}
