#ifndef _SHUTDOWN_MANAGER_H
#define _SHUTDOWN_MANAGER_H

void* ShutDownManger_WaitForShutdown();
void ShutDownManager_TriggerShutdown();
bool ShutDownManager_IsShuttingdown(); // UNSURE; currently an unused function
void ShutDownManager_init(pthread_mutex_t* pMutexShutdown, pthread_cond_t* pCVStartShuttingdown);
void ShutDownManager_shutdown();

#endif