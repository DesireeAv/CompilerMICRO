//
// Created by desiree on 31/03/25.
//

#ifndef NASM_GENERATOR_H
#define NASM_GENERATOR_H

#endif //NASM_GENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int write_counter = 0;
FILE * OUT_TEMP;

/* Esta función escribe en otro archivo el código de NASM para guardarle un espacio a la variable nueva escrita en test.mic,
 * hay que llamarla cada vez que hay una asignación (como x = 9;), y manejar otra cosa por si se encuentra una misma variable pero que cambia,
 * como "x = 9; y = 10; x = 8-10; "
 */

void begin_nasm(FILE *file) {
    fprintf(file,
            "section .data\n"
            "    prompt db \"Enter variable: \", 0\n"
            "    len equ $ - prompt\n"
            "    buffer times 20 db 0\n\n"
            "segment .bss                   ; donde vamos a declarar todas las variables\n");
}

void assignment_bss_nasm(FILE *file, const char *variable_name) {
    fprintf(file,"    %s resd 1\n",variable_name);
}

/* Esta función escribe en otro archivo el código de NASM para leer un número (variable) desde la terminal */
void read_nasm(FILE *file, const char *variable_name) {
    fprintf(file,
    "    ; Leer\n"
    ";    mov eax, 4 ; ESTO SE QUITA PORQUE NO VA AL CASO, SOLO IMPRIME BASURA\n"
    ";    mov ebx, 1     ; le quite lo de hacer el prompt\n"
    ";    int 0x80\n"
    "    call read_input\n"
    "    mov [%s], eax\n\n", variable_name);
}

void functions_nasm(FILE *file) {
    fprintf(file,
        "section .text\n"
        "    global _start\n\n"

        "read_input:\n"
        "; Leer entrada\n"
        "    mov eax, 3\n"
        "    mov ebx, 0\n"
        "    mov ecx, buffer\n"
        "    mov edx, 20\n"
        "    int 0x80\n"

        "    ; Convertir a entero\n"
        "    call convert_string_to_int\n"
        "    ret\n\n"

        "convert_string_to_int:\n"
        "    xor eax, eax\n"
        "    xor ecx, ecx\n\n"

        "convert_loop:\n"
        "    mov dl, [buffer + ecx]\n"
        "    cmp dl, 0xA\n"
        "    je convert_done\n"
        "    sub dl, '0'\n"
        "    movzx edx, dl\n"
        "    imul eax, 10\n"
        "    add eax, edx\n"
        "    inc ecx\n"
        "    jmp convert_loop\n\n"

        "convert_done:\n"
        "    ret\n\n"

        "_start:\n");
}

/* Esta función escribe en otro archivo el código de NASM para escribir el valor de la variable en la terminal */
void write_nasm(FILE *file, const char *variable_name) {
  write_counter++;
    fprintf(file,
        "    ; Escribir la variable %s\n"
        "    mov eax, [%s]          ; Cargar valor\n"
        "    mov ecx, buffer        ; Puntero al buffer\n"
        "    add ecx, 10            ; Posicionarse al final del buffer\n"
        "    mov esi, ecx           ; Guardamos el puntero final en esi (para calcular longitud luego)\n"

        "convert_loop_%d:\n"
        "    dec ecx\n"
        "    xor edx, edx\n"
        "    mov ebx, 10\n"
        "    div ebx\n"
        "    add dl, '0'\n"
        "    mov [ecx], dl\n"
        "    test eax, eax\n"
        "    jnz convert_loop_%d\n"
        "    ; Añadir salto de linea después del número\n"
        "    mov byte [esi], 10     ; <- Esi sigue siendo buffer+10\n"
        "    mov eax, 4             ; sys_write\n"
        "    mov ebx, 1             ; stdout\n"
        "    mov edx, esi            ; longitud máxima\n"
        "    sub edx, ecx           ; longitud = fin - inicio\n"
        "    add edx, 1             ; incluir salto de linea \n"
        "    int 0x80\n",
        variable_name, variable_name, write_counter, write_counter
    );
}


// TODO: HACER EL OUT_TEMP GLOBAL QUE VA A SER EL ACHOVO QUE VAMOS A BORRAR el temporal duh
void suma_variable_nasm(FILE *file, char * variable_name) {
    fprintf(file, "    add eax, [%s]\n", variable_name);
}

void suma_entero_nasm(FILE *file, char * variable_name) {
    fprintf(file, "    add eax, %s\n", variable_name);
}

void iniciar_asignacion_nasm(FILE *file) {
    fprintf(file, "    mov eax, 0\n");
}

void terminar_asignacion_nasm(FILE *file, char * variable_name) {
    fprintf(file, "    mov [%s], eax\n", variable_name);
}

// Genera una asignación NASM con suma de enteros y/o variables
/*
void generate_assignment_nasm(FILE *out, const char *dest, char **operands, int operand_count) {
    fprintf(out, "    ; Asignación: %s = ...\n", dest);

    // Primer operando
    if (isdigit(operands[0][0])) {
        fprintf(out, "    mov eax, %s\n", operands[0]);
    }
    else {
        fprintf(out, "    mov eax, [%s]\n", operands[0]);
    }

    // Siguientes operandos
    for (int i = 1; i < operand_count; i++) {
        if (isdigit(operands[i][0])) {
            fprintf(out, "    add eax, %s\n", operands[i]);
        }
        else {
            fprintf(out, "    add eax, [%s]\n", operands[i]);
        }
    }

    // Guardar resultado
    fprintf(out, "    mov [%s], eax\n\n", dest);
}
*/



/* Esta función escribe en otro archivo el código de NASM para terminar el programa, uso de syscall */
void end_nasm(FILE *file) {
    fprintf(file,
    "    ; Salir del programa\n"
        "    mov eax, 1          ; System call number for exit\n"
        "    xor ebx, ebx        ; Return code 0 (success)\n"
        "    int 0x80             ; Trigger the system call\n");
}

/* Esta función debe tener los nombres de todas las variables que van a haber en el código, o bien, la función que tiene todas las variables tiene que llamar a la función
 * write_bss y también a la de inicio y luego a las de ejecución (las correspondientes de read, write, asignación, suma, resta, Y TAMBIÉN: begin y end)*/
