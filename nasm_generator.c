//
// Created by desiree on 31/03/25.
//

#ifndef NASM_GENERATOR_H
#define NASM_GENERATOR_H

#endif //NASM_GENERATOR_H




#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "main.h"

int write_counter = 0;

/* Esta función escribe en otro archivo el código de NASM para guardarle un espacio a la variable nueva escrita en test.mic,
 * hay que llamarla cada vez que hay una asignación (como x = 9;), y manejar otra cosa por si se encuentra una misma variable pero que cambia,
 * como "x = 9; y = 10; x = 8-10; "
 */
void assignment_bss_nasm(FILE *file, const char *variable_name) {
    fprintf(file,"    %s resb 10\n\n",variable_name);
}

/* Esta función escribe en otro archivo el código de NASM para leer un número (variable) desde la terminal */
void read_nasm(FILE *file, const char *variable_name) {
    fprintf(file,
         "    ; Leer número desde la entrada estándar\n"
     "    mov eax, 3          ; sys_read\n"
     "    mov ebx, 0          ; stdin\n"
     "    mov ecx, %s         ; dirección del buffer\n"
     "    mov edx, 10         ; número máximo de bytes\n"
     "    int 0x80            ; llamada al sistema\n\n", variable_name);
}


/* Esta función escribe en otro archivo el código de NASM para escribir el valor de la variable en la terminal */
void write_nasm_inline(FILE *file, const char *variable_name) {
  write_counter++;
    fprintf(file,
        "    ; Escribir la variable %s\n"
        "    mov eax, [%s]          ; Cargar valor\n"
        "    mov ecx, buffer_%d     ; Puntero al buffer\n"
        "    add ecx, 10            ; Posicionarse al final del buffer\n"
        "convert_loop_%d:\n"
        "    dec ecx\n"
        "    xor edx, edx\n"
        "    mov ebx, 10\n"
        "    div ebx\n"
        "    add dl, '0'\n"
        "    mov [ecx], dl\n"
        "    test eax, eax\n"
        "    jnz convert_loop_%d\n"
        "    mov eax, 4             ; sys_write\n"
        "    mov ebx, 1             ; stdout\n"
        "    mov edx, 10            ; longitud máxima\n"
        "    sub edx, ecx\n"
        "    int 0x80\n\n",
        variable_name, variable_name, write_counter, write_counter, write_counter
    );
}

/*
_start:
    ; Read x
    mov rax, 1
    mov rdi, 1
    syscall

    call read_input
    mov [x], rax

    ; Read y
    mov rax, 1
    mov rdi, 1
    syscall

    call read_input
    mov [y], rax

read_input:
    ; Read input
    mov rax, 0          ; sys_read
    mov rdi, 0          ; stdin
    mov rsi, buffer
    mov rdx, 20
    syscall

    ; Convert to integer
    call convert_string_to_int
    ret

convert_string_to_int:
    xor rax, rax
    xor rcx, rcx

    convert_loop:
        mov dl, [buffer + rcx]
        cmp dl, 0xA     ; Stop at newline
        je convert_done
        sub dl, '0'
        imul rax, 10
        add rax, rdx
        inc rcx
        jmp convert_loop
    convert_done:
        ret
*/

// Genera una asignación NASM con suma de enteros y/o variables
void generate_assignment_nasm(FILE *out, const char *dest, char **operands, int operand_count) {
    fprintf(out, "    ; Asignación: %s = ...\n", dest);

    // Primer operando
    if (isdigit(operands[0][0])) {
        fprintf(out, "    mov eax, %s\n", operands[0]);
    } else {
        fprintf(out, "    mov eax, [%s]\n", operands[0]);
    }

    // Siguientes operandos
    for (int i = 1; i < operand_count; i++) {
        if (isdigit(operands[i][0])) {
            fprintf(out, "    add eax, %s\n", operands[i]);
        } else {
            fprintf(out, "    add eax, [%s]\n", operands[i]);
        }
    }

    // Guardar resultado
    fprintf(out, "    mov [%s], eax\n\n", dest);
}




/* Esta función escribe en otro archivo el código de NASM para terminar el programa, uso de syscall */
void end_nasm(FILE *file) {
    fprintf(file,
    "    ; Salir del programa\n"
        "    mov eax, 1          ; System call number for exit\n"
        "    xor ebx, ebx        ; Return code 0 (success)\n"
        "    syscall             ; Trigger the system call\n");
}

/* Esta función debe tener los nombres de todas las variables que van a haber en el código, o bien, la función que tiene todas las variables tiene que llamar a la función
 * write_bss y también a la de inicio y luego a las de ejecución (las correspondientes de read, write, asignación, suma, resta, Y TAMBIÉN: begin y end)*/
void generate_nasm_code(const char *variable_name) {
    FILE *file = fopen("test.asm", "w");
    if (!file) {
        perror("Error al crear el archivo");
        exit(EXIT_FAILURE);
    }
    fprintf(file,
            "section .bss\n");
    assignment_bss_nasm(file, variable_name); // TODO: Esto se debe de hacer en un for() para el nombre de todas las nuevas variables que hay
    fprintf(file,
        "section .text\n"
        "   global _start\n"
        "_start:\n");
    read_nasm(file, variable_name);
    end_nasm(file);

    fclose(file);
    // printf("Archivo test.asm generado con éxito.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_variable>\n", argv[0]);
        return EXIT_FAILURE;
    }
    generate_nasm_code(argv[1]);
    return EXIT_SUCCESS;
}
