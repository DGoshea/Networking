/*
 * This conf_parser library contains the load_config function.
 *
 * load_config parses the name=value pairs from the "server.conf" file.
 * The file is used to configure the echo server's port and server mode.
 *
 * The first parameter to load_config is an int pointer for the server port.
 * The second parameter is the server mode string (char pointer).
 * These two parameters will be filled in by load_config.
 * The third parameter is the size of the server mode string parameter.
 * The returned server port will be in the range 11000-11999.
 * The returned server mode string will either be "upper" or "lower".
 * The mode string parameter must be large enough to store either of these.
 *
 * The return value of load_config is true if the configuration options were
 *     successfully parsed from the server configuration file and false if it
 *     was unable to access the server configuration file.
 *
 * The conf_parser library defines two port constants as described below.
 *
 * The "server.conf" file format is brittle.
 * The configuration file:
 *     is made up of comments and name=value pairs
 *     comments start with # (actually anything that isn't a valid name)
 *     names and values are always returned as lower case, but may be any case
 *         in the configuration file
 *     does *NOT* allow spaces around the equal symbol
 *     name=value pair *MUST* start in the first column
 *     *MUST* be named "server.conf" 
 *     *MUST* be stored in the same folder as the server executable
 */
#include <stdbool.h>

#ifndef CONF_PARSER_H
#define CONF_PARSER_H

extern const int   MIN_PORT;     // 11000
extern const int   MAX_PORT;     // 11999

bool load_config(int*, char*, size_t);

#endif  /* CONF_PARSER_H */
