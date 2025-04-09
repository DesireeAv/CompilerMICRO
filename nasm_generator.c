//
// Created by desiree on 31/03/25.
//

#ifndef NASM_GENERATOR_H
#define NASM_GENERATOR_H

#endif //NASM_GENERATOR_H



int main(int argc, char *argv[]) {
    /*if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_variable>\n", argv[0]);
        return EXIT_FAILURE;
    }
    generate_nasm_code(argv[1]);*/
    FILE *file = fopen("test3.asm", "w");
    char *sum1[] = {"7", "3", "9", "0", "2", "8"};
    generate_assignment_nasm(file, "y", sum1, 6);
    char *sum3[] = {"7", "3", "9", "0", "22", "18"};
    generate_assignment_nasm(file, "v", sum1, 6);

    char *sum2[] = {"v", "2", "x"};
    generate_assignment_nasm(file, "y", sum2, 3);

    return EXIT_SUCCESS;
}

/*
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
}*/