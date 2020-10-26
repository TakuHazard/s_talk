#ifndef _KEYBOARD_PRODUCER_H_
#define _KEYBOARD_PRODUCER_H_

#include "list.h"
void Keyboard_Producer_init(void* localListInput, pthread_mutex_t* pMutexShutdown,pthread_cond_t* pCVStartShuttingdown, 
                           pthread_mutex_t* plistManipulation, pthread_cond_t* plocalListNotEmpty, pthread_cond_t* plocalListNotFull);
void Keyboard_Producer_shutdown();

#endif