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

#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define MSG_MAX_LEN 1024


static struct sockaddr_in sinRemote;
static int socketDescriptor;
static int bytesRx;
static unsigned int sin_len;


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
		// printf("RECEIVED SOMETHING %d\n", bytesRx);
		int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN;
		messageRx[terminateIdx] = 0;
		printf("%s", messageRx);
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

	printf("PORTNUM %d hostname %s remotePortNum %d\n", localPortNum, givenHostName, remotePortNum);


	// GET ADDRESSINFO FOR THE OTHER PEER
	// struct addrinfo hints, *res;
	
	// memset(&hints, 0, sizeof hints);
	// hints.ai_family = AF_UNSPEC;
	// hints.ai_socktype = SOCK_DGRAM;

	// if((rv = getaddrinfo(givenHostName, givenRemotePortNum, &hints, &servinfo)) != 0){
	// 	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	// 	return 1;
	// }

	// for(p = servinfo; p != NULL; p = p->ai_next) {
 //        if ((socketDescriptor = socket(p->ai_family, p->ai_socktype,
 //                p->ai_protocol)) == -1) {
 //            perror("client: socket");
 //            continue;
 //        }

 //        if (connect(socketDescriptor, p->ai_addr, p->ai_addrlen) == -1) {
 //            close(socketDescriptor);
 //            perror("client: connect");
 //            continue;
 //        }

 //        break;
 //    }

 //    if (p == NULL) {
 //        fprintf(stderr, "client: failed to connect\n");
 //        return 2;
 //    }

 //    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
 //            s, sizeof s);
 //    printf("client: connecting to %s\n", s);
 //    freeaddrinfo(servinfo);
	
	// getaddrinfo(givenHostName,givenPortNum,&hints,&res);



	struct addrinfo hints, *res,*p;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	int rv = getaddrinfo(givenHostName, NULL, &hints, &res);
	if(rv < 0){
		printf("ERRROR \n");
		return 1;
	}
	printf("Checking results after getaddrinfo \n");
	printf("ai_flags %d ai_family %d ai_socktype %d ai_cannonname %s \n",
		res->ai_flags, res->ai_family, res->ai_socktype, res->ai_canonname);

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
    printf("IP ADDRESS is %s\n", ipstr);


	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(localPortNum);


	memset(&sinRemote, 0, sizeof(sinRemote));
	sinRemote.sin_family = res->ai_family;
	// sinRemote.sin_addr.s_addr = htonl(INADDR_ANY);
	sinRemote.sin_port = htons(remotePortNum);
	inet_pton(AF_INET,ipstr,&(sinRemote.sin_addr));

	printf("Address of remote afterwards is %d\n",sinRemote.sin_addr.s_addr);

	// Create the socket for UDP
	socketDescriptor = socket(AF_INET,SOCK_DGRAM,0);

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

