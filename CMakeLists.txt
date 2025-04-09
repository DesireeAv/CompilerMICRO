cmake_minimum_required(VERSION 3.28)
project(micro C)

set(CMAKE_C_STANDARD 11)

add_executable(micro
        main.c
        main.h
        #test_abrir_asm.c
)
