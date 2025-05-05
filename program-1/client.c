// THIS IS JUST A START
/*
 * Name: Divine Goshea 
 * Course-section: CS440-01
 * Assignment: #1 
 * Due Date: 2/14/25 11:59pm
 * Collaborators: ChatGPT, Claude 3.5 Sonnet 
 * Resources: man pages for random number, gets, snprintf, printf, style, nc
 * Websites: geeks for geeks, beej.us, csrc.nist, aourceware
 * Description: A simple server that will accept a yes or no question  from client and generate a random response to send back to client. 
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int SERVER_PORT = 11063;
const int MAX_LINE    = 256;

int
main(int argc, char* argv[])
{
    char buf[MAX_LINE];
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    int s;
    size_t len, slen;

    if (argc == 2)
        host = argv[1];
    else {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    /* translate the hostname into an IP address */
    hp = gethostbyname(host);
    if (!hp)
        errx(1, "unknown host %s", host);

    /* build address data structure */
    memset(&sin, 0, sizeof sin);
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);
    
    /* active open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        err(2, "unable to open socket");

    if ((connect(s, (struct sockaddr*)&sin, sizeof sin)) == -1) {
        close(s);
        err(3, "connect failed");
    }

   /* Get user input */
   printf("Enter your (yes/no) question: "); 
   if(!fgets(buf, sizeof(buf), stdin)){
       fprintf(stderr, "Could not read question");
       close(s); 
       exit(1); 
   }

   /* Null terminate */
   slen = strnlen(buf, sizeof(buf)); 
   buf[slen] = '\0'; 

   /*Send that bad boy */
   len = strnlen(buf, sizeof(buf)); 
   if (send(s, buf, len, 0) == -1){
       perror("could not send"); 
       close(s); 
       exit(1); 
    }

    /* Recieve and print to terminal */
    memset(buf, 0, sizeof(buf)); 
    if((len = recv(s, buf, sizeof(buf)-1, 0)) > 0){
        buf[len] = '\0'; 
        printf("\n%s\n", buf); 
    }
    else {
        perror("recv error"); 
    }

    close(s);
    return 0;
}

