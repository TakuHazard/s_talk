#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h> // for strncmp()
#include <unistd.h> // for close()
#include <ctype.h> // for toupper()
#include "list.h"

#define PORT 6060

int main() {
    int clientSocket = socket(PF_INET, SOCK_DGRAM, 0); // create UDP socket for client
    struct sockaddr_in serv_addr; // connector's address info

    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; // family/domain
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // interface to listen on
    serv_addr.sin_port = htons(PORT); // client port

    // Initialization for while loop
	unsigned int serv_len = sizeof(serv_addr);
	char msg[1024], msgserv[1024];
	int nBytes;

    while(1) {
        printf("Type a sentence to send to server: ");
        fgets(msg, sizeof(msg), stdin);

        nBytes = strlen(msg) + 1;

        // Send msg to server
        sendto(clientSocket, msg, nBytes, 0, 
            (struct sockaddr*) &serv_addr, serv_len);

        // Receive msg from server
        nBytes = recvfrom(clientSocket, msgserv, 1024, 0, NULL, NULL);

        printf("Message from server: %s\n", msgserv);
    }

    // Close
    close(clientSocket);
    
    return 0;
}