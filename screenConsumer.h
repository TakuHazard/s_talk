#ifndef _SCREEN_CONSUMER_H_
#define _SCREEN_CONSUMER_H_

#include "list.h"

void Screen_Consumer_init(pthread_mutex_t* psyncOkToRemoveFromList, pthread_cond_t* pRemoteListNotEmpty, 
                            void* remoteListInput, pthread_mutex_t* pMutexShutdown, pthread_cond_t* pCVStartShuttingdown);
void Screen_Consumer_shutdown();

#endif