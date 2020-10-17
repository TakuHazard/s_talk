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
#include "keyboardProducer.h"

#define MSG_MAX_LEN 1024

pthread_t threadSendPID;
pthread_mutex_t syncOkToTypeMutex;
pthread_cond_t* s_localListNotEmpty;
void* localList;

void* storeInList(void* localList) {

    while(1) {
        char* msg = (char*)malloc(sizeof(char)*(MSG_MAX_LEN));
        fflush(stdin);
        fgets(msg, sizeof(char)*(MSG_MAX_LEN), stdin);
        
        pthread_mutex_lock(&syncOkToTypeMutex);
    
        List_append(localList, msg);
        
        if(List_count(localList) > 0) {
            pthread_cond_signal(s_localListNotEmpty);
        }
    
        pthread_mutex_unlock(&syncOkToTypeMutex);
    }

	return NULL;

}

void Keyboard_Producer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty, void* localListInput) {
    // create thread and put into list

    s_localListNotEmpty = pLocalListNotEmpty;
    localList = localListInput;
    syncOkToTypeMutex = *pSyncOkToTypeMutex;
    
    pthread_create(
		&threadSendPID,
		NULL,
		storeInList,
		localList
	);

}

void Keyboard_Producer_shutdown() {
    // "close/cleanup" the thread

    pthread_join(threadSendPID, NULL);
}