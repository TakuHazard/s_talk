#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "list.h"
#include "socketStuff.h"
#include "udpConsumer.h"

socketStuff* socketInput;

static pthread_t listenThreadPID;
static pthread_mutex_t s_syncOkToTypeMutex;
static pthread_cond_t* s_localListNotEmpty;

static unsigned int sin_len;
void* localList;

// Sends the message over the network
void* sendThread() {	
	int socketDescriptor = socketInput->socketDescriptor;
	struct sockaddr_in sinRemote = socketInput->sin;

	while(1) {
		pthread_mutex_lock(&s_syncOkToTypeMutex);
		
		List_first(localList);
		
		while(List_count(localList) <= 0) {
			pthread_cond_wait(s_localListNotEmpty, &s_syncOkToTypeMutex);
		}

		char* messageToSend = List_remove(localList);

		pthread_mutex_unlock(&s_syncOkToTypeMutex);
		
		sin_len = sizeof(sinRemote);
		int error = sendto(socketDescriptor, messageToSend, strlen(messageToSend), 0, (struct sockaddr*) &sinRemote, sin_len);

		//free(messageToSend);

		if(error==-1) {
			perror("send");
		}
	}

	return NULL;
}

// Initialize the UDP Consumer Thread
void UDP_Consumer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty,void* localListInput, socketStuff* sockInfo){
    s_localListNotEmpty = pLocalListNotEmpty;
    localList = localListInput;
    s_syncOkToTypeMutex = *pSyncOkToTypeMutex;
    socketInput = sockInfo;

    pthread_create(
		&listenThreadPID,
		NULL,
		sendThread,
		localList
	);
}

// "close/cleanup" the thread
void UDP_Consumer_shutdown() {
        pthread_join(listenThreadPID, NULL);
}