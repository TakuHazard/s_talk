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

static pthread_mutex_t listManipulation;
static pthread_cond_t* s_localListNotEmpty;
static pthread_cond_t* s_localListNotFull;

static pthread_mutex_t mutexShutdown;
static pthread_cond_t* s_CVStartShuttingdown;
void* localList;

// Store the message into a List
void* storeInList(void* localList) {
    char* msg = NULL;

    while(1) {
        // Start by producing new item.
        msg = (char*)malloc(sizeof(char)*(MSG_MAX_LEN));
        char tmp[3];
        tmp[0] = '!';
        tmp[1] = '\n';
        tmp[2] = 0;

        fflush(stdin);
        char* rv = fgets(msg, sizeof(char)*(MSG_MAX_LEN), stdin);
        if(rv == NULL){
            continue;
        }

        pthread_mutex_lock(&listManipulation);
        // pthread_mutex_lock(&mutexLocalListNotFull);

        while(List_count(localList) >= LIST_MAX_NUM_NODES){
            pthread_cond_wait(s_localListNotFull, &listManipulation);
        }
        List_prepend(localList,msg);

        pthread_mutex_unlock(&listManipulation);
        pthread_cond_signal(s_localListNotEmpty);


        if (strcmp(msg, tmp) ==0) {
            printf("Triggering shutting down sequence\n");
            pthread_mutex_lock(&mutexShutdown);
            ShutDownManager_TriggerShutdown();
            pthread_cond_signal(s_CVStartShuttingdown);
            pthread_mutex_unlock(&mutexShutdown);
        }

    }
    
    // free(msg);
	return NULL;

}

// Initialize the Keyboard Producer Thread
void Keyboard_Producer_init(void* localListInput, pthread_mutex_t* pMutexShutdown,pthread_cond_t* pCVStartShuttingdown, 
                           pthread_mutex_t* plistManipulation, pthread_cond_t* plocalListNotEmpty, pthread_cond_t* plocalListNotFull){


    listManipulation    = *plistManipulation;
    s_localListNotEmpty = plocalListNotEmpty;
    s_localListNotFull  = plocalListNotFull;

    localList = localListInput;
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
    pthread_cancel(threadSendPID);
    pthread_join(threadSendPID, NULL);
}