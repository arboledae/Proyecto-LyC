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
- **`src/main_lexer.c`**: Programa independiente que consume tokens uno por uno e imprime cada token con su número de línea y lexema. Sirve para probar el lexer sin pasar por el análisis sintáctico.

---

## 🛠️ Prerrequisitos

### En Linux (Debian/Ubuntu):
```bash
sudo apt-get update
sudo apt-get install flex bison build-essential
```

### En Windows:
- **Compilación Nativa**: Usa **MinGW-w64** o **MSYS2** para obtener `gcc` en Windows.
- **Compilación Cruzada (desde Linux)**: Instala el compilador cruzado:
    ```bash
    sudo apt-get install gcc-mingw-w64-x86-64
    ```

---

## 🚀 Fase 1: Generación de archivos intermedios (Bison & Flex)

Ejecuta estos comandos desde la raíz del proyecto:

```bash
bison -d src/parser.y -o src/parser.tab.c
flex -o src/lex.yy.c src/lexer.l
```

Esto genera `src/parser.tab.c`, `src/parser.tab.h` y `src/lex.yy.c`.

---

## 💻 Instrucciones de Compilación

### 1. Compilar el Compilador Completo (Léxico + Sintáctico + AST)

**Linux:**
```bash
gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c src/ast.c -o compilador
```

**Windows (nativo):**
```cmd
gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c src/ast.c -o compilador.exe
```

**Windows (compilación cruzada desde Linux):**
```bash
x86_64-w64-mingw32-gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c src/ast.c -o compilador.exe
```

### 2. Compilar solo el Analizador Léxico

**Linux:**
```bash
gcc -Wall -Isrc src/main_lexer.c src/lex.yy.c -o analizador_lexico
```

**Windows:**
```cmd
gcc -Wall -Isrc src/main_lexer.c src/lex.yy.c -o analizador_lexico.exe
```

---

## 🔍 Verificación del Funcionamiento

### 1. Analizador Léxico independiente

```bash
./analizador_lexico ejemplos/ejemplo1.g5z80
```

**Salida esperada:**
```text
--- INICIANDO ANALISIS LEXICO DEL ARCHIVO ---

[Linea 1] Token: KW_INICIO (258) | Lexema: "inicio"
[Linea 1] Token: DELIM_LLAVE_IZQ (292) | Lexema: "{"
[Linea 2] Token: KW_NUM (264) | Lexema: "num"
[Linea 2] Token: IDENTIFICADOR (261) | Lexema: "x"
[Linea 2] Token: OP_ASIGNACION (270) | Lexema: "="
[Linea 2] Token: LIT_ENTERO (267) | Lexema: "10"
...

--- ANALISIS LEXICO FINALIZADO ---
```

### 2. Compilador completo (Léxico + Sintáctico + AST)

#### Prueba A: Código válido

```bash
./compilador ejemplos/ejemplo1.g5z80
```

**Salida esperada:**
```text
==================================================
Iniciando analisis de: ejemplos/ejemplo1.g5z80
==================================================

[Lexico] Linea 1: Palabra clave 'inicio'
...

==================================================
ARBOL DE SINTAXIS ABSTRACTA (AST)
==================================================

[PROGRAMA]
    [DECLARACION] tipo=num  var=x
    [VALOR INICIAL]
      [LIT_ENTERO] 10
    [CUANDO]
    [CONDICION]
      [OP_BIN] >
        [ID] x
        [LIT_ENTERO] 5
    [BLOQUE_SI]
        [IMPRIMIR]
          [LIT_TEXT] "Mayor que 5"
    ...

==================================================
FIN DEL AST
==================================================

==================================================
ANALISIS SINTACTICO Y AST COMPLETADOS EXITOSAMENTE
==================================================
```

#### Prueba B: Código con error sintáctico

```bash
./compilador ejemplos/ejemplo_con_error.g5z80
```

**Salida esperada:**
```text
[ERROR SINTACTICO] Linea 3: syntax error

==================================================
ANALISIS CONCLUIDO CON ERRORES SINTACTICOS
==================================================
```

---

## 🧹 Limpieza de Archivos Generados

**Linux:**
```bash
rm -f src/lex.yy.c src/parser.tab.c src/parser.tab.h compilador analizador_lexico
```

**Windows:**
```cmd
del src\lex.yy.c src\parser.tab.c src\parser.tab.h compilador.exe analizador_lexico.exe
```