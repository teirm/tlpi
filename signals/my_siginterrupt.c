/*
 * my_siginterrupt.c
 *
 * An implementation of siginterrupt using
 * sigaction
 */

#include <signal.h>

int
my_siginterrupt(int sig, int flag)
{
    struct sigaction sa;
    
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_DFL;

    if (!flag) {
        sa.sa_flags = SA_RESTART;
    }

    return sigaction(sig, &sa, 0);

}
