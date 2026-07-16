# Documentación: Interfaz Gráfica Verificadora de Análisis (Léxico · Sintáctico · Semántico)

**Curso:** Lenguajes y Compiladores — Grupo 5
**Lenguaje:** Dummy (especificación Grupo 5)
**Docente:** Prudencio Vidal, Javier Antonio
**Periodo:** Semanas 11–13 (Fase 3 — Semántico + entregable GUI)
**Proyecto:** `Proyecto-LyC/`

---

## 1. Objetivo

Implementar y documentar la **interfaz gráfica** que permite **verificar visualmente los
distintos niveles de análisis** del compilador del lenguaje Dummy:

1. **Análisis léxico** — tokens reconocidos.
2. **Análisis sintáctico** — Árbol de Sintaxis Abstracta (AST).
3. **Análisis semántico** — tabla de símbolos con ámbitos y diagnósticos de tipos.

Esta interfaz es el **entregable solicitado por el profesor**. Además de los tres niveles de
análisis, en la **Fase 4 (Semana 13)** el compilador genera **código intermedio de 3
direcciones** y su traducción a **ensamblador Z80**, que la GUI muestra en una cuarta vista
"Código".

> Este documento parte de lo planificado en `Sesión 7/Plan_Analisis_Semantico_Semana11-13.md`,
> del informe del lenguaje (`Sesión 2`), del cronograma (`Sesión 3`) y de los avances de las
> Semanas 9–10 (`Sesión 6`).

---

## 2. Resumen del lenguaje Dummy (contexto)

| Elemento | Detalle |
|---|---|
| Inicio de programa | `inicio { ... }` (ámbito global) |
| Tipos declarables | `num` (entero), `dec` (decimal), `text` (cadena) |
| Booleano | `verdadero` / `falso` (en condiciones, no declarable) |
| Estructuras | `cuando (cond) { } sino { }`, `loop (cond) { }`, `imprimir(expr)`, `retornar expr` |
| Bloques `{ }` | Abren ámbitos (scopes) anidados |
| Comentarios | `%% ... %%` (ignorados) |
| Operadores | aritméticos `+ - * / %`, relacionales `> < >= <= == !=`, lógicos `&& || !`, asignación `=` |

---

## 3. Arquitectura de la solución

```
┌──────────────────────┐   POST /analizar (JSON: {codigo})   ┌────────────────────────┐
│  Navegador (frontend) │ ──────────────────────────────────▶ │  server.py (Python std) │
│  index.html/app.js/   │ ◀────────────── JSON ─────────────── │  - escribe tmp .g5z80   │
│  style.css            │   {tokens, ast, simbolos, errores,   │  - ejecuta compilador   │
└──────────────────────┘    estados}                          │    --gui                │
                                                               │  - parsea flujo @@...   │
                                                               └───────────┬────────────┘
                                                                           │ stdout delimitado
                                                                           ▼
                                                            ┌────────────────────────────┐
                                                            │  compilador --gui (C)       │
                                                            │  Flex + parser RD + AST +   │
                                                            │  tabla de símbolos +        │
                                                            │  verificación semántica     │
                                                            └────────────────────────────┘
```

- **Compilador en C** (Flex + descenso recursivo): intacto en su lógica de análisis; se le añadió un
  **modo `--gui`** que emite un flujo de texto delimitado, fácil de parsear.
- **Backend `gui/server.py`**: servidor HTTP de la **biblioteca estándar de Python**
  (`http.server`), **sin Flask ni dependencias**. Invoca `compilador --gui`, traduce su salida
  a JSON y sirve los archivos estáticos.
- **Frontend** (`gui/static/`): HTML + CSS + JavaScript *vanilla* (sin CDN, funciona offline).

### 3.1 Decisión de diseño: formato delimitado en vez de JSON en C

Generar JSON desde C es propenso a errores (escape de comillas, comas). En su lugar, el
compilador emite secciones marcadas con `@@` y campos separados por `|`, que Python convierte
a JSON. Así el código C se mantiene simple y robusto, tal como se anticipó en el plan (§5.1).

---

## 4. El modo `--gui` del compilador

Invocación:

```bash
./compilador --gui ejemplos/ejemplo1.g5z80
```

Emite **en una sola pasada** las siguientes secciones:

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

Características clave:

- **`@@TOKENS`** se genera con una *pre-pasada* léxica sobre el archivo; luego el lexer se
  rebobina (`yyrestart`) para que el parser vuelva a recorrerlo. Es la misma cadena de tokens
  que consume el análisis sintáctico, garantizando consistencia entre vistas.
