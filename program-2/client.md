## Title  
** Echo Client **  

## Requirements  
1. Prompts the user to enter a string to send to the server.  
2. Connects to the echo server via TCP.  
3. Sends the entered message to the server.  
4. Receives the echoed message and displays it.  
5. Handles errors gracefully.  
6. Compiles with `cc`.  

## Inputs  
- The IP address or hostname of the server (command-line argument).  
- A string entered by the user.  

## Outputs  
- The echoed message received from the server.  

## Test Plan  
1. Compile the client: `cc echo_client.c -o echo_client`  
2. Start the echo server.  
3. Run the client with the server's IP as an argument: `./echo_client <server_ip>`  
4. Enter a test string and verify that the server echoes it back correctly.  
5. Test different cases (uppercase/lowercase) based on server mode.  
6. Verify error handling (invalid input, lost connection, incorrect arguments).  

### Test Cases  
| Input | Expected Output |
|-------|---------------|
| "test" (server in uppercase mode) | "TEST" |
| "HELLO" (server in lowercase mode) | "hello" |
| Empty message | No response or error |
| "exit" | Client disconnects |

## Design Overview  
- The client takes the server's IP/hostname as a command-line argument.  
- It establishes a TCP connection with the echo server.  
- Reads user input and sends it to the server.  
- Waits for the echoed message and displays it to the user.  
- Handles connection failures and invalid inputs gracefully.  

