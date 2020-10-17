
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

#include "list.h"
#include "udpProducer.h"

pthread_t threadReceivePID;
pthread_mutex_t syncOkToRemoveFromList;
pthread_cond_t* s_remoteListNotEmpty;
struct socketStuff* socketInput;

void* remoteList;
#define MSG_MAX_LEN 1024
static unsigned int sin_len;

void* receiveThread(){
	int socketDescriptor = socketInput->socketDescriptor;
	struct sockaddr_in sinRemote = socketInput->sin;

	while(1){
		sin_len = sizeof(sinRemote);
        char* msg = (char*)malloc(sizeof(char)*(MSG_MAX_LEN));

		int bytesRx = recvfrom(socketDescriptor, msg, MSG_MAX_LEN, 0, 
			(struct sockaddr*) &sinRemote, &sin_len);
        
        printf("Message Received is %s\n", msg);
        pthread_mutex_lock(&syncOkToRemoveFromList);
        List_append(remoteList);
        if(List_count(remoteList) > 0){
            pthread_cond_signal(s_remoteListNotEmpty);
        }
        pthread_mutex_unlock(&syncOkToRemoveFromList);

		// int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN;
		// messageRx[terminateIdx] = 0;
		// printf("%s", messageRx);
	}

	printf("Done with rxThread\n");
	return NULL;
}
void UDP_Producer_init (pthread_mutex_t* psyncOkToRemoveFromListMutex, pthread_cond_t* pRemoteListNotEmpty,void* remoteListInput, struct socketStuff* sockInfo){
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
void UDP_Producer_shutdown(){
    pthread_join(threadReceivePID, NULL);

}