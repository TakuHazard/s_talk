#include <stdlib.h>
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
#include <errno.h>

#include "list.h"
#include "keyboardProducer.h"
// #include "screenConsumer.h"

#define MSG_MAX_LEN 1024


// static struct sockaddr_in sinRemote;
static unsigned int sin_len;
static pthread_mutex_t s_syncOkToTypeMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_localListNotEmpty = PTHREAD_COND_INITIALIZER;

List* localList;

struct socketStuff{
	int socketDescriptor;
	struct sockaddr_in sin;
};

void* sendThread(void* socketInput){
	struct socketStuff* socketInfo = (struct socketStuff* ) socketInput;
	
	int socketDescriptor = socketInfo->socketDescriptor;
	struct sockaddr_in sinRemote = socketInfo->sin;

	bool continueSending = true;
	while(continueSending){
		// Compose the reply message
		// char messageTx[MSG_MAX_LEN];
		// fgets(messageTx, MSG_MAX_LEN, stdin);


		// Transmit a reply

		pthread_mutex_lock(&s_syncOkToTypeMutex);
		printf("About to send \n");
		List_first(localList);
		if(List_count(localList) <= 0){
			printf("Sending thread is waiting on %p\n",&s_localListNotEmpty);
			pthread_cond_wait(&s_localListNotEmpty, &s_syncOkToTypeMutex);
			printf("sendThread: We got the signal to wake up\n");
		}
		char* messageToSend = List_remove(localList);
		printf("sendThread: about to give up lock");
		pthread_mutex_unlock(&s_syncOkToTypeMutex);
		printf("Message being sent is %s\n", messageToSend);
		sin_len = sizeof(sinRemote);
		int error = sendto(socketDescriptor, messageToSend, strlen(messageToSend), 0, 
			(struct sockaddr*) &sinRemote, sin_len);
		printf("sendThread: sendingCode is %d\n", error);

	}

	return NULL;
}

void* receiveThread(void* socketInput){
	// Address
	printf("beginning to receive !\n");
	struct socketStuff* socketInfo = (struct socketStuff* ) socketInput;
	int socketDescriptor = socketInfo->socketDescriptor;
	struct sockaddr_in sinRemote = socketInfo->sin;
	while(1){
		printf("starting to listen\n");
		sin_len = sizeof(sinRemote);
		char messageRx[MSG_MAX_LEN];
		printf("receiveThread: before recvfrom\n");
		int bytesRx = recvfrom(socketDescriptor, messageRx, MSG_MAX_LEN, 0, 
			(struct sockaddr*) &sinRemote, &sin_len);
		printf("receiveThread: after recfrom\n");
		int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN;
		messageRx[terminateIdx] = 0;
		printf("%s", messageRx);
	}

	printf("Done with rxThread\n");
	return NULL;
}

int createSockets(int localPortNum, int remotePortNum, char* givenHostName, 
	struct sockaddr_in* sinLocalInput, struct sockaddr_in* sinRemoteInput) {

	struct addrinfo hints, *res,*p;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	int rv = getaddrinfo(givenHostName, NULL, &hints, &res);

	if(rv < 0) {
		printf("ERROR \n");
		return 1;
	}

	char ipstr[INET6_ADDRSTRLEN];

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
	
	memset(sinLocalInput, 0, sizeof(*sinLocalInput));
	(*sinLocalInput).sin_family = AF_INET;
	(*sinLocalInput).sin_addr.s_addr = htonl(INADDR_ANY);
	(*sinLocalInput).sin_port = htons(localPortNum);

	memset(sinRemoteInput, 0, sizeof(*sinRemoteInput));
	(*sinRemoteInput).sin_family = AF_INET;
	(*sinRemoteInput).sin_port = htons(remotePortNum);
	inet_pton(AF_INET,ipstr,&(sinRemoteInput->sin_addr));


	return 0;
}

int main(int argc, char* argv[]){
	int socketDescriptor;
	char* givenPortNum = argv[1];
	char* givenHostName = argv[2];
	char* givenRemotePortNum = argv[3];

	int localPortNum = atoi(givenPortNum);
	int remotePortNum = atoi(givenRemotePortNum);


	printf("PORTNUM %d hostname %s remotePortNum %d\n", localPortNum, givenHostName, remotePortNum);

	struct sockaddr_in sin;
	struct sockaddr_in sinRemote;
	int socketResult = createSockets(localPortNum, remotePortNum, givenHostName, &sin, &sinRemote);
	if(socketResult != 0){
		printf("Connection failed\n");
		return 1;
	}
	// Create the socket for UDP
	socketDescriptor = socket(AF_INET,SOCK_DGRAM,0);


	// printf("After returning we have the following local %d %s  remote %d %s\n", sin.sin_port, sin.sin_addr.s_addr, sinRemote.sin_port, sinRemote.sin_addr.s_addr);
	// // Bind the socket to the port (PORT) that we specify
	bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	
	struct socketStuff sockInfo;
	sockInfo.socketDescriptor = socketDescriptor;
	sockInfo.sin = sinRemote;

	//Create a list
	localList = List_create();

	Keyboard_Producer_init(&s_syncOkToTypeMutex,&s_localListNotEmpty, localList);

	pthread_t threadListenPID;
	pthread_t threadSendPID;
    
    pthread_create(
		&threadSendPID,
		NULL,
		sendThread,
		&sockInfo
	);

	pthread_create(
		&threadListenPID,
		NULL,
		receiveThread,
		&sockInfo
	);
	

	Keyboard_Producer_shutdown();

	// pthread_join(threadListenPID, NULL);
	return 0;

}



