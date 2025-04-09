section .data
    prompt db "Enter variable: ", 0
    len equ $ - prompt
    buffer times 20 db 0

segment .bss                   ; donde vamos a declarar todas las variables
    o resd 1
    xyz resd 1
    rrrrrrr resd 1
    fofo resd 1
    m resd 1
section .text
    global _start

read_input:
; Leer entrada
    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, 20
    int 0x80
    ; Convertir a entero
    call convert_string_to_int
    ret

convert_string_to_int:
    xor eax, eax
    xor ecx, ecx

convert_loop:
    mov dl, [buffer + ecx]
    cmp dl, 0xA
    je convert_done
    sub dl, '0'
    movzx edx, dl
    imul eax, 10
    add eax, edx
    inc ecx
    jmp convert_loop

convert_done:
    ret

_start:
    ; Leer
    mov eax, 4
    mov ebx, 1     ; le quite lo de hacer el prompt
    int 0x80
    call read_input
    mov [o], eax

    mov eax, 0
    add eax, 10
    add eax, 5
    mov [xyz], eax
    mov eax, 0
    add eax, 09
    mov [rrrrrrr], eax
    ; Escribir la variable rrrrrrr
    mov eax, [rrrrrrr]          ; Cargar valor
    mov ecx, buffer        ; Puntero al buffer
    add ecx, 10            ; Posicionarse al final del buffer
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
    ; Añadir salto de linea después del número
    mov byte [esi], 10     ; <- Esi sigue siendo buffer+10
    mov eax, 4             ; sys_write
    mov ebx, 1             ; stdout
    mov edx, esi            ; longitud máxima
    sub edx, ecx           ; longitud = fin - inicio
    add edx, 1             ; incluir salto de linea 
    int 0x80
    ; Leer
    mov eax, 4
    mov ebx, 1     ; le quite lo de hacer el prompt
    int 0x80
    call read_input
    mov [rrrrrrr], eax

    ; Escribir la variable rrrrrrr
    mov eax, [rrrrrrr]          ; Cargar valor
    mov ecx, buffer        ; Puntero al buffer
    add ecx, 10            ; Posicionarse al final del buffer
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
    ; Añadir salto de linea después del número
    mov byte [esi], 10     ; <- Esi sigue siendo buffer+10
    mov eax, 4             ; sys_write
    mov ebx, 1             ; stdout
    mov edx, esi            ; longitud máxima
    sub edx, ecx           ; longitud = fin - inicio
    add edx, 1             ; incluir salto de linea 
    int 0x80
    mov eax, 0
    add eax, 0
    add eax, 5
    add eax, [rrrrrrr]
    add eax, 7
    add eax, 1
    mov [fofo], eax
    ; Escribir la variable fofo
    mov eax, [fofo]          ; Cargar valor
    mov ecx, buffer        ; Puntero al buffer
    add ecx, 10            ; Posicionarse al final del buffer
    mov esi, ecx           ; Guardamos el puntero final en esi (para calcular longitud luego)
convert_loop_3:
    dec ecx
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, '0'
    mov [ecx], dl
    test eax, eax
    jnz convert_loop_3
    ; Añadir salto de linea después del número
    mov byte [esi], 10     ; <- Esi sigue siendo buffer+10
    mov eax, 4             ; sys_write
    mov ebx, 1             ; stdout
    mov edx, esi            ; longitud máxima
    sub edx, ecx           ; longitud = fin - inicio
    add edx, 1             ; incluir salto de linea 
    int 0x80
    ; Escribir la variable xyz
    mov eax, [xyz]          ; Cargar valor
    mov ecx, buffer        ; Puntero al buffer
    add ecx, 10            ; Posicionarse al final del buffer
    mov esi, ecx           ; Guardamos el puntero final en esi (para calcular longitud luego)
convert_loop_4:
    dec ecx
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, '0'
    mov [ecx], dl
    test eax, eax
    jnz convert_loop_4
    ; Añadir salto de linea después del número
    mov byte [esi], 10     ; <- Esi sigue siendo buffer+10
    mov eax, 4             ; sys_write
    mov ebx, 1             ; stdout
    mov edx, esi            ; longitud máxima
    sub edx, ecx           ; longitud = fin - inicio
    add edx, 1             ; incluir salto de linea 
    int 0x80
    mov eax, 0
    add eax, 10
    add eax, [xyz]
    mov [m], eax
    mov eax, 0
    add eax, [m]
    add eax, [m]
    add eax, [m]
    add eax, [m]
    mov [m], eax
    ; Escribir la variable m
    mov eax, [m]          ; Cargar valor
    mov ecx, buffer        ; Puntero al buffer
    add ecx, 10            ; Posicionarse al final del buffer
    mov esi, ecx           ; Guardamos el puntero final en esi (para calcular longitud luego)
convert_loop_5:
    dec ecx
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, '0'
    mov [ecx], dl
    test eax, eax
    jnz convert_loop_5
    ; Añadir salto de linea después del número
    mov byte [esi], 10     ; <- Esi sigue siendo buffer+10
    mov eax, 4             ; sys_write
    mov ebx, 1             ; stdout
    mov edx, esi            ; longitud máxima
    sub edx, ecx           ; longitud = fin - inicio
    add edx, 1             ; incluir salto de linea 
    int 0x80
    ; Salir del programa
    mov eax, 1          ; System call number for exit
    xor ebx, ebx        ; Return code 0 (success)
    int 0x80             ; Trigger the system call
