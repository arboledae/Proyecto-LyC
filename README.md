# Analizador Léxico, Sintáctico, AST y Semántico en C (Flex & Bison) + Interfaz Gráfica

Este proyecto corresponde a la implementación de las fases de análisis léxico, análisis sintáctico, construcción del Árbol de Sintaxis Abstracta (AST) y **análisis semántico** para el lenguaje de programación Dummy (especificación del Grupo 5), diseñado utilizando **Flex** y **Bison (YACC)** en lenguaje C. Incluye además una **interfaz gráfica web** para visualizar los tres análisis.

---

## 📂 Estructura del Proyecto

```text
Proyecto-LyC/
├── src/
│   ├── lexer.l                 # Reglas léxicas para Flex (trazas conmutables)
│   ├── parser.y                # Gramática, acciones, main y modo --gui del compilador
│   ├── ast.h / ast.c           # AST: nodos, impresión indentada y liberación
│   ├── simbolos.h / simbolos.c # Tabla de símbolos con ámbitos únicos (semántico)
│   ├── semantico.h / semantico.c # Verificación semántica + inferencia de tipos
│   ├── gui_salida.h / gui_salida.c # Emisión delimitada @@TOKENS / @@AST para la GUI
│   ├── tokens.h / tokens.c     # Mapeo código→nombre de token (compartido)
│   └── main_lexer.c            # Driver independiente del analizador léxico
├── gui/                        # Interfaz gráfica web (Python stdlib, sin dependencias)
│   ├── server.py               # Servidor HTTP que invoca `compilador --gui` y traduce a JSON
│   └── static/ (index.html, style.css, app.js)
├── ejemplos/
│   ├── ejemplo1.g5z80          # Programa válido
│   ├── ejemplo_scopes.g5z80    # Ámbitos (scopes) anidados
│   ├── ejemplo_con_error.g5z80 # Error sintáctico intencional
│   └── ejemplo_semantico_error.g5z80 # Errores semánticos
├── Makefile                    # Automatiza bison/flex/gcc y lanza la GUI
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

o simplemente:
```bash
make clean
```

---

## 🧩 Análisis Semántico (Semana 11–12)

Tras construir el AST, el compilador realiza el análisis semántico:

1. **Tabla de símbolos** con manejo de ámbitos (scopes): el bloque `inicio { }` es el scope global (0); los cuerpos de `cuando`/`sino`/`loop` abren un nuevo ámbito.
2. **Inferencia de tipos** por nodo de expresión (`num`, `dec`, `text`, `booleano`).
3. **Verificaciones:** variable no declarada, doble declaración en el mismo ámbito, tipos incompatibles en asignación/declaración, operaciones inválidas entre tipos (`text + num`, `text > num`, `&&` con no-booleanos) y condición de `cuando`/`loop` debe ser booleana.

```bash
./compilador ejemplos/ejemplo_semantico_error.g5z80
```

**Salida (resumen):**
```
[ERROR SEMANTICO] Linea 3: Tipo incompatible: no se puede asignar 'text' a 'num' (variable 'x').
[ERROR SEMANTICO] Linea 4: Variable 'z' no declarada.
[ERROR SEMANTICO] Linea 5: Doble declaracion de la variable 'x' en el mismo ambito.
```

---

## 🖥️ Interfaz Gráfica (entregable final)

Interfaz web para **verificar los tres niveles de análisis** (léxico, sintáctico y semántico) de forma gráfica. **No requiere instalar nada** (usa el servidor HTTP estándar de Python, sin Flask ni dependencias).

### Requisitos
- Tener compilado el proyecto: `make`.
- Python 3 (ya incluido en la mayoría de sistemas).

### Ejecutar
```bash
make gui
```
Luego abre **http://localhost:5000** en el navegador.

### Uso
1. Escribe código o elige uno del desplegable **Cargar ejemplo**.
2. Pulsa **Léxico**, **Sintáctico**, **Semántico** o **⚡ Analizar Todo**.
3. En el panel derecho, navega entre las pestañas:
   - **Léxico:** tabla de tokens con colores por categoría y conteos por tipo.
   - **Sintáctico:** árbol AST dibujado como árbol visual anidado y colapsable.
   - **Semántico:** tabla de símbolos coloreada por ámbito + diagnósticos (léxicos, sintácticos y semánticos) con su línea.
4. La **barra de pipeline** del encabezado (① Léxico → ② Sintáctico → ③ Semántico) resume el estado de cada fase.

> La vista activa y el ejemplo inicial se pueden enlazar: `…/?archivo=ejemplo_scopes.g5z80#semantico`.

### Modo `--gui` del compilador (formato delimitado, sin JSON en C)

El servidor invoca el compilador con `--gui`, que emite en **una sola pasada** un flujo de
texto delimitado por `|` (trivial de parsear en Python, evita escribir un serializador JSON en C):

```text
@@TOKENS
<linea>|<NOMBRE_TOKEN>|<lexema>
@@AST
<profundidad>|<etiqueta>
@@SIMBOLOS
<nombre>|<tipo>|<scope>|<linea>|<si|no>
@@ERRORES
<lexico|sintactico|semantico>|<linea>|<mensaje>
@@ESTADO
sintactico|<ok|error>
semantico|<ok|error|na>
@@END
```

La profundidad de cada nodo del AST se emite de forma explícita, por lo que el frontend
reconstruye el árbol sin ambigüedad. Se puede inspeccionar directamente:

```bash
./compilador --gui ejemplos/ejemplo1.g5z80
```