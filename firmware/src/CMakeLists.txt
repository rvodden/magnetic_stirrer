add_avr_executable(MagneticStirrer main.c)

find_library(C_LIB c)
message(STATUS "avr-libc: ${C_LIB}")