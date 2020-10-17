#ifndef _KEYBOARD_PRODUCER_H_
#define _KEYBOARD_PRODUCER_H_

#include "list.h"

void Keyboard_Producer_init(pthread_mutex_t* pSyncOkToTypeMutex, pthread_cond_t* pLocalListNotEmpty,
                            void* localListInput);
void Keyboard_Producer_shutdown();

#endif