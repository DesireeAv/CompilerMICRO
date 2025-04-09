#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  // Para manejar errores de conversión
#include <ctype.h>

#include "main.h"
struct trie * rootTrie = NULL;
struct trie * trieTail = NULL;

#define MAX_TOKEN_LENGTH 32
#define MAX_LINE 256
#define MAX_TOKENS 100
int SECOND_TIME = 0;
// Definición de los tipos de tokens
enum TokenType {
    T_BEGIN, T_END, T_READ, T_WRITE, T_IDENTIFIER, T_INTEGER, T_ASSIGN, T_SEMICOLON, T_PLUS, T_MINUS, T_UNKNOWN
};

struct Token {
    enum TokenType type;
    char value[MAX_TOKEN_LENGTH];
};

// Código fuente y posición de lectura
char *source_code;
int pos = 0;

// Variables para las variables y sus valores
int variables[26]; // Asume que las variables son solo letras de 'a' a 'z'

// Funciones de manipulación del código fuente
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
            skip_comment();  // Ignorar el comentario completo
        } else {
            next_char();
        }
    }
}

void error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

// Analizar un número entero
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

// Analizar identificadores y palabras clave
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

// Obtener el siguiente token
struct Token next_token() {
    skip_whitespace();
    struct Token token;
    token.type = T_UNKNOWN;

    const char c = current_char();
    if (c == '\0') return token; // CREO QUE AQUI HAY QUE CAMBIARLO POR LO DEL CAMBIO DE LINEA

    if (isalpha(c)) token = scan_identifier();
    else if (isdigit(c)) {
        token = scan_integer();
        //if (token < 0) error("No se permiten restas ni números negativos.");
    }
    else if (c == '=') { token.type = T_ASSIGN; strcpy(token.value, "="); next_char(); }
    else if (c == ';') { token.type = T_SEMICOLON; strcpy(token.value, ";"); next_char(); }
    else if (c == '+') { token.type = T_PLUS; strcpy(token.value, "+"); next_char(); }
    else if (c == '-') { skip_comment(); }
    else { next_char(); }

    //printf("Token leído: Tipo=%d, Valor='%s'\n", token.type, token.value); // Debug
    return token;
}

// Variables para el análisis sintáctico
struct Token current_token;
void next_token_from_scanner() {
    current_token = next_token();
}


// Función para evaluar expresiones aritméticas
void evaluate_expression() {
    if (current_token.type == T_INTEGER) {
        suma_entero_nasm(current_token.value);
        next_token_from_scanner(); // Avanzar al siguiente token
    } else if (current_token.type == T_IDENTIFIER) {
        // Acceder a la variable y asignar el valor
        char identifier[MAX_TOKEN_LENGTH];
        strcpy(identifier, current_token.value);
        if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
            error("Variable no declarada previamente;");
        }
        suma_variable_nasm(current_token.value);
        next_token_from_scanner(); // Avanzar al siguiente token
    } else {
        error("Se esperaba un número entero o un identificador.");
    }

    // Evaluar el resto de la expresión
    while (current_token.type == T_PLUS || current_token.type == T_MINUS) {
        char op = current_token.value[0];
        next_token_from_scanner(); // Avanzar al siguiente token
        if (current_token.type == T_INTEGER) {
            // Usar strtol para convertir la cadena a un número entero
            suma_entero_nasm(current_token.value);
            next_token_from_scanner(); // Avanzar al siguiente token
        } else if (current_token.type == T_IDENTIFIER) {
            char identifier[MAX_TOKEN_LENGTH];
            strcpy(identifier, current_token.value);
            if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
                    error("Variable no declarada previamente;");
            }
            suma_variable_nasm(current_token.value);
            next_token_from_scanner(); // Avanzar al siguiente token
        } else {
            error("Se esperaba un número entero o un identificador.");
        }
    }
}

