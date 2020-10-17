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
#include "screenConsumer.h"

#define MSG_MAX_LEN 1024

pthread_t threadPrintPID;
pthread_mutex_t syncOkToRemoveFromList;
pthread_cond_t* s_remoteListNotEmpty;
void* remoteList;

void* printThread(){
    while(1){    
        pthread_mutex_lock(&syncOkToRemoveFromList);
        while(List_count(remoteList)<=0){
            pthread_cond_wait(s_remoteListNotEmpty, &syncOkToRemoveFromList);
        }
        List_first(remoteList);
        char* msg = List_remove(remoteList);
        pthread_mutex_unlock(&syncOkToRemoveFromList);

        printf("%s", msg);

	}

    return NULL;
}

void Screen_Consumer_init(pthread_mutex_t* psyncOkToRemoveFromList, pthread_cond_t* pRemoteListNotEmpty, void* remoteListInput){
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

void Screen_Consumer_shutdown(){
    pthread_join(threadPrintPID, NULL);
}
