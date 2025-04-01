cmake_minimum_required(VERSION 3.28)
project(micro C)

set(CMAKE_C_STANDARD 11)

add_executable(micro
        main.c
        nasm_generator.c
        nasm_generator.h
)