// Análisis de asignaciones
void parse_assign() {
    if (current_token.type != T_IDENTIFIER) error("Se esperaba un identificador.");

    char identifier[MAX_TOKEN_LENGTH];
    strcpy(identifier, current_token.value);

    if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
        int temp = 0;
        temp = trie_insert (rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH));
        if (temp == -1) {
            error("Could not insert variable into trie");
        }
        // Se debe escribir la variable en el .bss
    }
    next_token_from_scanner();

    if (current_token.type != T_ASSIGN) error("Se esperaba '='.");
    next_token_from_scanner();

    iniciar_asignacion_nasm();
    evaluate_expression(); // Evaluar la expresión
    terminar_asignacion_nasm(identifier);

    if (current_token.type != T_SEMICOLON) error("Se esperaba ';'.");
    next_token_from_scanner();
}

void parse_io_operation() {
    if (current_token.type != T_READ && current_token.type != T_WRITE) {
        error("Se esperaba 'read' o 'write'.");
    }

    char operation[MAX_TOKEN_LENGTH];
    strcpy(operation, current_token.value);
    next_token_from_scanner();  // Avanzamos al siguiente token

    if (current_token.type != T_IDENTIFIER) {
        error("Se esperaba un identificador después de 'read' o 'write'.");
    }

    char identifier[MAX_TOKEN_LENGTH];
    strcpy(identifier, current_token.value);
    next_token_from_scanner();  // Avanzamos al siguiente token

    // Si la operación es "read", pedimos al usuario que ingrese un valor
    if (strcmp(operation, "read") == 0) {
        if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
            int temp = 0;
            temp = trie_insert (rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH));
            if (temp == -1) {
                error("Could not insert variable into trie");
            }
        }
        read_nasm(identifier);
    }
    // Si la operación es "write", imprimimos el valor de la variable
    else if (strcmp(operation, "write") == 0) {
        // Imprimimos el valor de la variable
        if ((trie_search(rootTrie, identifier, strnlen(identifier, MAX_TOKEN_LENGTH), &trieTail)) == -1) {
            error("Variable no declarada previamente;");
        }
        write_nasm(identifier);
    }

    // Verificamos si el siguiente token es un punto y coma
    if (current_token.type != T_SEMICOLON) {
        error("Se esperaba ';' después de la operación.");
    }
    next_token_from_scanner();  // Avanzamos al siguiente token (punto y coma)
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

    // Omitir punto y coma después de 'begin' si está presente
    if (current_token.type == T_SEMICOLON) {
        next_token_from_scanner();
    }

    parse_statements();

    if (current_token.type != T_END) error("Se esperaba 'end'.");
    next_token_from_scanner();
}

// Función principal
int main(const int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.mic>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }

    int ret = 0;
    ret = trie_new(&rootTrie);
    if (-1 == ret) {
        error( "Could not create trie");
    }

    fseek(input, 0, SEEK_END);
    long length = ftell(input);
    fseek(input, 0, SEEK_SET);

    source_code = malloc(length + 1);
    if (!source_code) {
        perror("Error de memoria");
        fclose(input);
        return EXIT_FAILURE;
    }
    fread(source_code, 1, length, input);
    source_code[length] = '\0';
    fclose(input);

    next_token_from_scanner();
    parse_program();

    free(source_code);

    //Todo lo que sigue se debe borrar

    SECOND_TIME = 1; // PARA QUE EL PROGRAMA SEPA QUE YA SE TIENEN QUE ESCRIBIR LAS FUNCIONES Y NO SOLO LEER LAS VARIABLES
    printf("SECOND TIME\n");
    pos = 0;

    // Segunda lectura
    input = fopen(argv[1], "r");
    if (!input) {
        perror("Error al volver a abrir el archivo");
        return EXIT_FAILURE;
    }

    fseek(input, 0, SEEK_END);
    length = ftell(input);
    fseek(input, 0, SEEK_SET);

    source_code = malloc(length + 1);
    if (!source_code) {
        perror("Error de memoria en segunda lectura");
        fclose(input);
        return EXIT_FAILURE;
    }

    fread(source_code, 1, length, input);
    source_code[length] = '\0';
    fclose(input);

    next_token_from_scanner();
    parse_program();

    free(source_code);
    return EXIT_SUCCESS;
}