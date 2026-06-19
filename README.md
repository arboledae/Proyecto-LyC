# Analizador Léxico, Sintáctico y AST en C (Flex & Bison)

Este proyecto corresponde a la implementación de las fases de análisis léxico, análisis sintáctico y construcción del Árbol de Sintaxis Abstracta (AST) para el lenguaje de programación Dummy (especificación del Grupo 5), diseñado utilizando **Flex** y **Bison (YACC)** en lenguaje C.

---

## 📂 Estructura del Proyecto

```text
Proyecto-LyC/
├── src/
│   ├── lexer.l                 # Reglas léxicas para Flex
│   ├── parser.y                # Gramática, acciones semánticas y main del compilador
│   ├── ast.h                   # Definición de tipos y declaraciones del AST
│   ├── ast.c                   # Implementación de nodos, impresión y liberación del AST
│   ├── symbol_table.h          # Declaraciones de la tabla de símbolos
│   ├── symbol_table.c          # Registro y búsqueda de símbolos por scope
│   ├── semantic.h              # Declaración del analizador semántico
│   ├── semantic.c              # Recorrido semántico del AST
│   └── main_lexer.c            # Driver independiente para probar solo el analizador léxico
├── ejemplos/
│   ├── ejemplo1.g5z80          # Código fuente de prueba válido
│   └── ejemplo_con_error.g5z80 # Código fuente de prueba con error sintáctico intencional
└── README.md
```

### Descripción de los Archivos Principales:
- **`src/lexer.l`**: Expresiones regulares para identificar tokens del lenguaje Dummy.
- **`src/parser.y`**: Gramática Libre de Contexto (GLC) del lenguaje. Contiene las acciones semánticas que construyen el AST y la función `yyerror` para el reporte de errores sintácticos.
- **`src/ast.h`**: Declaración del enum `TipoNodo`, la estructura `NodoAST` y los prototipos de todos los constructores.
- **`src/ast.c`**: Implementación de los constructores de nodos, la función `imprimir_ast` (impresión con indentación) y `liberar_ast` (liberación de memoria).
- **`src/symbol_table.h` / `src/symbol_table.c`**: Definición e implementación de la tabla de símbolos. Registra variables, tipos y nivel de scope.
- **`src/semantic.h` / `src/semantic.c`**: Analizador semántico que recorre el AST, registra declaraciones y detecta variables no declaradas o redeclaradas en el mismo scope.
- **`src/main_lexer.c`**: Programa independiente que consume tokens uno por uno e imprime cada token con su número de línea y lexema. Sirve para probar el lexer sin pasar por el análisis sintáctico.

---


PARA QUE FUNCIONE EL ANALIZADOR DEBEN ABRIR EL TERMINAL CTRL + Ñ  Y EJECUTAR:XDDDD
gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c src/ast.c src/semantic.c src/symbol_table.c -o compilador.exe


.\compilador.exe ejemplos\ejemplo_error_semantico.g5z80











