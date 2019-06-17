# Memory Allocation Notes

## Tools from glibc for dealing with memory

* mtrace and muntrace work with MALLOC_TRACE environment
variable

* mcheck and mprobe provide consistency checking.

* MALLOC_CHECK_ environment variable modifies how a running program
responds to memory errors:
    * 0 ignore
    * 1 print information to stderr
    * 2 abort()

## Malloc manipulation and information
* mallopt - modify algorithm, releasable space, when to use mmaps.
* mallinfo - return structure with various stats about memory used
by malloc

* Can align memory to a specific  power of 2 address with memalign 
or posix_memalign 
