#ifndef _KEYBOARD_PRODUCER_H_
#define _KEYBOARD_PRODUCER_H_

#include "list.h"

void Keyboard_Producer_init(pthread_mutex_t* pSyncOkToTypeMutex, void* localList);
void Keyboard_Producer_shutdown();

#endif