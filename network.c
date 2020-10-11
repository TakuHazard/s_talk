// #include <stdio.h>
#include <stdlib.h>
// #include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MSG_MAX_LEN 1024
#define PORT 22110


static struct sockaddr_in sinRemote;
static int socketDescriptor;
static int bytesRx;
static unsigned int sin_len;

void* sendThread(void* unused){
	bool continueSending = true;
	while(continueSending){
		// char messageToSend[MSG_MAX_LEN];
		// printf("Here in sendThreadRx Type message to send\n");
		// fgets(messageToSend,sizeof messageToSend, stdin);
		// printf("Here in sendThreadRx messageTosend is %s", messageToSend);

		// int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN;
		// messageRx[terminateIdx] = 0;

		// //Extract the value from the message :
		// // (process the message any way your app requires)
		// int incMe = atoi(messageRx);

		// Compose the reply message
		char messageTx[MSG_MAX_LEN];
		// printf("Inside threadSendPID\n");
		fgets(messageTx, MSG_MAX_LEN, stdin);
		// sprintf(messageTx, "this is our reply\n");


		// Transmit a reply

		sin_len = sizeof(sinRemote);
		sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr*) &sinRemote, sin_len);


	}

	return NULL;


	
}

void* receiveThread(void* unused){
	//Address
	while(1){
		sin_len = sizeof(sinRemote);
		char messageRx[MSG_MAX_LEN];
		int bytesRx = recvfrom(socketDescriptor, messageRx, MSG_MAX_LEN, 0, (struct sockaddr*) &sinRemote, &sin_len);
		printf("RECEIVED SOMETHING %d\n", bytesRx);
		int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN;
		messageRx[terminateIdx] = 0;
		printf("Message received: %s\n", messageRx);
	}

	printf("Done with rxThread\n");
	return NULL;
}
int main(int argc, char* argv[]){

	char* givenPortNum = argv[1];
	char* givenHostName = argv[2];
	char* givenRemotePortNum = argv[3];

	int localPortNum = atoi(givenPortNum);
	int remotePortNum = atoi(givenRemotePortNum);
	strcat(givenHostName,".cs.surrey.sfu.ca");
	printf("This are the commandline args %d %s %d \n", localPortNum, givenHostName, remotePortNum);

	printf("This is the prot %d\n", PORT);
	printf("Connect using : \n");
	printf("       netcat -u 127.0.0.1 %d\n", PORT);

	struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(givenHostName, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(res); // free the linked list



	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT);
	// Create the socket for UDP
	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
	// Bind the socket to the port (PORT) that we specify
	bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));


	pthread_t threadListenPID;
	pthread_t threadSendPID;
	pthread_create(
		&threadListenPID,
		NULL,
		receiveThread,
		NULL
	);
	pthread_create(
		&threadSendPID,
		NULL,
		sendThread,
		NULL
	);
	pthread_join(threadSendPID, NULL);

	pthread_join(threadListenPID, NULL);
	return 0;

}



