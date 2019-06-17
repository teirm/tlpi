/*
 * signal_functions.h
 *
 * Signal functions provided by TLPI.
 *
 */

#include <stdio.h>
#include <signal.h>

void printSigset(FILE *of, const char *prefix, const sigset_t *sigset);
int printSigMask(FILE *of, const char *msg);
int printPendingSigs(FILE *of, const char *msg);
