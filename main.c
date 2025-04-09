#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "main.h"
struct trie *rootTrie = NULL;
struct trie *trieTail = NULL;

#include "nasm_generator.h"

#define MAX_TOKEN_LENGTH 32
#define MAX_LINE 256
#define MAX_TOKENS 100

enum TokenType {
    T_BEGIN, T_END, T_READ, T_WRITE, T_IDENTIFIER, T_INTEGER, T_ASSIGN, T_SEMICOLON, T_PLUS, T_MINUS, T_UNKNOWN
};

struct Token {
    enum TokenType type;
    char value[MAX_TOKEN_LENGTH];
};

FILE *bssFile;
FILE *textFile;

char *source_code;
int pos = 0;

int variables[26]; // asume que las variables son solo letras de 'a' a 'z'


char current_char() {
    return source_code[pos];
}

void next_char() {
    pos++;
}

void skip_comment() {
    if (current_char() == '-') {
        while (current_char() != '\n' && current_char() != '\0') {
            next_char();
        }
    }
}

void skip_whitespace() {
    while (isspace(current_char()) || current_char() == '-') {
        if (current_char() == '-') {
            skip_comment(); // IGNORAR EL COMENTARIO COMPLETOOOO
        } else {
            next_char();
        }
    }
}

void error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

/* Analiza un número entero*/
struct Token scan_integer() {
    struct Token token;
    token.type = T_INTEGER;
    int i = 0;
    while (isdigit(current_char())) {
        token.value[i++] = current_char();
        next_char();
    }
    token.value[i] = '\0';
    return token;
}

/* Analiza nombres de variables y palabras clave*/
struct Token scan_identifier() {
    struct Token token;
    int i = 0;
    while (isalnum(current_char())) {
        token.value[i++] = current_char();
        next_char();
    }
    token.value[i] = '\0';

    if (strcmp(token.value, "begin") == 0) token.type = T_BEGIN;
    else if (strcmp(token.value, "end") == 0) token.type = T_END;
    else if (strcmp(token.value, "read") == 0) token.type = T_READ;
    else if (strcmp(token.value, "write") == 0) token.type = T_WRITE;
    else token.type = T_IDENTIFIER;

    return token;
}

/*obtener el siguiente token*/
struct Token next_token() {
    skip_whitespace();
    struct Token token;
    token.type = T_UNKNOWN;

    const char c = current_char();
    if (c == '\0') return token; // CREO QUE AQUI HAY QUE CAMBIARLO POR LO DEL CAMBIO DE LINEA

    if (isalpha(c)) token = scan_identifier();
    else if (isdigit(c)) {
        token = scan_integer();
    } else if (c == '=') {
        token.type = T_ASSIGN;
        strcpy(token.value, "=");
        next_char();
    } else if (c == ';') {
        token.type = T_SEMICOLON;
        strcpy(token.value, ";");
        next_char();
    } else if (c == '+') {
        token.type = T_PLUS;
        strcpy(token.value, "+");
        next_char();
    } else if (c == '-') { skip_comment(); } else { next_char(); }

    //printf("Token leído: Tipo=%d, Valor='%s'\n", token.type, token.value); // Debug
    return token;
}

/* variables para el análisis sintáctico*/
struct Token current_token;

void next_token_from_scanner() {
    current_token = next_token();
}


/*funcion para evaluar expresiones aritméticas*/
void evaluate_expression() {
    if (current_token.type == T_INTEGER) {
        suma_entero_nasm(textFile, current_token.value);
        next_token_from_scanner();
    } else if (current_token.type == T_IDENTIFIER) {
        // // accede a la variable y le asigna el valor
        char identifier[MAX_TOKEN_LENGTH];
        strcpy(identifier, current_token.value);
        if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
            error("Variable no declarada previamente;");
        }
        suma_variable_nasm(textFile, current_token.value);
        next_token_from_scanner();
    } else {
        error("Se esperaba un número entero o un identificador.");
    }

    //aqui se evalua el resto de la expresión de sumas
    while (current_token.type == T_PLUS ) { // le quité el || current_token.type == T_MINUS que estaba dentro del while
        char op = current_token.value[0];
        next_token_from_scanner();
        if (current_token.type == T_INTEGER) {
            suma_entero_nasm(textFile, current_token.value);
            next_token_from_scanner();
        } else if (current_token.type == T_IDENTIFIER) {
            char identifier[MAX_TOKEN_LENGTH];
            strcpy(identifier, current_token.value);
            if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
                error("Variable no declarada previamente;");
            }
            suma_variable_nasm(textFile, current_token.value);
            next_token_from_scanner();
        } else {
            error("Se esperaba un número entero o un identificador.");
        }
    }
}

/*análisis de asignaciones*/
void parse_assign() {
    if (current_token.type != T_IDENTIFIER) error("Se esperaba un identificador.");

    char identifier[MAX_TOKEN_LENGTH];
    strcpy(identifier, current_token.value);

    if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) { // o sea, si no encuentra la variable
        int temp = trie_insert(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH));
        if (temp == -1) {
            error("No se pudo insertar el valor en el trie.");
        }
        // else // Se debe escribir la variable en el .bss
        else {
            assignment_bss_nasm(bssFile, identifier); // lo escribe en el output.asm
        }
    }
    next_token_from_scanner();

    if (current_token.type != T_ASSIGN) error("Se esperaba '='.");
    next_token_from_scanner();

    iniciar_asignacion_nasm(textFile);
    evaluate_expression();
    terminar_asignacion_nasm(textFile, identifier);

    if (current_token.type != T_SEMICOLON) error("Se esperaba ';'.");
    next_token_from_scanner();
}

