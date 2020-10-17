#ifndef _UDP_PRODUCER_H_
#define _UDP_PRODUCER_H_

#include "list.h"
struct socketStuff{
	int socketDescriptor;
	struct sockaddr_in sin;
};

void UDP_Producer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty, void* localListInput, struct socketStuff* sockInfo);
void UDP_Producer_shutdown();

#endif