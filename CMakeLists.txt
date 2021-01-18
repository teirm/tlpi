cmake_minimum_required(VERSION 3.18)

# Examples from The Linux Programming interface
# along with additional work for newer interfaces
# Only C code, so don't test for C++ compiler
project(tlpi VERSION 1.0 LANGUAGES C)

# add common library sub directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

#add file io binaries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fileio)

# add file and directory binaries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/files)
