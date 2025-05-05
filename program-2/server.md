## Title  
** Echo Server Daemon **  

## Requirements  
1. Runs as a daemon process.  
2. Controlled using SIGHUP or SIGTERM signals.  
3. Uses the `select` function to avoid blocking on `accept` calls.  
4. Reads configuration from a file to determine the listening port and server mode (uppercase or lowercase).  
5. Echoes every message from the client back after converting it to uppercase or lowercase based on the configured mode.  
6. Handles errors gracefully.  
7. Compiles with `cc`.  

## Inputs  
- A TCP connection from the client.  
- Configuration file specifying the port and server mode.  
- Messages sent by the client.  

## Outputs  
- Echoed messages in either uppercase or lowercase, depending on the mode.  

## Test Plan  
1. Compile the server: `cc echo_server.c -o echo_server`  
2. Start the server daemon using the configuration file.  
3. Use signals to control the server (SIGHUP to reload config, SIGTERM to terminate).  
4. Start the client and send test messages.  
5. Verify that messages are received and transformed correctly.  
6. Check error handling by providing invalid input or stopping the server unexpectedly.  

### Test Cases  
| Input | Expected Output |
|-------|---------------|
| "hello" (server in uppercase mode) | "HELLO" |
| "WORLD" (server in lowercase mode) | "world" |
| Empty message | No response or error |
| Connection dropped | Server continues running |

## Design Overview  
- The server runs as a daemon and initializes by reading its configuration file.  
- It listens on the specified TCP port and waits for connections.  
- Uses `select` to handle multiple clients without blocking on `accept`.  
- Upon receiving a message, it converts it to the specified case and sends it back.  
- Supports SIGHUP for reloading configuration and SIGTERM for graceful shutdown.  
- Continues listening after processing each connection.  

