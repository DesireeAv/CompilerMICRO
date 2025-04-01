section .bss
    x resb 10

section .text
   global _start
_start:
    ; Leer número desde la entrada estándar
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin
    mov ecx, r        ; dirección del buffer
    mov edx, 10         ; número máximo de bytes
    int 0x80           ; llamada al sistema
    ; Leer número desde la entrada estándar
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin
    mov ecx, t        ; dirección del buffer
    mov edx, 10         ; número máximo de bytes
    int 0x80           ; llamada al sistema
    ; Leer número desde la entrada estándar
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin
    mov ecx, r        ; dirección del buffer
    mov edx, 10         ; número máximo de bytes
    int 0x80           ; llamada al sistema

    ; Leer número desde la entrada estándar
    mov eax, 3          ; sys_read
    mov ebx, 0          ; stdin
    mov ecx, r        ; dirección del buffer
    mov edx, 10         ; número máximo de bytes
    int 0x80           ; llamada al sistema
