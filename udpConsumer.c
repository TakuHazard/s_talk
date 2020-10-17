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


#include "udpConsumer.h"
#include "list.h"


struct socketStuff* socketInput;

pthread_t listenThreadPID;
pthread_mutex_t s_syncOkToTypeMutex;
pthread_cond_t* s_localListNotEmpty;
static unsigned int sin_len;
void* localList;

void* sendThread(){	
	int socketDescriptor = socketInput->socketDescriptor;
	struct sockaddr_in sinRemote = socketInput->sin;
	while(1){
		pthread_mutex_lock(&s_syncOkToTypeMutex);
		// printf("About to send \n");
		List_first(localList);
		if(List_count(localList) <= 0){
			// printf("Sending thread is waiting on %p\n",&s_localListNotEmpty);
			pthread_cond_wait(s_localListNotEmpty, &s_syncOkToTypeMutex);
			// printf("sendThread: We got the signal to wake up\n");
		}
		char* messageToSend = List_remove(localList);
		// printf("sendThread: about to give up lock");
		pthread_mutex_unlock(&s_syncOkToTypeMutex);
		// printf("Message being sent is %s\n", messageToSend);
		sin_len = sizeof(sinRemote);
        // printf("Checking socketInfo stuff \n");
        struct sockaddr_in sinVar = socketInput->sin;
        // printf("socketDescript %d socketPort %d IPADDR %d", socketDescriptor,sinVar.sin_port,sinVar.sin_addr.s_addr);
		int error = sendto(socketDescriptor, messageToSend, strlen(messageToSend), 0, 
			(struct sockaddr*) &sinRemote, sin_len);
		// printf("sendThread: sendingCode is %d\n", error);
	}

	return NULL;
}



void UDP_Consumer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty,void* localListInput, struct socketStuff* sockInfo){
    s_localListNotEmpty = pLocalListNotEmpty;
    localList = localListInput;
    s_syncOkToTypeMutex = *pSyncOkToTypeMutex;
    socketInput = sockInfo;
    struct sockaddr_in sinVar = socketInput->sin;
    int socketDescriptor = socketInput -> socketDescriptor;
    printf("UDP_CONSUMER_INIT : socketDescript %d socketPort %d IPADDR %d", socketDescriptor,sinVar.sin_port,sinVar.sin_addr.s_addr);

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