void parse_io_operation() {
    if (current_token.type != T_READ && current_token.type != T_WRITE) {
        error("Se esperaba 'read' o 'write'.");
    }

    char operation[MAX_TOKEN_LENGTH];
    strcpy(operation, current_token.value);
    next_token_from_scanner(); // aqiu avanzamos al siguiente token

    if (current_token.type != T_IDENTIFIER) {
        error("Se esperaba un identificador después de 'read' o 'write'.");
    }

    char identifier[MAX_TOKEN_LENGTH];
    strcpy(identifier, current_token.value);
    next_token_from_scanner(); // aqui tambien avanzamos al siguiente token

    // Si la operación es "read", pedimos al user que meta un valor
    if (strcmp(operation, "read") == 0) {
        if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
            int temp = 0;
            temp = trie_insert(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH));
            if (temp == -1) {
                error("No se pudo insertar la variable en el trie.");
            }
            // iba aquí
            assignment_bss_nasm(bssFile, identifier);

        }
        read_nasm(textFile, identifier);// va as[i no? si no esta lo asigna en el .bss y despues lo lee independientemente si esta o no
        // si, correcto
    }
    // Si la operación es "write", imprimimos el valor de la variable
    else if (strcmp(operation, "write") == 0) {
        // se imprime el valor de la variable pero primero verificamos si está o no
        if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
            error("Variable no declarada previamente;");
        }
        write_nasm(textFile, identifier);
    }

    // se ve si el siguiente token es un punto y coma
    if (current_token.type != T_SEMICOLON) {
        error("Se esperaba ';' después de la operación.");
    }
    next_token_from_scanner(); // vamos al siguiente punto y coma
}

void parse_statement() {
    if (current_token.type == T_IDENTIFIER) parse_assign();
    else if (current_token.type == T_READ || current_token.type == T_WRITE) parse_io_operation();
    else error("Instrucción no válida.");
}

void parse_statements() {
    while (current_token.type != T_END) {
        parse_statement();
    }
}

void parse_program() {
    if (current_token.type != T_BEGIN) error("Se esperaba 'begin'.");
    next_token_from_scanner();

    if (current_token.type == T_SEMICOLON) {
        next_token_from_scanner();
    }

    parse_statements();

    if (current_token.type != T_END) error("Se esperaba 'end'.");
    next_token_from_scanner();
    // xd
    // suave solo que no va a qui, va en el main
}

int main(const int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.mic>\n", argv[0]);
        return EXIT_FAILURE;
    }

    bssFile = fopen("output.asm", "w"); // TODO: IMPORTANTE, YA NO SE LLAMA OUTPUT.BSS AHORA ES OUTPUT.ASM
    textFile = fopen("output.text", "w");

    //todo: aqui se debe escribir lo de las funciones en el output.bss y lo de segment .bss
    begin_nasm(bssFile); // se inicia el codigo en el archivo final
    functions_nasm(textFile); // se inicia el archivo temporal

    FILE *input = fopen(argv[1], "r");
    if (!input) error("Error al abrir el archivo.");


    int ret = 0;
    ret = trie_new(&rootTrie);
    if (-1 == ret) error("No se pudo crear el trie.");


    fseek(input, 0, SEEK_END);
    long length = ftell(input);
    fseek(input, 0, SEEK_SET);

    source_code = malloc(length + 1);
    if (!source_code) {
        perror("Error de memoria.");
        fclose(input);
        return EXIT_FAILURE;
    }
    fread(source_code, 1, length, input);
    source_code[length] = '\0';
    fclose(input);

    next_token_from_scanner();
    parse_program();

    end_nasm(textFile); // MAEEEEEE OMG ERA ESO JAJAJA NO LO PUSIMOS

    fclose(textFile); // cierra y flushea el .text

    textFile = fopen("output.text", "r");
    if (textFile == NULL) {
        perror("Error abriendo output.text.");
    }
    char line[256];
    while (fgets(line, sizeof(line), textFile)) {
        fputs(line, bssFile);
    }

    fclose(textFile);
    fclose(bssFile);
    free(source_code);

    //Todo: lo que sigue es borrar el archivo output.text
    if (remove("output.text") != 0) {
        error("No se eliminó el archivo temporal.");
    }

    // todo: y lo último, despues de que se borra el output.text se ejecuta el output.asm por medio de los comandos del archivo test_abrir_asm.c

    // Aqui se abre y se ejecuta el output.asm:
    // compilarlo
        if(system("nasm -f elf32 output.asm -o output.o") != 0) {
            perror("Error en ensamblar el archivo de codigo nasm.");
            return 1;
        }

    // enlazarlo
    if(system("ld -m elf_i386 output.o -o output") != 0) {
        perror("Error al enlazar el archivo de codigo nasm.");
        return 1;
    }

    // ejecutarlo
    if(system("./output") != 0) {
        perror("Error al ejecutar el archivo de codigo nasm.");
        return 1;
    }
    // okay ya voy a probarlo, que nervios omg
    // dele
    // dio error el remove vea telegram
    return EXIT_SUCCESS;
}
