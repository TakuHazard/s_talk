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
void* localList;

void* storeInList(void* localList) {

    while(1) {
        char* msg = (char*)malloc(sizeof(char)*(MSG_MAX_LEN));
        fflush(stdin);
        fgets(msg, sizeof(char)*(MSG_MAX_LEN), stdin);
        printf("this is what you typed: %s\n", msg);

        pthread_mutex_lock(&syncOkToTypeMutex);
        { 
            List_append(localList, msg);
        }
        pthread_mutex_unlock(&syncOkToTypeMutex);
    }

	return NULL;

}

void Keyboard_Producer_init(pthread_mutex_t* pSyncOkToTypeMutex, void* localListInput) {
    // create thread and put into list
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