# Markdown file for SERVER.c 

## Title
** Fortune Teller Server **

## Requiremnts 
1. No interface 
2. Shutdown by using CTRL-C 
3. Handle errors gracefully (see client.c for specs) 
4. Listen on the assigned port number using TCP after each connection is processed until it is killed 
5. Reply with one response from a randomly selected array of 20 responses. The reply should include the question and the answer 
6. Compile with cc

## Inputs 
- A tcp connection to client
- A yes or no question from the client over the network

## Outputs 
- A response sent back to the client (randomly chosen from array of 20 repsonses)

## Test Plan
1. Compile the server
2. Start the server
3. Start the client and then connect them 
4. Enter a yes or no question from client
5. Verify that the question is recieved correctly and is printed 
6. Verify that the client correctly recieves the response (randomly chosen from the array) 

## Design Overview 
* The server creates a TCP socket and binds it to port
* It listens for the connections and accepts the client 
* The server recieves the message and prints it
* The server should randomly generate a response (see paper) 
* The server should send the quesiton and reply to the client 
* Hnadle errors gracefully
* Continues listening after each connection is processed until it is killed 
