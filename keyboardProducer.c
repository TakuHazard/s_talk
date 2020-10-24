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
#include "keyboardProducer.h"

#define MSG_MAX_LEN 1024

static pthread_t threadSendPID;
static pthread_mutex_t syncOkToTypeMutex;
static pthread_cond_t* s_localListNotEmpty;

void* localList;

// Store the message into a List
void* storeInList(void* localList) {
    char* msg = NULL;

    while(1) {
        msg = (char*)malloc(sizeof(char)*(MSG_MAX_LEN));
        char tmp[3];
        tmp[0] = '!';
        tmp[1] = '\n';
        tmp[2] = 0;

        fflush(stdin);
        fgets(msg, sizeof(char)*(MSG_MAX_LEN), stdin);
        
        pthread_mutex_lock(&syncOkToTypeMutex);
    
        List_append(localList, msg);
        
        if(List_count(localList) > 0) {
            pthread_cond_signal(s_localListNotEmpty);
        }
    
        pthread_mutex_unlock(&syncOkToTypeMutex);

        if (strcmp(msg, tmp)==0) {
            printf("End of session in keyboard!! \n");
        }
    }
    
    //free(msg);
	return NULL;

}

// Initialize the Keyboard Producer Thread
void Keyboard_Producer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty, void* localListInput) {
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

// "close/cleanup" the thread
void Keyboard_Producer_shutdown() {
    pthread_join(threadSendPID, NULL);
}