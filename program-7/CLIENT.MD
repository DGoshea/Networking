Challenge Client

## Program Requirements

- Accept exactly one command-line argument: the server hostname or IP.
- Establish a TCP connection to the server on port 11000.
- Send a 105 coded challenge request packet
- Receive a 110 coded challenge packet & decrypt the payload 
  - A 1 byte operation (`+`, `-`, `*`, `/`, `%`).
  - Two 4 byte  unsigned operands (network byte order).
- Compute the 64 bit integer result of op1 op op2
- Encrypt the 8 byte result (network byte order) into payload.
- Send a 115 coded challenge response packet  with IV, payload length, and encrypted result.
- Receive a 205 (SUCCESS) or 210 (FAILURE) packet
- Print status messages at each step:
  - `send request`
  - `receive challenge`
  - `send challenge response`
  - `receive <username> SUCCESS|FAILURE`
- Handle all errors gracefully

## Program Inputs

- Command-line argument: 1 string (`argv[1]`), hostname or IP.  
  - Type: Null-terminated C string.  
- Hard coded username: 16 bytes
  - Type: Up to 15 printable characters
- Server challenge:
  - 1 byte `char` for operation: one of `'+', '-', '*', '/', '%'`.
  - 4 bytes `uint32_t` left operand (network byte order).
  - 4 bytes `uint32_t` right operand (network byte order).

## Program Outputs

- Console log: Plain text messages on stdout or stderr:
  1. send request
  2. receive challenge
  3. send challenge response
  4. receive <username> SUCCESS or receive <username> FAILURE
- Error messages: On protocol or I/O failure, print to stderr and exit with non-zero.

## Test Plan

| Test Case                  | Input                                                          | Expected Output                           
| 1. Missing hostname        | ./client                                                       | usage: client <hostname> (exit non-zero)   |
| 2. Valid addition          | Server sends op '+', operands 4, 5                             | Steps 1-3  then receive USER SUCCESS  |
| 3. Division                | Server sends op '/', operands 10, 3                            | receive USER SUCCESS with correct result   |
| 4. Division by zero        | Server sends op '/', operands 8, 0                             | Client error division by zero (exit non-zero) |
| 5. Invalid operation       | Server sends op '#', operands 1, 1                             | Client error invalid op '#' (exit non-zero)|
| 6. Malformed packet        | Server closes pipe early (EOF)                                 | Client error failed to receive challenge   |

## Design Overview

1. Parse arguments: Verify argc == 2. On failure, print usage and exit.
2. Prepare username field: copy usernmae into 16 bytes
3. Choose AES Key
4. Open the socket (TCP on port 11000)
5. Send request: 
 - zero a msg_len buffer
 - memcpy
 - send_all 
6. Receive Challenge: 
 - recv bytes
 - verify first 3 bytes (110)
 - extract 16 byte IV and payload len 4 bytes
 - copy encrypted payload, decrypt via crypt_aes
 - extract op, op1, op2
7. Compute result:
 - switch on op for 5 operations 
 - check division/modulo by zero 
8. Send response: 
 - convert unit64_t result to network order 
 - ecrypt 8 bytes 
 - zero buffer
 - send
9. Reveive status: 
 - only need 19 bytes 
 - first 3 == 205 print SUCCESS
 - first 3 == 210 print FAILURE
 - else error 
10. Cleanup, handle errors gracefully, free allocations, exit 0 on success
