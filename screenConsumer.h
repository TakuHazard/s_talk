#ifndef _SCREEN_CONSUMER_H_
#define _SCREEN_CONSUMER_H_

#include "list.h"

void Screen_Consumer_init(pthread_mutex_t* psyncOkToRemoveFromList, pthread_cond_t* pRemoteListNotEmpty, void* remoteListInput);
void Screen_Consumer_shutdown();

#endif