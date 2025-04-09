; nasm -f elf test3.asm -o test.o
; ld -m elf_i386 test.o -o test
; ./test

section .data
    buffer times 20 db 0

segment .bss
    x resd 1
    y resd 1
    v resd 1

section .text

global _start

_start:
    ; Asignación: y = ...
    mov eax, 7
    add eax, 3
    add eax, 9
    add eax, 0
    add eax, 2
    add eax, 8
    mov [y], eax

    ; Asignación: v = ...
    mov eax, 7
    add eax, 3
    add eax, 9
    add eax, 0
    add eax, 2
    add eax, 108
    mov [v], eax

    ; Asignación: y = ...
    mov eax, [v]
    add eax, 32600
    add eax, [x]
    mov [y], eax




        ; write imprimir v, write v;
    mov eax, [v]
    mov ecx, buffer
    add ecx, 10
    mov esi, ecx           ; Guardamos el puntero final en esi (para calcular longitud luego)

convert_loop_1:
    dec ecx
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, '0'
    mov [ecx], dl
    test eax, eax
    jnz convert_loop_1

    ; Añadir '\n' después del número
    mov byte [esi], 10     ; <- Esi sigue siendo buffer+10

    ; Calcular longitud a imprimir
    mov eax, 4
    mov ebx, 1
    mov edx, esi
    sub edx, ecx           ; longitud = fin - inicio
    add edx, 1             ; incluir '\n'
    int 0x80



        ; write imprimir v, write v;
    mov eax, [y]
    mov ecx, buffer
    add ecx, 10
    mov esi, ecx           ; Guardamos el puntero final en esi (para calcular longitud luego)

convert_loop_2:
    dec ecx
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, '0'
    mov [ecx], dl
    test eax, eax
    jnz convert_loop_2

    ; Añadir '\n' después del número
    mov byte [esi], 10     ; <- Esi sigue siendo buffer+10

    ; Calcular longitud a imprimir
    mov eax, 4
    mov ebx, 1
    mov edx, esi
    sub edx, ecx           ; longitud = fin - inicio
    add edx, 1             ; incluir '\n'
    int 0x80



    ; Salir del programa
    mov eax, 1          ; System call number for exit
    xor ebx, ebx        ; Return code 0 (success)
    int 0x80             ; Trigger the system call
