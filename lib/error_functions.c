#include <stdarg.h>
#include <lib/error_functions.h>
#include <lib/tlpi_hdr.h>
#include "ename.c.inc"

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#elif __CLANG__
__attribute__ ((__noreturn__))
#endif
static void
terminate(Boolean useExit3)
{
    char *s;
    
    /* Dump core if EF_DUMPCORE environment variable is defined and 
     * is a nonempty string; otherwise call exit(3) or _exit(2),
     * depending on value of 'useExit3'.
     */

    s = getenv("EF_DUMPCORE");

    if (s != NULL && *s != '\0') {
        abort();
    } else if (useExit3) {
        exit(EXIT_FAILURE);
    } else {
        _exit(EXIT_FAILURE);
    }
}

static void
outputError(Boolean useErr, int err, Boolean flushStdout,
        const char *format, va_list ap)
{
#define BUF_SIZE 500
    char buf[BUF_SIZE];
    char userMsg[BUF_SIZE]; 
    char errText[BUF_SIZE];

    vsnprintf(userMsg, BUF_SIZE, format, ap);

    if (useErr) {
        snprintf(errText, BUF_SIZE, " [%s, %s]",
                (err > 0 && err <= MAX_ENAME) ?
                ename[err] : "?UNKNOWN?", strerror(err));
    } else {
        snprintf(errText, BUF_SIZE, ":");
    }

    snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);

    if (flushStdout) {
        /* flush any pending output */
        fflush(stdout);
    }
    fputs(buf, stderr);
    /* in case stderr is not line buffered */
    fflush(stderr);
}

void 
errMsg(const char *format, ...)
{
    va_list argList;
    int savedErrno;

    /* in case we change it here */
    savedErrno = errno;

    va_start(argList, format);
    outputError(TRUE, errno, TRUE, format, argList);
    va_end(argList);

    errno = savedErrno;
}

void 
errExit(const char *format, ...) 
{
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errno, TRUE, format, argList);
    va_end(argList);

    terminate(TRUE);
}

void
err_exit(const char *format, ...) 
{
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errno, FALSE, format, argList);
    va_end(argList);

    terminate(FALSE);
}

void
errExitEN(int errnum, const char *format, ...)
{
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errnum, TRUE, format, argList);
    va_end(argList);

    terminate(TRUE);
}

void
fatal(const char *format, ...)
{
    va_list argList;
    
    va_start(argList, format);
    outputError(FALSE, 0, TRUE, format, argList);
    va_end(argList);

    terminate(TRUE);
}

void
usageErr(const char *format, ...)
{
    va_list argList;
    
    /* flush any pending output */
    fflush(stdout); 

    fprintf(stderr, "Usage: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);
    
    /* in case stderr is not line-buffered */
    fflush(stderr);
    exit(EXIT_FAILURE);
}

void 
cmdLineErr(const char *format, ...)
{
    va_list argList;
    
    /* flush any pending output */
    fflush(stdout); 

    fprintf(stderr, "Command-line usage error: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);
    
    /* in case stderr is not line-buffered */
    fflush(stderr);
    exit(EXIT_FAILURE);
}
