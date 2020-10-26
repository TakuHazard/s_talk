#ifndef _UDP_CONSUMER_H_
#define _UDP_CONSUMER_H_
#include "list.h"
#include "socketStuff.h"
void UDP_Consumer_init(void* localListInput,socketStuff* sockInfo,pthread_mutex_t* pListManipulation,pthread_cond_t* plocalListNotEmpty,pthread_cond_t* plocalListNotFull);
void UDP_Consumer_shutdown();

#endif

