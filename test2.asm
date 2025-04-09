section .data
    prompt_x db "Enter x: ", 0
    len_x equ $ - prompt_x

    prompt_y db "Enter y: ", 0
    len_y equ $ - prompt_y

    prompt_z db "Enter z: ", 0
    len_z equ $ - prompt_z

    buffer times 20 db 0   ; Buffer para entrada

section .bss
    x resd 1               ; Enteros de 32 bits
    y resd 1
    z resd 1

section .text
    global _start

_start:
    ; Leer x
    mov eax, 4             ; sys_write
    mov ebx, 1             ; stdout
    mov ecx, prompt_x
    mov edx, len_x
    int 0x80

    call read_input
    mov [x], eax

    ; Leer y
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_y
    mov edx, len_y
    int 0x80

    call read_input
    mov [y], eax

    ; Leer z
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_z
    mov edx, len_z
    int 0x80

    call read_input
    mov [z], eax

    ; Ejemplo: suma x + y + z
    mov eax, [x]
    add eax, [y]
    add eax, [z]
    ; Resultado en eax

    ; Salir
    mov eax, 1             ; sys_exit
    xor ebx, ebx
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
    xor eax, eax           ; Acumulador
    xor ecx, ecx           ; Índice

convert_loop:
    mov dl, [buffer + ecx] ; Cargar carácter
    cmp dl, 0xA            ; Verificar fin de línea
    je convert_done
    sub dl, '0'            ; Convertir ASCII a número
    movzx edx, dl          ; Extender a 32 bits
    imul eax, 10           ; Desplazar dígitos
    add eax, edx           ; Agregar nuevo dígito
    inc ecx                ; Siguiente carácter
    jmp convert_loop

convert_done:
    ret
