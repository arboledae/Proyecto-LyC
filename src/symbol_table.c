#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

static char *duplicar_cadena(const char *texto) {
    char *copia = (char *)malloc(strlen(texto) + 1);
    if (!copia) {
        fprintf(stderr, "[ERROR] No se pudo reservar memoria para la tabla de simbolos.\n");
        exit(1);
    }
    strcpy(copia, texto);
    return copia;
}

void inicializar_tabla(TablaSimbolos *tabla) {
    tabla->primero = NULL;
    tabla->scope_actual = 0;
}

void liberar_tabla(TablaSimbolos *tabla) {
    Simbolo *actual = tabla->primero;
    while (actual) {
        Simbolo *siguiente = actual->siguiente;
        free(actual->nombre);
        free(actual->tipo);
        free(actual);
        actual = siguiente;
    }
    tabla->primero = NULL;
}

void entrar_scope(TablaSimbolos *tabla) {
    tabla->scope_actual++;
}

void salir_scope(TablaSimbolos *tabla) {
    Simbolo **actual = &tabla->primero;
    while (*actual) {
        if ((*actual)->scope == tabla->scope_actual) {
            Simbolo *eliminar = *actual;
            *actual = eliminar->siguiente;
            free(eliminar->nombre);
            free(eliminar->tipo);
            free(eliminar);
        } else {
            actual = &(*actual)->siguiente;
        }
    }

    if (tabla->scope_actual > 0) {
        tabla->scope_actual--;
    }
}

int insertar_simbolo(TablaSimbolos *tabla, const char *nombre, const char *tipo) {
    if (buscar_simbolo_en_scope(tabla, nombre, tabla->scope_actual)) {
        return 0;
    }

    Simbolo *nuevo = (Simbolo *)malloc(sizeof(Simbolo));
    if (!nuevo) {
        fprintf(stderr, "[ERROR] No se pudo reservar memoria para un simbolo.\n");
        exit(1);
    }

    nuevo->nombre = duplicar_cadena(nombre);
    nuevo->tipo = duplicar_cadena(tipo);
    nuevo->scope = tabla->scope_actual;
    nuevo->siguiente = tabla->primero;
    tabla->primero = nuevo;
    return 1;
}

Simbolo *buscar_simbolo(TablaSimbolos *tabla, const char *nombre) {
    Simbolo *actual = tabla->primero;
    Simbolo *mejor = NULL;

    while (actual) {
        if (strcmp(actual->nombre, nombre) == 0 &&
            (!mejor || actual->scope > mejor->scope)) {
            mejor = actual;
        }
        actual = actual->siguiente;
    }

    return mejor;
}

Simbolo *buscar_simbolo_en_scope(TablaSimbolos *tabla, const char *nombre, int scope) {
    Simbolo *actual = tabla->primero;
    while (actual) {
        if (actual->scope == scope && strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

void imprimir_tabla(TablaSimbolos *tabla) {
    Simbolo *actual = tabla->primero;

    printf("\n==================================================\n");
    printf("TABLA DE SIMBOLOS\n");
    printf("==================================================\n\n");
    printf("%-20s %-10s %-10s\n", "Nombre", "Tipo", "Scope");
    printf("%-20s %-10s %-10s\n", "------", "----", "-----");

    while (actual) {
        printf("%-20s %-10s %-10d\n", actual->nombre, actual->tipo, actual->scope);
        actual = actual->siguiente;
    }
}
