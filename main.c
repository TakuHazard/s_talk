#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define MSG_MAX_LEN 1024
#define PORT 22110

int main(){

	printf("This is the prot %d\n", PORT);
	printf("Connect using : \n");
	printf("       netcat -u 127.0.0.1 %d\n", PORT);

	//Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT);

	// Create the socket for UDP
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

	while(1){
		struct sockaddr_in sinRemote;
		unsigned int sin_len = sizeof(sinRemote);
		char messageRx[MSG_MAX_LEN];
		int bytesRx = recvfrom(socketDescriptor, messageRx, MSG_MAX_LEN, 0, (struct sockaddr*) &sinRemote, &sin_len);
		int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN;
		messageRx[terminateIdx] = 0;
		printf("Message received (%d bytes :\n\n%s\n", bytesRx, messageRx);

		//Extract the value from the message :
		// (process the message any way your app requires)
		int incMe = atoi(messageRx);


		// Compose the reply message
		char messageTx[MSG_MAX_LEN];
		fgets(messageTx, sizeof(messageTx),stdin);


		// Transmit a reply

		sin_len = sizeof(sinRemote);
		sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr*) &sinRemote, sin_len);


	}

	//close
	close(socketDescriptor);


}