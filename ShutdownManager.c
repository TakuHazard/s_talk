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
#include <stdbool.h>

#include "ShutdownManager.h"
#include "screenConsumer.h"
#include "udpConsumer.h"
#include "udpProducer.h"
#include "keyboardProducer.h"


static pthread_t threadShutDownPID;
static pthread_mutex_t mutexShutdown;
static pthread_cond_t* s_CVStartShuttingdown;
static bool beginShuttingDown = false;



// Wait on a mutex/conditional variable block this thread until someone calls trigger shutdown
void* ShutDownManger_WaitForShutdown(){
    printf("This is the shutdown manager beginning to wait \n");
    pthread_mutex_lock(&mutexShutdown); 
    while(!beginShuttingDown){
        printf("Beginning to go to sleep while waiting for signal to start shutdown\n");
        pthread_cond_wait(s_CVStartShuttingdown, &mutexShutdown);
        printf("Waking up to start shutting down");
    }
    pthread_mutex_unlock(&mutexShutdown);

    printf("Telling all other threads to start shutting down\n");
    // ShutDownManager_TriggerShutdown();
    return NULL;
}

// Indicates that we are beginning to shutdown and tells blocked threads to continue. 
// Occurs after we get exclamation mark from either keyboard or screen
void ShutDownManager_TriggerShutdown(){
    beginShuttingDown = true;
}


// UNSURE;
bool ShutDownManager_IsShuttingdown(){
    return false;
}

// Tell the other threads to start shutting down
void ShutDownManager_shutdown(){
    pthread_join(threadShutDownPID, NULL);
    Screen_Consumer_shutdown();
    UDP_Consumer_shutdown();
    UDP_Producer_shutdown();
    Keyboard_Producer_shutdown();
}

void ShutDownManager_init(pthread_mutex_t* pMutexShutdown,pthread_cond_t* pCVStartShuttingdown){
    mutexShutdown = *pMutexShutdown;
    s_CVStartShuttingdown = pCVStartShuttingdown;

    pthread_create(
        &threadShutDownPID,
        NULL,
        ShutDownManger_WaitForShutdown,
        NULL
    );
}
