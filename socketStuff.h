#ifndef _SOCKET_STUFF_H_
#define _SOCKET_STUFF_H_

#include <netdb.h>

typedef struct socketStuff_s socketStuff;
struct socketStuff_s {
	int socketDescriptor;
	struct sockaddr_in sin;
};


#endif