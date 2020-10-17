#ifndef _SCREEN_CONSUMER_H_
#define _SCREEN_CONSUMER_H_
#include "list.h"

struct socketStuff{
	int socketDescriptor;
	struct sockaddr_in sin;
};

void UDP_Consumer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty,
                            void* localListInput, struct socketStuff* sockInfo);
void UDP_Consumer_shutdown();

#endif

