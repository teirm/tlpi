# binaries from the fileio chapter of TLPI

# include common include files for all the binaries built
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

link_libraries(tlpi)

add_executable(atomic_append atomic_append.c)
add_executable(begin_append begin_append.c)
add_executable(copy copy.c)
add_executable(copy_with_holes copy_with_holes.c)
add_executable(dup_check dup_check.c)
add_executable(dup_implementations dup_implementations.c)
add_executable(large_file large_file.c)
add_executable(my_scatter_io my_scatter_io.c)
add_executable(my_tee my_tee.c)
add_executable(seek_io seek_io.c)
add_executable(t_readv.c t_readv.c)
add_executable(unlink_file unlink_file.c)

