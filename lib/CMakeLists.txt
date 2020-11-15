# Library of common functions used in TLPI
# programs

add_library(tlpi
    become_daemon.c
    binary_sems.c
    cap_manip.c
    curr_time.c
    error_functions.c
    get_num.c
    print_rlimit.c
    print_wait_status.c
    signal_functions.c
    ugid_functions.c
    )

target_include_directories(tlpi PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
