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
#include "screenConsumer.h"
#include "ShutdownManager.h"

#define MSG_MAX_LEN 5096

static pthread_t threadPrintPID;
static pthread_mutex_t syncOkToRemoveFromList;
static pthread_cond_t* s_remoteListNotEmpty;
void* remoteList;
static char* msg = NULL;


// Mutex and condition variable for shutting down;
static pthread_mutex_t mutexShutdown;
static pthread_cond_t* s_CVStartShuttingdown;

void messageDestructorCons(void* pItem) {
    free(pItem);
    pItem = NULL;
}

// Displays the message
void* printThread() {
    while(1) {     
        pthread_mutex_lock(&syncOkToRemoveFromList);

        while(List_count(remoteList) <= 0) {
            pthread_cond_wait(s_remoteListNotEmpty, &syncOkToRemoveFromList);
        }

        List_first(remoteList);
        msg = List_remove(remoteList);
        char tmp[3];
        tmp[0] = '!';
        tmp[1] = '\n';
        tmp[2] = 0;

        pthread_mutex_unlock(&syncOkToRemoveFromList);

        if (strcmp(msg, tmp) == 0) {
            pthread_mutex_lock(&mutexShutdown);

            ShutDownManager_TriggerShutdown();
            pthread_cond_signal(s_CVStartShuttingdown);
            
            pthread_mutex_unlock(&mutexShutdown);

        } else {
            // fputs(msg, stdout);
            write(stdout,msg,MSG_MAX_LEN);
            // List_free(remoteList, *messageDestructorCons);
            free(msg);
            msg = NULL;
        }
	}

    return NULL;
}

// Initialize the UDP Producer Thread
void Screen_Consumer_init(pthread_mutex_t* psyncOkToRemoveFromList, pthread_cond_t* pRemoteListNotEmpty, 
                            void* remoteListInput, pthread_mutex_t* pMutexShutdown, pthread_cond_t* pCVStartShuttingdown) {
    s_remoteListNotEmpty = pRemoteListNotEmpty;
    remoteList = remoteListInput;
    syncOkToRemoveFromList = *psyncOkToRemoveFromList;

    mutexShutdown = *pMutexShutdown;
    s_CVStartShuttingdown = pCVStartShuttingdown;
    
    pthread_create(
        &threadPrintPID,
        NULL,
        printThread,
        NULL
    );
}

// "close/cleanup" the thread
void Screen_Consumer_shutdown() {
    int recv;
    recv = pthread_cancel(threadPrintPID);
    if(recv != 0) {
		printf("Screen Thread failed to cancel!\n");
	}

    pthread_mutex_unlock(&syncOkToRemoveFromList);
    pthread_mutex_unlock(&mutexShutdown);

    List_free(remoteList, *messageDestructorCons);
    if(msg){
        free(msg);
        msg = NULL;
    }

    recv = pthread_join(threadPrintPID, NULL);
    if(recv != 0) {
		printf("Screen Thread failed to join!\n");
	}
}