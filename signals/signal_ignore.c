/*
 * signal_ignore.c
 *
 * Ignoring a single.
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t change_handler;

void sig_handler(int sig) 
{
    printf("Caught signal %s(%d)", strsignal(sig), sig);
    change_handler = 1;
}

int main()
{
    signal(SIGINT, sig_handler);

    for (;;) {
        if (change_handler == 1) {
            signal(SIGINT, SIG_IGN);
        }
        sleep(3);
        printf("Sleeping\n");
    }
    return 0;
}