- **`@@AST`** emite la **profundidad explícita** de cada nodo (no por indentación), de modo que
  el frontend reconstruye el árbol sin ambigüedad.
- **`@@SIMBOLOS`** lista **todos** los símbolos de **todos** los ámbitos (ver §5.2).
- **`@@ESTADO`** entrega el veredicto por fase: `semantico=na` cuando hubo error sintáctico
  (no se llega a analizar la semántica).

Ejemplo real (fragmento para `ejemplo1.g5z80`):

```text
@@TOKENS
1|KW_INICIO|inicio
2|KW_NUM|num
2|IDENTIFICADOR|x
...
@@AST
0|PROGRAMA
1|DECLARACION num x
2|VALOR_INICIAL
3|LIT_ENTERO 10
...
@@SIMBOLOS
x|num|0|2|si
promedio|dec|0|3|si
@@ERRORES
@@ESTADO
sintactico|ok
semantico|ok
@@END
```

---

## 5. Cambios realizados sobre el proyecto

La parte gráfica ya estaba **avanzada** (servidor + 3 pestañas). El trabajo de esta entrega
consistió en **corregir defectos de la base que impedían una verificación fiable** y en
**mejorar la interfaz**. Resumen:

### 5.1 Corrección del AST (defecto crítico)

La impresión previa del AST acumulaba sangrías en los nodos lista, produciendo profundidades
**no monótonas** (un nodo hijo podía aparecer *menos* indentado que su padre). En consecuencia,
el árbol que dibujaba la GUI quedaba **corrupto**. Se corrigió `imprimir_ast` (consola) y se
añadió un emisor con profundidad explícita (`gui_salida.c`) para la GUI. Ahora la profundidad
crece de forma estrictamente monótona.

### 5.2 Tabla de símbolos con ámbitos únicos

La tabla anterior **eliminaba** los símbolos al salir de un bloque, por lo que la GUI solo
mostraba el ámbito global. Se rediseñó `simbolos.c` con el enfoque estándar:

- Cada bloque recibe un **identificador de ámbito único y creciente** (global `0`, luego `1, 2, 3…`).
- La **visibilidad** se resuelve con una **pila de ámbitos activos** (`buscar` recorre del
  ámbito más interno al más externo).
- Los símbolos **no se liberan** al cerrar un bloque: se conservan para mostrar la tabla
  completa. La corrección del análisis se mantiene porque `buscar` solo considera los ámbitos
  activos.

Esto permite, por ejemplo, que dos variables `y` en bloques hermanos (`cuando` y `sino`)
**coexistan** en ámbitos distintos **sin** generar un falso error de doble declaración.

### 5.3 Limpieza del lexer

El lexer imprimía una traza por cada token de forma incondicional, lo que **duplicaba** la
salida del analizador léxico independiente y contaminaba el flujo. Se enrutaron las trazas por
una bandera `lexer_verbose` (apagada en `--gui` y en el analizador independiente). Esto cumple
lo pedido por el cronograma (Semanas 4 y 8: *"retorno de tokens, no printf"*).

### 5.4 Módulo compartido de nombres de token

Se extrajo `nombre_token()` a `tokens.c/.h`, reutilizado por el analizador léxico y por la
emisión `--gui` (antes estaba duplicado).

### 5.5 Backend (`server.py`)

- Invoca **un solo binario** (`compilador --gui`) en lugar de dos procesos.
- Parser robusto del flujo `@@...` (usa `split('|', maxsplit)` para tolerar `|` dentro de
  cadenas de texto).
- Sirve la lista de ejemplos (`/ejemplos`) y su contenido (`/ejemplo?archivo=…`).

### 5.6 Frontend (`index.html`, `app.js`, `style.css`)

- **Barra de pipeline** ① Léxico → ② Sintáctico → ③ Semántico con estado por fase.
- **Editor con numeración de líneas**.
- **Léxico:** tabla con índice, badges por categoría y **leyenda con conteos**.
- **Sintáctico:** árbol **colapsable**, con nodos coloreados por tipo (programa, control,
  declaración, operador, literal, identificador).
- **Semántico:** tabla de símbolos con **ámbitos y tipos coloreados**; **diagnósticos**
  unificados (léxico/sintáctico/semántico) con etiqueta de tipo y línea.
- **Desplegable de ejemplos** y **deep-linking** (`?archivo=…#vista`).

---

## 6. Cómo compilar y ejecutar

```bash
# 1. Compilar el compilador (y el analizador léxico)
make

# 2. Levantar la interfaz gráfica
make gui

# 3. Abrir en el navegador
#    http://localhost:5000
```

Requisitos: `flex`, `gcc`, `make` y `python3` (sin Bison ni librerías adicionales).

