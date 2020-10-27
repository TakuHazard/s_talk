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
#include "ShutdownManager.h"

#define MSG_MAX_LEN 1024

static pthread_t threadSendPID;
static pthread_mutex_t syncOkToTypeMutex;
static pthread_cond_t* s_localListNotEmpty;
static bool wasMsgSentToList = false;

// Mutex and condition variable for shutting down;
static pthread_mutex_t mutexShutdown;
static pthread_cond_t* s_CVStartShuttingdown;

void* localList;
char* msg = NULL;

// Store the message into a List
void* storeInList(void* localList) {
    while(1) {
        wasMsgSentToList = false;
        msg = (char*)malloc(sizeof(char)*(MSG_MAX_LEN));
        char tmp[3];
        tmp[0] = '!';
        tmp[1] = '\n';
        tmp[2] = 0;

        fflush(stdin);
        char* fgetsResult = fgets(msg, sizeof(char)*(MSG_MAX_LEN), stdin);
        if(fgetsResult == NULL){
            printf("Error getting input\n");
            continue;
        }
        
        pthread_mutex_lock(&syncOkToTypeMutex);

        wasMsgSentToList = true;
        List_append(localList, msg);
        
        if(List_count(localList) > 0) {
            pthread_cond_signal(s_localListNotEmpty);
        }
    
        pthread_mutex_unlock(&syncOkToTypeMutex);

        if (strcmp(msg, tmp)==0) {
            pthread_mutex_lock(&mutexShutdown);

            ShutDownManager_TriggerShutdown();
            pthread_cond_signal(s_CVStartShuttingdown);
            
            pthread_mutex_unlock(&mutexShutdown);
        }
    }
    
    free(msg);
	return NULL;

}

// Initialize the Keyboard Producer Thread
void Keyboard_Producer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty, void* localListInput, 
                                pthread_mutex_t* pMutexShutdown, pthread_cond_t* pCVStartShuttingdown) {
    s_localListNotEmpty = pLocalListNotEmpty;
    localList = localListInput;
    syncOkToTypeMutex = *pSyncOkToTypeMutex;

    mutexShutdown = *pMutexShutdown;
    s_CVStartShuttingdown = pCVStartShuttingdown;
    
    pthread_create(
		&threadSendPID,
		NULL,
		storeInList,
		localList
	);

}

// "close/cleanup" the threads
void Keyboard_Producer_shutdown() {
    int recv;
    recv = pthread_cancel(threadSendPID);
    if(recv != 0) {
		printf("Keyboard Thread failed to cancel!\n");
	}

    if(!wasMsgSentToList) {
        free(msg);
        msg = NULL;
    }

    pthread_mutex_unlock(&syncOkToTypeMutex);
    pthread_mutex_unlock(&mutexShutdown);

    recv = pthread_join(threadSendPID, NULL);
    if(recv != 0) {
		printf("Keyboard Thread failed to join!\n");
	}
}