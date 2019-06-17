#include <signal.h>

void printWaitStatus(const char *msg, int status);
void printWaitIdStatus(const char *msg, siginfo_t *wait_info);
