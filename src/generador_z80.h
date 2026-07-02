#ifndef GENERADOR_Z80_H
#define GENERADOR_Z80_H

/* ── Generacion de codigo Z80 (Amstrad CPC / WinAPE) ──────────
   Quinta fase del compilador: traduce el codigo intermedio (TAC)
   producido por codigo_intermedio.c a ensamblador Z80 para el
   Amstrad CPC. El programa resultante:

     1. fija el modo de pantalla 1 con el firmware (&BC0E),
     2. ejecuta las sentencias compiladas (asignaciones, cuando,
        loop, imprimir por &BB5A, ...),
     3. pinta un cuadro de bits escribiendo directamente en la
        memoria de video (&C000) y queda en un bucle infinito
        para que el resultado permanezca visible.

   Convenciones:
     - Variables y temporales: palabras de 16 bits con signo.
     - 'dec' se trunca a entero de 16 bits (sin punto flotante).
     - 'text' guarda un puntero a una cadena terminada en 0.
     - org &4000; se carga y ejecuta con RUN"MAIN en el emulador.

   Debe llamarse despues de generar_tac() y antes de liberar_tac(). */

void imprimir_z80 (void);   /* listado con cabeceras (consola)      */
void emitir_z80_gui(void);  /* solo lineas de asm (seccion @@Z80)   */

#endif /* GENERADOR_Z80_H */
