#ifndef _UDP_PRODUCER_H_
#define _UDP_PRODUCER_H_

#include "list.h"
#include "socketStuff.h"

void UDP_Producer_init(pthread_mutex_t* psyncOkToRemoveFromListMutex, pthread_cond_t* pRemoteListNotEmpty, 
                        void* remoteListInput, socketStuff* sockInfo);
void UDP_Producer_shutdown();

#endif