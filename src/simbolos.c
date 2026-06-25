#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simbolos.h"

#define MAX_AMBITOS 256

/* Lista de simbolos en orden de declaracion (append al final). No se poda
   al salir de un bloque: se conserva para mostrar la tabla completa. */
static Simbolo *tabla      = NULL;
static Simbolo *tabla_tail = NULL;

/* Pila de ambitos activos. pila[0..tope] son los ids visibles ahora mismo,
   del mas externo (pila[0] = 0, global) al mas interno (pila[tope]). */
static int pila[MAX_AMBITOS];
static int tope         = 0;   /* indice del ambito activo mas interno */
static int siguiente_id = 0;   /* proximo id unico de ambito a asignar */

void tabla_init(void) {
    liberar_tabla();
    tope         = 0;
    pila[0]      = 0;   /* ambito global */
    siguiente_id = 0;
}

int scope_actual(void) {
    return pila[tope];
}

int entrar_scope(void) {
    if (tope < MAX_AMBITOS - 1)
        pila[++tope] = ++siguiente_id;
    return pila[tope];
}

int salir_scope(void) {
    if (tope > 0) tope--;
    return pila[tope];
}

int declarar(const char *nombre, const char *tipo, int linea, int inicializado) {
    if (buscar_local(nombre)) return -1;   /* doble declaracion en el mismo ambito */

    Simbolo *s = (Simbolo*)malloc(sizeof(Simbolo));
    s->nombre       = strdup(nombre);
    s->tipo         = strdup(tipo);
    s->scope        = pila[tope];
    s->linea        = linea;
    s->inicializado = inicializado;
    s->siguiente    = NULL;

    if (tabla_tail) tabla_tail->siguiente = s;
    else            tabla = s;
    tabla_tail = s;
    return 0;
}

Simbolo* buscar_local(const char *nombre) {
    int sc = pila[tope];
    for (Simbolo *p = tabla; p; p = p->siguiente)
        if (p->scope == sc && strcmp(p->nombre, nombre) == 0)
            return p;
    return NULL;
}

Simbolo* buscar(const char *nombre) {
    /* visibilidad: del ambito activo mas interno al mas externo */
    for (int i = tope; i >= 0; i--) {
        int sc = pila[i];
        for (Simbolo *p = tabla; p; p = p->siguiente)
            if (p->scope == sc && strcmp(p->nombre, nombre) == 0)
                return p;
    }
    return NULL;
}

void marcar_inicializado(const char *nombre) {
    Simbolo *s = buscar(nombre);
    if (s) s->inicializado = 1;
}

void imprimir_tabla(void) {
    printf("\n==================================================\n");
    printf("TABLA DE SIMBOLOS\n");
    printf("==================================================\n");
    printf("%-15s %-10s %-7s %-7s %-14s\n",
           "Nombre", "Tipo", "Scope", "Linea", "Inicializado");
    printf("--------------------------------------------------\n");
    if (!tabla) {
        printf("(tabla vacia)\n");
    } else {
        for (Simbolo *p = tabla; p; p = p->siguiente)
            printf("%-15s %-10s %-7d %-7d %-14s\n",
                   p->nombre, p->tipo, p->scope, p->linea,
                   p->inicializado ? "si" : "no");
    }
    printf("==================================================\n");
}

void emitir_simbolos_gui(void) {
    for (Simbolo *p = tabla; p; p = p->siguiente)
        printf("%s|%s|%d|%d|%s\n",
               p->nombre, p->tipo, p->scope, p->linea,
               p->inicializado ? "si" : "no");
}

void liberar_tabla(void) {
    Simbolo *p = tabla;
    while (p) {
        Simbolo *n = p->siguiente;
        free(p->nombre);
        free(p->tipo);
        free(p);
        p = n;
    }
    tabla = tabla_tail = NULL;
    tope = 0;
    pila[0] = 0;
    siguiente_id = 0;
}
