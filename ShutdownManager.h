#ifndef _SHUTDOWN_MANAGER_H
#define _SHUTDOWN_MANAGER_H


// Wait on a mutex/conditional variable block this thread until someone calls trigger shutdown
void* ShutDownManger_WaitForShutdown();

// Indicates that we are beginning to shutdown and tells blocked threads to continue. 
// Occurs after we get exclamation mark from either keyboard or screen
void ShutDownManager_TriggerShutdown();


// UNSURE;
bool ShutDownManager_IsShuttingdown();


// Init Thread
void ShutDownManager_init(pthread_mutex_t* pMutexShutdown,pthread_cond_t* pCVStartShuttingdown);


void ShutDownManager_shutdown();
#endif