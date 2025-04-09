#include <stdlib.h>
#include <stdio.h>

int main() {
    // Compilar
    if(system("nasm -f elf32 test.asm -o test.o") != 0) {
        perror("Error en ensamblado");
        return 1;
    }

    // Enlazar
    if(system("ld -m elf_i386 test.o -o test") != 0) {
        perror("Error en enlace");
        return 1;
    }

    // Ejecutar
    if(system("./test") != 0) {
        perror("Error en ejecución");
        return 1;
    }

    return 0;
}