#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct Simbolo {
    char *nombre;
    char *tipo;
    int scope;
    struct Simbolo *siguiente;
} Simbolo;

typedef struct TablaSimbolos {
    Simbolo *primero;
    int scope_actual;
} TablaSimbolos;

void inicializar_tabla(TablaSimbolos *tabla);
void liberar_tabla(TablaSimbolos *tabla);
void entrar_scope(TablaSimbolos *tabla);
void salir_scope(TablaSimbolos *tabla);
int insertar_simbolo(TablaSimbolos *tabla, const char *nombre, const char *tipo);
Simbolo *buscar_simbolo(TablaSimbolos *tabla, const char *nombre);
Simbolo *buscar_simbolo_en_scope(TablaSimbolos *tabla, const char *nombre, int scope);
void imprimir_tabla(TablaSimbolos *tabla);

#endif
