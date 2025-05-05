# Markdown file for client.c

## Title  
** Fortune Teller Client **

## Requirments 
The client file must do the following 
- Accept one command line argument (will be the IP or hostname on the server)
-  Display the recieved string in the terminal 
-  Communicate with the server 
-  Handle errors gracefully (Recover from the error and continue or display error message and end with cleanup) 
-  Send a yes or no question to server and then display the reply from the server
- Compile with cc 
- Running instance of the server 

## Inputs 
* Command line argument of the server name
* A question from the user 

## Outputs
* The program should dipslay the question and the reply from the server 

## Test Plan 
1. Compile the client using cc (./client <name> ) 
2. Start the server on speciifed IP 
3. Run the client with the server's IP as arg
4. Enter a question 
5. Verify the response from server
6. Use different question (try to break it) 
7. Verify error handling (comand line args and questions) 

## Design Overview 
- The client will connect via TCP to the server using IP address and port
- Read a question from the user and sends it to the server 
- Wait on the server for a resposne and print that to terminal 
- Hand errors gracefully (return message, deallocate, and terminate or recover and continue running) 
**Design Overview** 

