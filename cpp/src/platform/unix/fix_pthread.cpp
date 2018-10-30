#include "fix_pthread.h"
#include <signal.h>
#include <string.h>

void fix_pthread_handler_thread_exit_on_cancel(int sig)
{
    pthread_exit(0);
}

void fix_pthread_cancel_disable() {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
}

void fix_pthread_cancel_enable() {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
}

void fix_pthread_cancel_thread(pthread_t thread) {
	pthread_kill(thread, SIGUSR1);
}

void fix_pthread_install_cancel_handler() {
	struct sigaction actions;
	memset(&actions, 0, sizeof(actions));
	sigemptyset(&actions.sa_mask);
	actions.sa_flags = 0;
	actions.sa_handler = fix_pthread_handler_thread_exit_on_cancel;
	sigaction(SIGUSR1,&actions,NULL);
}


