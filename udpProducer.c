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
#include "udpProducer.h"

#define MSG_MAX_LEN 5096

socketStuff* socketInput;

static pthread_t threadReceivePID;
static pthread_mutex_t syncOkToRemoveFromList;
static pthread_cond_t* s_remoteListNotEmpty;
bool wasMessageSent = false;

void* remoteList;
static unsigned int sin_len;
static char* msg = NULL;

// Receives the message and appends it into a list
void* receiveThread() {
	int socketDescriptor = socketInput->socketDescriptor;
	struct sockaddr_in sinRemote = socketInput->sin;
	while(1) {
		sin_len = sizeof(sinRemote);
        msg = (char*)malloc(sizeof(char)*(MSG_MAX_LEN));
        wasMessageSent = false;
		int bytesRx = recvfrom(socketDescriptor, msg, MSG_MAX_LEN, 0, (struct sockaddr*) &sinRemote, &sin_len);

        if(bytesRx==-1) {
            perror("recv");
        }
        
        pthread_mutex_lock(&syncOkToRemoveFromList);

        List_append(remoteList, msg);
        wasMessageSent = true;
        
        if(List_count(remoteList) > 0) {
            pthread_cond_signal(s_remoteListNotEmpty);
        }

        pthread_mutex_unlock(&syncOkToRemoveFromList);
	}

	return NULL;
}

// Initialize the UDP Producer Thread
void UDP_Producer_init(pthread_mutex_t* psyncOkToRemoveFromListMutex, pthread_cond_t* pRemoteListNotEmpty, 
                        void* remoteListInput, socketStuff* sockInfo) {
    s_remoteListNotEmpty = pRemoteListNotEmpty;
    remoteList = remoteListInput;
    syncOkToRemoveFromList = *psyncOkToRemoveFromListMutex;
    socketInput = sockInfo;
    
    pthread_create(
        &threadReceivePID,
        NULL,
        receiveThread,
        NULL
    );
}

// "close/cleanup" the thread
void UDP_Producer_shutdown() {
    int recv = pthread_cancel(threadReceivePID);
    if(recv != 0) {
		printf("UDP Producer Thread failed to cancel!\n");
	}

    pthread_mutex_unlock(&syncOkToRemoveFromList);

    if(!wasMessageSent) {
        free(msg);
        msg = NULL;
    }

    recv = pthread_join(threadReceivePID, NULL);
    if(recv != 0) {
		printf("UDP Producer Thread failed to join!\n");
	}
}