Inspección por consola del modo gráfico (útil para depurar):

```bash
./compilador --gui ejemplos/ejemplo_semantico_error.g5z80
```

---

## 7. Casos de prueba (ejemplos incluidos)

| Ejemplo | Qué demuestra | Resultado esperado |
|---|---|---|
| `ejemplo1.g5z80` | Programa válido | 48 tokens · AST completo · 2 símbolos · sin errores |
| `ejemplo_scopes.g5z80` | Ámbitos anidados | 4 símbolos en scopes 0/1/2/3 · sin errores |
| `ejemplo_semantico_error.g5z80` | Errores semánticos | 4 diagnósticos (tipos, no declarada, doble decl.) |
| `ejemplo_con_error.g5z80` | Error sintáctico | AST vacío · 1 error sintáctico · semántico `na` |

Detalle de los errores detectados en `ejemplo_semantico_error.g5z80`:

```text
[Semántico] Línea 3: Tipo incompatible: no se puede asignar 'text' a 'num' (variable 'x').
[Semántico] Línea 4: Variable 'z' no declarada.
[Semántico] Línea 5: Doble declaracion de la variable 'x' en el mismo ambito.
[Semántico] Línea 6: Comparacion '>' invalida entre tipos 'text' y 'num'.
```

---

## 8. Capturas de la interfaz

### 8.1 Vista Léxico — tabla de tokens con categorías y conteos
![Vista Léxico](img/01_vista_lexico.png)

### 8.2 Vista Sintáctico — AST como árbol visual anidado (ejemplo de ámbitos)
![Vista Sintáctico](img/02_vista_sintactico_ast.png)

### 8.3 Vista Semántico — diagnósticos de un programa con errores
![Vista Semántico con errores](img/03_vista_semantico_errores.png)

### 8.4 Vista Semántico — tabla de símbolos con ámbitos anidados
![Tabla de símbolos por ámbito](img/04_vista_semantico_scopes.png)

---

## 9. Mapa de la verificación por nivel

| Nivel | Fuente en el compilador | Sección `@@` | Vista en la GUI |
|---|---|---|---|
| **Léxico** | `lexer.l` (Flex) | `@@TOKENS` | Tabla de tokens + leyenda |
| **Sintáctico** | `parser_rd.c` (descenso recursivo LL(1)) + `ast.c` | `@@AST` + `@@ESTADO` | Árbol AST colapsable |
| **Semántico** | `simbolos.c` + `semantico.c` | `@@SIMBOLOS` + `@@ERRORES` + `@@ESTADO` | Tabla de símbolos + diagnósticos |

---

## 10. Fase 4 — Generación de código (Semana 13, implementada)

Tras un análisis semántico correcto, el compilador genera:

1. **Código intermedio de 3 direcciones** a partir del AST (temporales, etiquetas y saltos).
2. **Ensamblador Z80**: asignaciones, aritmética, condicionales (saltos) y bucles
   (`loop` → etiquetas + jumps).
3. **Mapeo de variables y temporales a memoria** (sección de datos `_nombre: DW 0`).

La GUI lo muestra en una **cuarta vista "Código"** (secciones `@@CODIGO_INTERMEDIO` y
`@@CODIGO_Z80` del modo `--gui`). El detalle completo, con esquemas de traducción y diagramas,
está en `Documentacion_Tecnica_Compilador_S6-S12.md` §9.

---

## 11. Estructura final de archivos relevantes

```
Proyecto-LyC/
├── src/
│   ├── lexer.l            # traza conmutable (lexer_verbose)
│   ├── parser_rd.c        # descenso recursivo LL(1) + captura de error sintáctico
│   ├── main.c             # driver: consola y modo --gui
│   ├── ast.c              # imprimir_ast con profundidad monótona
│   ├── simbolos.c         # ámbitos únicos + pila de visibilidad
│   ├── semantico.c        # + emitir_errores_gui()
│   ├── gui_salida.c       # NUEVO: @@TOKENS y @@AST
│   └── tokens.c           # NUEVO: nombre_token() compartido
├── gui/
│   ├── server.py          # invoca compilador --gui y traduce a JSON
│   └── static/            # index.html, app.js, style.css
└── ejemplos/
    ├── ejemplo1.g5z80
    ├── ejemplo_scopes.g5z80          # NUEVO
    ├── ejemplo_semantico_error.g5z80
    └── ejemplo_con_error.g5z80
```

---

*Documento elaborado para la Fase 3 (Semanas 11–13). La interfaz gráfica verifica de forma
visual los niveles léxico, sintáctico y semántico del compilador del lenguaje Dummy del Grupo 5.*
