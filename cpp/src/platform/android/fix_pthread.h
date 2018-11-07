#pragma once
#include <pthread.h>

void fix_pthread_handler_thread_exit_on_cancel(int sig);

void fix_pthread_cancel_disable();

void fix_pthread_cancel_enable();

void fix_pthread_cancel_thread(pthread_t thread);

void fix_pthread_install_cancel_handler();


