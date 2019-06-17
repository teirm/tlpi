/*
 * sys_v_signals.c
 *
 * Implementation of SYS V signals
 * using the posix api.
 *
 * 07 February 2019
 */

#define _GNU_SOURCE
#include <stddef.h>
#include <signal.h>

sighandler_t my_sigset(int sig, sighandler_t disp)
{
    int rc; 
    sigset_t curr_mask;
    sigset_t prev_mask;
    struct sigaction sa;
    struct sigaction old_sa; 
    if (sigprocmask(SIG_SETMASK, NULL, &curr_mask) == -1) {
        return (void *)-1;
    }
    
    rc = sigismember(&curr_mask, sig);

    if (rc == -1) {
        return (void *)-1;
    }

    if (rc == 1) {
        return SIG_HOLD;
    }
    
    if (disp != SIG_HOLD) {
        if (sigdelset(&curr_mask, sig) == -1) {
            return (void *)-1;
        }
    }
    
    sa.sa_handler = disp;
    sa.sa_flags = 0; 
   
    if (disp != SIG_DFL && disp != SIG_IGN && disp != SIG_HOLD) {
        if (sigaddset(&sa.sa_mask, sig) == -1) {
            return (void *)-1;
        }
    }
    
    if (sigaction(sig, &sa, &old_sa) == -1) {
        return (void *)-1;
    }

    return old_sa.sa_handler;
}

int my_sighold(int sig)
{
    sigset_t curr_mask;

    if (sigprocmask(SIG_SETMASK, NULL, &curr_mask) == -1) {
        return -1;
    }

    if (sigaddset(&curr_mask, sig) == -1) {
        return -1;
    }

    
    return sigprocmask(SIG_SETMASK, &curr_mask, NULL);
}

int my_sigrelse(int sig)
{
    sigset_t curr_mask;
    
    if(sigprocmask(SIG_SETMASK, NULL, &curr_mask) == -1) {
        return -1;
    }

    if (sigdelset(&curr_mask, sig) == -1) {
        return -1;
    }

    return sigprocmask(SIG_SETMASK, &curr_mask, NULL);
}

int my_sigignore(int sig)
{
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigemptyset(&sa.sa_mask) == -1) {
        return -1;
    }

    return sigaction(sig, &sa, NULL);
}

int my_sigpause(int sig)
{
    sigset_t new_mask;

    if (sigaddset(&new_mask, sig) == -1) {
        return -1;
    }

    return sigsuspend(&new_mask);
}
