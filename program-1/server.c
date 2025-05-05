// THIS IS JUST A START
/*
 * Resources: man pages for random number, gets, snprintf, printf, style, nc
 * Websites: geeks for geeks, beej.us, csrc.nist, aourceware
 * Description: A simple server that will accept a yes or no question  from client and generate a random response to send back to client. 
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

const int MAX_LINE    = 256;
const int SERVER_PORT = 11063;
const int MAX_PENDING = 5;

int
main(void)
{
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    socklen_t new_len;
    int s, new_s;

    /* build address data structure */
    memset(&sin, 0, sizeof sin);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    /*Build sockect handle errors (some use of exit to handle errors gracefully)*/
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        err(1, "unable to open socket");
        close(s); 
        exit(1); 
    }

    /*Setup bind */
    if ((bind(s, (struct sockaddr*)&sin, sizeof sin)) == -1) {
        close(s);
        err(2, "unable to bind socket");
        close(s); 
        exit(1); 
    }
    
    /*Setup listen and handle errors */
    if ((listen(s, MAX_PENDING)) == -1) {
        err(3, "unable to listen on socket");
        close(s);
        exit(1); 
    }

    printf("Server is listening on port %d \n", SERVER_PORT); 

    /* wait for a connection then recieve the question from client */
    while (1) {
        new_len = sizeof sin;
        if ((new_s = accept(s, (struct sockaddr*)&sin, &new_len)) == -1) {
            err(4, "accept error");
            continue; 
        }

    
        /* recv will return < 0 if error handle gracefully */
        memset(buf, 0, sizeof buf);
        if (recv(new_s, buf, sizeof buf, 0) > 0){   
            printf("Q: %s", buf);

            //Check buffer size (handle gracefully)  
            if(strnlen(buf, MAX_LINE) >= MAX_LINE){
                fprintf(stderr, "Question is too long. Try Again.\n"); 
                close(new_s); 
                continue; 
            }
            
            //Must null terminate C-strings 
            size_t len = 0; 
            len = strlen(buf);  
            buf[len-1] = '\0'; 
            

            //Init responses and arcrandom to get reply 
            char* responses[] = {
            "It is certain", "Without a doubt", "You may rely on it", "Most likely", "Yes", "Reply hazy try again", "Better not tell you now",
            "Concentrate and ask again", "My reply is no", "Outlook not so good", "It is decidedly so", "Yes, definitely", "As I see it, yes", 
            "Outlook good", "Signs point to yes", "Ask again later", "Cannot predict now", "Don't count on it", "My sources say no", "Very doubtful"
            }; 
            char* reply = responses[arc4random() % 20]; 

            //Check for buffer overflow and handle gracefully 
            char response_buf[MAX_LINE];
            int ret = snprintf(response_buf, sizeof response_buf, "Q: \"%s\"\nR: \"%s\"\n", buf, reply); 
            if(ret < 0 || (size_t)ret >= sizeof(response_buf)){
                fprintf(stderr, "Response buffer overflow"); 
                close(new_s); 
                continue;
            } 

            //send that bad boy to client (how tho??) 
            send(new_s, response_buf, strlen(response_buf), 0); 
        
        }

        close(new_s);
    }

    close(s);
    return 0;
}
