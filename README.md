# Analizador Léxico y Sintáctico en C (Flex & Bison)

Este proyecto corresponde a la implementación de la fase de análisis léxico (Lexer) y análisis sintáctico (Parser) para nuestro lenguaje de programación dummy (especificación del Grupo 5), diseñado utilizando las herramientas **Flex** y **Bison (YACC)** en lenguaje C.

---

## 📂 Estructura del Proyecto

```text
Proyecto LyC/
├── src/
│   ├── lexer.l                 # Especificación de reglas léxicas para Flex
│   ├── parser.y                # Especificación de gramática y main del parser para Bison
│   └── main_lexer.c            # Driver de pruebas exclusivo para el analizador léxico
├── ejemplos/
│   ├── ejemplo1.g5z80          # Código fuente de prueba válido
│   └── ejemplo_con_error.g5z80 # Código fuente de prueba con error de sintaxis intencional
└── README.md                   # Documentación con instrucciones de uso
```

### Descripción de los Archivos Principales:
*   **`src/lexer.l`**: Reglas de expresiones regulares para identificar palabras clave, identificadores, números, operadores y delimitadores.
*   **`src/parser.y`**: Definición de la Gramática Libre de Contexto (GLC). Contiene la lógica del parser y la función `yyerror` para el manejo y reporte de errores sintácticos.
*   **`src/main_lexer.c`**: Programa independiente que consume tokens uno por uno de un archivo fuente y los imprime en consola. Sirve para probar el lexer de manera independiente sin pasar por el análisis sintáctico.

---

## 🛠️ Prerrequisitos

### En Linux (Debian/Ubuntu):
Debes contar con `flex`, `bison` y `gcc`. Puedes instalarlos ejecutando:
```bash
sudo apt-get update
sudo apt-get install flex bison build-essential
```

### En Windows:
*   **Compilación Nativa**: Puedes usar **MinGW-w64** o **MSYS2** para obtener el compilador `gcc` en Windows.
*   **Compilación Cruzada (desde Linux)**: Si estás en Linux y quieres generar los ejecutables `.exe` para Windows, instala el compilador cruzado:
    ```bash
    sudo apt-get install gcc-mingw-w64-x86-64
    ```

---

## 🚀 Fase 1: Generación de código intermedio (Bison & Flex)

Antes de compilar en cualquier plataforma, debes generar los archivos fuente C (`lex.yy.c`, `parser.tab.c` y `parser.tab.h`) a partir de las especificaciones `.l` y `.y`. Ejecuta estos comandos en tu terminal de Linux:

1. **Generar el Parser**:
   ```bash
   bison -d -o src/parser.tab.c src/parser.y
   ```
2. **Generar el Lexer**:
   ```bash
   flex -o src/lex.yy.c src/lexer.l
   ```

---

## 💻 Instrucciones de Compilación

Una vez generados los archivos C en el directorio `src/`, procede con la compilación según tu plataforma:

### 1. Compilación para Linux (GCC)

*   **Compilar solo el Analizador Léxico (Lexer)**:
    ```bash
    gcc -Wall -Isrc src/main_lexer.c src/lex.yy.c -o analizador_lexico
    ```
*   **Compilar el Analizador Sintáctico completo (Parser)**:
    ```bash
    gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c -o analizador_sintactico
    ```

### 2. Compilación para Windows

#### Opción A: Compilación cruzada desde Linux (MinGW)
Si estás en Linux y deseas generar los binarios para Windows:
*   **Compilar el Analizador Léxico**:
    ```bash
    x86_64-w64-mingw32-gcc -Wall -Isrc src/main_lexer.c src/lex.yy.c -o analizador_lexico.exe
    ```
*   **Compilar el Analizador Sintáctico**:
    ```bash
    x86_64-w64-mingw32-gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c -o analizador_sintactico.exe
    ```

#### Opción B: Compilación nativa en Windows (Cmd / PowerShell)
Si ya tienes instalada la herramienta `gcc` en tu PATH de Windows, solo requieres compilar los archivos C previamente generados:
*   **Compilar el Analizador Léxico**:
    ```cmd
    gcc -Wall -Isrc src/main_lexer.c src/lex.yy.c -o analizador_lexico.exe
    ```
*   **Compilar el Analizador Sintáctico**:
    ```cmd
    gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c -o analizador_sintactico.exe
    ```

---

## 🔍 Verificación del Funcionamiento

### 1. Verificar que el Analizador Léxico (Lexer) funciona
El ejecutable del lexer lee un archivo de entrada e imprime en pantalla la lista secuencial de todos los tokens reconocidos junto con su número de línea y lexema.

*   **Comando de ejecución (Linux)**:
    ```bash
    ./analizador_lexico ejemplos/ejemplo1.g5z80
    ```
*   **Comando de ejecución (Windows)**:
    ```cmd
    analizador_lexico.exe ejemplos\ejemplo1.g5z80
    ```

**Salida en consola esperada**:
```text
--- INICIANDO ANALISIS LEXICO DEL ARCHIVO ---

[Linea 1] Token: KW_INICIO (258) | Lexema: "inicio"
[Linea 1] Token: LLAVE_ABRIR (290) | Lexema: "{"
[Linea 2] Token: KW_NUM (264) | Lexema: "num"
[Linea 2] Token: ID (261) | Lexema: "x"
[Linea 2] Token: OP_ASIG (262) | Lexema: "="
...
[Linea 12] Token: LLAVE_CERRAR (291) | Lexema: "}"

--- ANALISIS LEXICO FINALIZADO ---
```

---

### 2. Verificar que el Analizador Sintáctico (Parser) funciona
El parser analiza la sintaxis y verifica si cumple con la GLC del lenguaje.

#### Prueba A: Código Sintácticamente Válido
*   **Comando de ejecución (Linux)**:
    ```bash
    ./analizador_sintactico ejemplos/ejemplo1.g5z80
    ```
*   **Comando de ejecución (Windows)**:
    ```cmd
    analizador_sintactico.exe ejemplos\ejemplo1.g5z80
    ```

**Salida esperada**:
```text
ANALISIS SINTACTICO COMPLETADO EXITOSAMENTE
```

#### Prueba B: Código con Error Sintáctico
*   **Comando de ejecución (Linux)**:
    ```bash
    ./analizador_sintactico ejemplos/ejemplo_con_error.g5z80
    ```
*   **Comando de ejecución (Windows)**:
    ```cmd
    analizador_sintactico.exe ejemplos\ejemplo_con_error.g5z80
    ```

**Salida en stderr esperada**:
```text
[ERROR SINTACTICO] Linea 3: syntax error
```
*(El error es arrojado en la línea 3 debido a la ausencia del punto y coma `;` al final de la declaración de la línea 2).*

---

## 🧹 Limpieza de Archivos Generados
Para eliminar todos los ejecutables e intermedios generados, ejecuta:

*   **En Linux**:
    ```bash
    rm -f src/lex.yy.c src/parser.tab.c src/parser.tab.h analizador_lexico analizador_lexico.exe analizador_sintactico analizador_sintactico.exe
    ```
*   **En Windows**:
    ```cmd
    del src\lex.yy.c src\parser.tab.c src\parser.tab.h analizador_lexico.exe analizador_sintactico.exe
    ```
