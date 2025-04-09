section .data
    prompt db "Enter variable: ", 0
    len equ $ - prompt


    buffer times 20 db 0

section .bss
    x resd 1               ; 32 bits ahora
    y resd 1
    z resd 1

section .text
    global _start

_start:
    ; Leer x
    mov eax, 4             ; sys_write
    mov ebx, 1             ; stdout
    mov ecx, prompt
    mov edx, len
    int 0x80

    call read_input
    mov [x], eax

    ; Leer y
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt
    mov edx, len_y
    int 0x80

    call read_input
    mov [y], eax

    ; Leer z
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt
    mov edx, len_z
    int 0x80

    call read_input
    mov [z], eax

    ; Suma
    mov eax, [x]
    add eax, [y]
    add eax, [z]
    mov [x], eax

    ; Salida (podrías mostrar el resultado aquí)
    mov eax, 1             ; sys_exit
    mov ebx, 0
    int 0x80

read_input:
    ; Leer entrada
    mov eax, 3             ; sys_read
    mov ebx, 0             ; stdin
    mov ecx, buffer
    mov edx, 20
    int 0x80

    ; Convertir a entero
    call convert_string_to_int
    ret

convert_string_to_int:
    xor eax, eax
    xor ecx, ecx           ; Índice

convert_loop:
    mov dl, [buffer + ecx]
    cmp dl, 0xA            ; Fin de línea
    je convert_done
    sub dl, '0'
    movzx edx, dl          ; Extender a 32 bits
    imul eax, 10
    add eax, edx
    inc ecx
    jmp convert_loop

convert_done:
    ret