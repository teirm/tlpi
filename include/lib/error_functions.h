#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

void errMsg(const char *format, ...);

#ifdef __GNUC__
    /* This macro stops 'gcc -Wall' complaining that "control
     * reaches end of non-void function" if we use the following
     * functions to termiante main() or some other non-void function. 
     */
#define NORETURN __attribute__ ((__noreturn__))
#elif __CLANG__ 
    /* This macro stops 'clang -Wall' complaining that "control
     * reaches end of non-void function" if we use the following
     * functions to termiante main() or some other non-void function. 
     */
#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

void errExit(const char *format, ...) NORETURN;
void err_exit(const char *format, ...) NORETURN;
void errExitEN(int errnum, const char *format, ...) NORETURN;
void fatal(const char *format, ...) NORETURN;
void usageErr(const char *format, ...) NORETURN;
void cmdLineErr(const char *format, ...) NORETURN;

#endif /* ERROR_FUNCTIONS_H */
