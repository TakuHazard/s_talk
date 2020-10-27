#ifndef _UDP_CONSUMER_H_
#define _UDP_CONSUMER_H_

#include "list.h"
#include "socketStuff.h"

void UDP_Consumer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty, 
							void* localListInput, socketStuff* sockInfo);
void UDP_Consumer_shutdown();

#endif

