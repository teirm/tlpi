# binaries from the files and directory chapter of TLPI

# include common include files for all the binaries built
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

link_libraries(tlpi)

add_executable(chmod_arX chmod_arX.c)
add_executable(my_chattr my_chattr.c)
add_executable(my_eaccess my_eaccess.c)

