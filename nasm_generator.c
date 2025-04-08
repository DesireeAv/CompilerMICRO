//
// Created by desiree on 31/03/25.
//

#ifndef NASM_GENERATOR_H
#define NASM_GENERATOR_H

#endif //NASM_GENERATOR_H




#include <stdio.h>
#include <stdlib.h>

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
    printf("Archivo test.asm generado con éxito.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_variable>\n", argv[0]);
        return EXIT_FAILURE;
    }
    generate_nasm_code(argv[1]);
    return EXIT_SUCCESS;
}
