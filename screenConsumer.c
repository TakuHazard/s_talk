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

#define MSG_MAX_LEN 1024

static pthread_t threadPrintPID;
static pthread_mutex_t syncOkToRemoveFromList;
static pthread_cond_t* s_remoteListNotEmpty;

void* remoteList;

// Displays the message
void* printThread() {
    while(1) {     
        pthread_mutex_lock(&syncOkToRemoveFromList);

        while(List_count(remoteList) <= 0) {
            pthread_cond_wait(s_remoteListNotEmpty, &syncOkToRemoveFromList);
        }

        List_first(remoteList);
        char* msg = List_remove(remoteList);
        char tmp[3];
        tmp[0] = '!';
        tmp[1] = '\n';
        tmp[2] = 0;

        pthread_mutex_unlock(&syncOkToRemoveFromList);

        if (strcmp(msg, tmp)==0) {
            printf("End of session in screen!! \n");
        } else {
            fputs(msg, stdout); // message returned
        }
	}

    return NULL;
}

// Initialize the UDP Producer Thread
void Screen_Consumer_init(pthread_mutex_t* psyncOkToRemoveFromList, pthread_cond_t* pRemoteListNotEmpty, void* remoteListInput) {
    s_remoteListNotEmpty = pRemoteListNotEmpty;
    remoteList = remoteListInput;
    syncOkToRemoveFromList = *psyncOkToRemoveFromList;
    
    pthread_create(
        &threadPrintPID,
        NULL,
        printThread,
        NULL
    );
}

// "close/cleanup" the thread
void Screen_Consumer_shutdown() {
    pthread_join(threadPrintPID, NULL);
}