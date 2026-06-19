# Analizador Léxico, Sintáctico y AST en C (Flex & Bison)

Este proyecto corresponde a la implementación de las fases de análisis léxico, análisis sintáctico y construcción del Árbol de Sintaxis Abstracta (AST) para el lenguaje de programación Dummy (especificación del Grupo 5), diseñado utilizando **Flex** y **Bison (YACC)** en lenguaje C.

---



PARA QUE FUNCIONE EL ANALIZADOR DEBEN ABRIR EL TERMINAL CTRL + Ñ  Y EJECUTAR:XDDDD
gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c src/ast.c src/semantic.c src/symbol_table.c -o compilador.exe


.\compilador.exe ejemplos\ejemplo_error_semantico.g5z80











