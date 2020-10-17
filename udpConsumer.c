#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include "socketStuff.h"
#include "udpConsumer.h"
#include "list.h"


socketStuff* socketInput;

pthread_t listenThreadPID;
pthread_mutex_t s_syncOkToTypeMutex;
pthread_cond_t* s_localListNotEmpty;
static unsigned int sin_len;
void* localList;

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
	}

	return NULL;
}



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

void UDP_Consumer_shutdown() {
        pthread_join(listenThreadPID, NULL);

    //close
}