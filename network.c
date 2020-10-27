#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "list.h"
#include "keyboardProducer.h"
#include "udpProducer.h"
#include "screenConsumer.h"
#include "udpConsumer.h"
#include "socketStuff.h"
#include "ShutdownManager.h"

// Global variables
static pthread_mutex_t s_syncOkToTypeMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_localListNotEmpty = PTHREAD_COND_INITIALIZER;
socketStuff sockInfo;

static pthread_mutex_t s_syncOkToRemoveFromList = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_remoteListNotEmpty = PTHREAD_COND_INITIALIZER;


static pthread_mutex_t s_mutexShutdown = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  s_cvBeginShuttingDown = PTHREAD_COND_INITIALIZER;

List* localList;
List* remoteList;

// Get IP address of local and remote processes, then build required UDP Sockets
int createSockets(int localPortNum, int remotePortNum, char* givenHostName, struct sockaddr_in* sinLocalInput, struct sockaddr_in* sinRemoteInput) {
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

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if(p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
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

int main(int argc, char* argv[]) {
	int socketDescriptor;
	char* givenPortNum = argv[1];
	char* givenHostName = argv[2];
	char* givenRemotePortNum = argv[3];

	int localPortNum = atoi(givenPortNum);
	int remotePortNum = atoi(givenRemotePortNum);

	struct sockaddr_in sin;
	struct sockaddr_in sinRemote;
	
	int socketResult = createSockets(localPortNum, remotePortNum, givenHostName, &sin, &sinRemote);
	if(socketResult != 0) {
		printf("Connection failed\n");
		return 1;
	}
	
	// Create the socket for UDP
	socketDescriptor = socket(AF_INET,SOCK_DGRAM,0);
	// Bind the socket to specified port and IP address
	int bindResult = bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	if(bindResult != 0) {
		printf("Error binding to port\n");
		return 1;
	}
	
	sockInfo.socketDescriptor = socketDescriptor;
	sockInfo.sin = sinRemote;

	// Create lists
	localList = List_create();
	remoteList = List_create();

	// Startup all threads
	Keyboard_Producer_init(&s_syncOkToTypeMutex, &s_localListNotEmpty, localList, &s_mutexShutdown, &s_cvBeginShuttingDown);
	UDP_Consumer_init(&s_syncOkToTypeMutex, &s_localListNotEmpty, localList, &sockInfo);
	UDP_Producer_init(&s_syncOkToRemoveFromList, &s_remoteListNotEmpty, remoteList, &sockInfo);
	Screen_Consumer_init(&s_syncOkToRemoveFromList, &s_remoteListNotEmpty, remoteList, &s_mutexShutdown, &s_cvBeginShuttingDown);

	// Cleanup threads
	ShutDownManager_init(&s_mutexShutdown, &s_cvBeginShuttingDown);
	ShutDownManager_shutdown();

	// Destroy all Mutex and Cond Variables
	int m_ResA = pthread_mutex_destroy(&s_syncOkToTypeMutex);
	int m_ResB = pthread_mutex_destroy(&s_syncOkToRemoveFromList);
	int m_ResC = pthread_mutex_destroy(&s_mutexShutdown);
	int cv_ResA = pthread_cond_destroy(&s_localListNotEmpty);
	int cv_ResB = pthread_cond_destroy(&s_remoteListNotEmpty);
	int cv_ResC = pthread_cond_destroy(&s_cvBeginShuttingDown);
	
	if(m_ResA != 0 || m_ResB != 0|| m_ResC != 0|| cv_ResA != 0|| cv_ResB != 0|| cv_ResC != 0) {
		printf("Error destroying mutexes or condition variables\n");
		return 1;
	}

	return 0;
} 