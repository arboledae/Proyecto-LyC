# Analizador Sintáctico y Léxico en C (Flex & Bison)

Este proyecto corresponde a la implementación de la fase de análisis léxico y sintáctico (Parser) para nuestro lenguaje dummy, estructurado y adaptado según las especificaciones de la gramática libre de contexto (GLC) definida en el informe de laboratorio.

---

## 📂 Estructura del Proyecto

El proyecto está organizado de la siguiente manera:

```text
Proyecto LyC/
├── src/
│   ├── lexer.l                 # Especificación de tokens para Flex
│   └── parser.y                # Especificación de gramática y punto de entrada para Bison
├── ejemplos/
│   ├── ejemplo1.g5z80          # Código de prueba sintácticamente válido
│   └── ejemplo_con_error.g5z80 # Código de prueba con error de sintaxis intencional
└── README.md                   # Este archivo explicativo con instrucciones
```

### Descripción de los Archivos:

*   **`src/lexer.l`**: Archivo de especificación léxica para Flex. Contiene las definiciones regulares y reglas de traducción para el reconocimiento de tokens de nuestro lenguaje (palabras clave como `inicio`, `sino`, variables, números, cadenas literales, operadores aritméticos, lógicos y relacionales). Reporta la línea y el carácter en caso de errores léxicos.
*   **`src/parser.y`**: Archivo de especificación sintáctica para Bison (YACC). Define la gramática libre de contexto (GLC), resuelve la precedencia y asociatividad de los operadores para evitar ambigüedades, y contiene la lógica principal en `main()`. Reporta cualquier error sintáctico con precisión mediante `yyerror` indicando la línea correspondiente.
*   **`ejemplos/ejemplo1.g5z80`**: Código fuente de prueba con sintaxis correcta que cubre variables, condicionales (`cuando` / `sino`), ciclos (`loop`) y llamadas de salida (`imprimir`).
*   **`ejemplos/ejemplo_con_error.g5z80`**: Código de prueba intencionalmente erróneo (sin un punto y coma `;` final en la declaración de variable) que demuestra la capacidad del parser para detectar y señalar errores sintácticos.

---

## 🛠️ Prerrequisitos

Para compilar y ejecutar este proyecto, necesitas tener instaladas las siguientes herramientas en tu sistema operativo Linux:

- **Flex** (Generador de analizadores léxicos)
- **Bison** (Generador de analizadores sintácticos)
- **GCC** (Compilador de C)

Si necesitas instalarlas en Debian/Ubuntu, puedes ejecutar:
```bash
sudo apt-get update
sudo apt-get install flex bison build-essential
```

---

## 🚀 Instrucciones de Compilación y Ejecución Manual

Sigue estos pasos en tu terminal para compilar el analizador manualmente desde la raíz de la carpeta `Proyecto LyC`:

### Paso 1: Generar los analizadores con Flex y Bison
Primero, compilamos las especificaciones para generar el código fuente C de los analizadores:

1. **Generar el parser (Bison)**:
   Este comando creará los archivos de soporte `src/parser.tab.c` y `src/parser.tab.h` (que contiene las declaraciones de tokens que usará el lexer).
   ```bash
   bison -d -o src/parser.tab.c src/parser.y
   ```

2. **Generar el lexer (Flex)**:
   Este comando generará el archivo `src/lex.yy.c` que implementa la función del escáner léxico.
   ```bash
   flex -o src/lex.yy.c src/lexer.l
   ```

### Paso 2: Compilar el Ejecutable Final con GCC
Una vez generados los archivos C, compílalos y enlázalos en un ejecutable único llamado `compilador`:

```bash
gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c -o compilador
```

### Paso 3: Ejecutar y Probar

*   **Verificar que el parser acepte un código válido**:
    ```bash
    ./compilador ejemplos/ejemplo1.g5z80
    ```
    **Salida esperada:**
    ```text
    ANALISIS SINTACTICO COMPLETADO EXITOSAMENTE
    ```

*   **Verificar que el parser reporte errores de sintaxis correctamente**:
    ```bash
    ./compilador ejemplos/ejemplo_con_error.g5z80
    ```
    **Salida esperada (en stderr):**
    ```text
    [ERROR SINTACTICO] Linea 3: syntax error
    ```

---

## 🧹 Limpieza

Para remover los archivos intermedios generados por Flex/Bison y el binario ejecutable compilado, ejecuta el siguiente comando:

```bash
rm -f src/lex.yy.c src/parser.tab.c src/parser.tab.h compilador
```
