#ifndef SIMBOLOS_H
#define SIMBOLOS_H

/* ── Tabla de simbolos con manejo de ambitos (scopes) ─────────
   Semana 11 - Analisis semantico del lenguaje Dummy.

   Cada bloque abre un AMBITO con un identificador unico y creciente:
   el bloque "inicio { }" es el ambito global (0) y los cuerpos de
   cuando / sino / loop reciben ids 1, 2, 3 ... en orden de apertura.
   La visibilidad se resuelve con una pila de ambitos activos: una
   variable es visible si pertenece a alguno de los ambitos abiertos
   en ese momento (del mas interno al mas externo).

   A diferencia de un interprete, NO se liberan los simbolos al salir
   de un bloque: se conservan todos hasta el final para poder mostrar
   la tabla completa (todos los ambitos) en la interfaz grafica. La
   correccion del analisis se mantiene porque buscar() solo considera
   los ambitos que siguen activos en la pila. */

typedef struct Simbolo {
    char *nombre;          /* nombre de la variable               */
    char *tipo;            /* "num", "dec", "text", "booleano"    */
    int   scope;           /* identificador unico del ambito      */
    int   linea;           /* linea de declaracion                */
    int   inicializado;    /* 1 si recibio un valor, 0 si no      */
    struct Simbolo *siguiente;
} Simbolo;

void      tabla_init         (void);
int       scope_actual       (void);   /* id del ambito activo mas interno      */
int       entrar_scope       (void);   /* abre un ambito nuevo y devuelve su id */
int       salir_scope        (void);   /* cierra el ambito activo               */
int       declarar           (const char *nombre, const char *tipo,
                              int linea, int inicializado);  /* 0 ok, -1 doble decl. */
Simbolo*  buscar             (const char *nombre);  /* visibilidad (ambitos activos)   */
Simbolo*  buscar_local       (const char *nombre);  /* solo el ambito activo interno   */
void      marcar_inicializado(const char *nombre);
void      imprimir_tabla     (void);   /* volcado legible para consola          */
void      emitir_simbolos_gui(void);   /* "nombre|tipo|scope|linea|si/no" (modo --gui) */
void      liberar_tabla      (void);

#endif /* SIMBOLOS_H */
