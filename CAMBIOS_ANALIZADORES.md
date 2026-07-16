# Cambios en los analizadores para soportar videojuegos (SPACEWAR)

**Grupo 5 · Lenguajes y Compiladores · UNMSM**
Documento que registra, **fase por fase**, qué se añadió al compilador del lenguaje
**Dummy** respecto a lo descrito en los informes previos, con el fin de poder escribir
y ejecutar un **videojuego jugable** (`ejemplos/spacewar.g5z80`) en el Amstrad CPC
(WinAPE).

> Informes de referencia: *Informe Lenguaje de Programación S2*, *Plan de Análisis
> Semántico (Semana 11‑13)*, *Documentación de la Interfaz Gráfica (Semana 11‑13)* e
> [`IMPLEMENTACION.md`](IMPLEMENTACION.md).

---

## 0. Motivo del cambio

Según los informes, el lenguaje Dummy solo tenía **una** forma de comunicarse con el
usuario: `imprimir(...)`, que además **siempre añade un salto de línea**. Por eso el
juego previo (`juego_adivina.g5z80`) **se auto‑juega**: no existía forma de leer el
teclado ni de controlar la pantalla.

Para poder **jugar de verdad** se incorporaron **7 primitivas** nuevas al lenguaje.
Todas atraviesan las 5 fases del compilador:

| Primitiva | Categoría | Significado |
|-----------|-----------|-------------|
| `limpiar()` | sentencia | Borra la pantalla |
| `posicionar(x, y)` | sentencia | Mueve el cursor a (columna, fila) |
| `dibujar(expr)` | sentencia | Imprime **sin** salto de línea |
| `pintar(codigo)` | sentencia | Dibuja un carácter por su código |
| `esperar(n)` | sentencia | Espera `n` cuadros de vídeo (ritmo) |
| `tecla()` | expresión | Lee el teclado **sin bloquear** (código de tecla, 0 si ninguna) |
| `aleatorio(n)` | expresión | Número pseudoaleatorio `0..n-1` |

También se añadió el delimitador **coma `,`** (necesario para `posicionar(x, y)`), que
no existía en el lenguaje.

---

## 1. Analizador Léxico (`src/lexer.l`, `src/tokens.c`)

**Informe previo:** el conjunto de tokens cubría palabras clave (`inicio`, `cuando`,
`sino`, `loop`, `imprimir`, `retornar`, `num`, `dec`, `text`), booleanos, operadores
aritméticos/relacionales/lógicos, delimitadores `; ( ) { }`, literales e identificadores.
**No había** tokens de entrada, pantalla, tiempo ni azar, **ni la coma**.

**Cambios:**

| Nuevo token | Lexema | Patrón (Flex) |
|-------------|--------|----------------|
| `KW_LIMPIAR` | `limpiar` | literal |
| `KW_POSICIONAR` | `posicionar` | literal |
| `KW_DIBUJAR` | `dibujar` | literal |
| `KW_PINTAR` | `pintar` | literal |
| `KW_ESPERAR` | `esperar` | literal |
| `KW_TECLA` | `tecla` | literal |
| `KW_ALEATORIO` | `aleatorio` | literal |
| `DELIM_COMA` | `,` | literal |

- Se agregaron 8 reglas en `lexer.l` (7 palabras clave + la coma), cada una con su
  traza `LEXLOG` para mantener el estilo del resto del lexer.
- Se añadió el mapeo código→nombre de cada token nuevo en `tokens.c`
  (`nombre_token`), de modo que la sección `@@TOKENS` de la GUI y el analizador léxico
  independiente muestren los nombres correctos.

> No hubo cambios en las expresiones regulares de literales/identificadores/comentarios:
> las palabras clave nuevas se resuelven **antes** que la regla de `IDENTIFICADOR`,
> igual que las palabras clave existentes.

---

## 2. Analizador Sintáctico (`src/parser.y`)

**Informe previo:** la gramática (GLC en Bison) generaba, como sentencias:
`declaracion`, `asignacion`, `condicional` (`cuando`/`sino`), `ciclo` (`loop`),
`impresion` (`imprimir`) y `retorno`. Las expresiones eran literales, identificadores y
operadores. **Una expresión no podía "llamar" a nada que devolviera un valor.**

**Cambios:**

1. **Declaración de tokens** nuevos (`%token`) y de sus no‑terminales (`%type <nodo>`).
2. **Nuevas sentencias** añadidas a la regla `sentencia`:

   ```
   sentencia:
       ... (las anteriores)
     | limpiar     DELIM_PUNTO_COMA
     | posicionar  DELIM_PUNTO_COMA
     | dibujar     DELIM_PUNTO_COMA
     | pintar      DELIM_PUNTO_COMA
     | esperar     DELIM_PUNTO_COMA
   ```

   con sus producciones:

   ```
   limpiar    : KW_LIMPIAR    '(' ')'
   posicionar : KW_POSICIONAR '(' expresion ',' expresion ')'
   dibujar    : KW_DIBUJAR    '(' expresion ')'
   pintar     : KW_PINTAR     '(' expresion ')'
   esperar    : KW_ESPERAR    '(' expresion ')'
   ```

3. **Expresiones que devuelven valor** (novedad conceptual): se amplió la regla
   `expresion` con dos "funciones intrínsecas":

   ```
   expresion:
       ... (las anteriores)
     | KW_TECLA '(' ')'                    -> nodo_leer_tecla()
     | KW_ALEATORIO '(' expresion ')'      -> nodo_aleatorio(expr)
   ```

   Esto permite usarlas dentro de asignaciones y condiciones, p. ej.
   `k = tecla();` o `ax = aleatorio(38) + 2;`.

> El único conflicto **reduce/reduce** que ya documentaba el informe (el booleano entre
> paréntesis) se mantiene igual; las reglas nuevas no introducen conflictos.

### AST (`src/ast.h`, `src/ast.c`, `src/gui_salida.c`)

Se añadieron **7 tipos de nodo** (`NODO_LIMPIAR`, `NODO_POSICIONAR`, `NODO_DIBUJAR`,
`NODO_PINTAR`, `NODO_ESPERAR`, `NODO_LEER_TECLA`, `NODO_ALEATORIO`), sus **constructores**
y su **impresión** tanto en consola (`imprimir_ast`) como en la vista gráfica del árbol
(`emit_nodo` en `gui_salida.c`). `posicionar` se muestra con sus dos hijos etiquetados
`[COLUMNA]` y `[FILA]`.

---

## 3. Analizador Semántico (`src/semantico.c`)

**Informe previo (Plan de Análisis Semántico):** tabla de símbolos con ámbitos,
inferencia de tipos por nodo y verificaciones de: variable no declarada, doble
declaración, tipos incompatibles, operaciones inválidas entre tipos y condición de
`cuando`/`loop` obligatoriamente booleana.

**Cambios (nuevas reglas de tipos):**

| Construcción | Regla semántica añadida | Tipo resultante |
|--------------|--------------------------|-----------------|
| `tecla()` | — (sin argumentos) | `num` |
| `aleatorio(n)` | `n` debe ser numérico (`num`/`dec`) | `num` |
| `posicionar(x, y)` | `x` e `y` deben ser numéricos | — |
| `pintar(codigo)` | `codigo` debe ser numérico | — |
| `esperar(n)` | `n` debe ser numérico | — |
| `dibujar(expr)` | acepta cualquier valor imprimible (igual que `imprimir`) | — |

- La inferencia de tipos (`inferir_tipo`) reconoce `tecla()`/`aleatorio(n)` como
  expresiones de tipo `num`, por lo que participan correctamente en comprobaciones de
  compatibilidad (p. ej. `num k = tecla();` es válido; `text k = tecla();` daría error).
- El recorrido de sentencias (`visitar`) valida los argumentos de las 5 nuevas
  sentencias y **reutiliza el mismo mecanismo de errores** (`agregar_error`), así los
  mensajes aparecen en consola y en la sección `@@ERRORES` de la GUI con el formato
  existente.

No se modificó la tabla de símbolos ni el manejo de ámbitos: las primitivas nuevas no
introducen nuevos alcances.

---

## 4. Código Intermedio / TAC (`src/codigo_intermedio.c`)

**Informe previo:** cuádruplas de tres direcciones con temporales `t1..tn` y etiquetas
`L1..Ln`; operaciones aritméticas, relacionales, lógicas, `=`, `label`, `goto`,
`ifFalse`, `imprimir`, `retornar`, `fin`.

**Cambios (nuevas operaciones de cuádrupla):**

| Operación | Forma de la cuádrupla | Legible |
|-----------|------------------------|---------|
| `cls` | `(cls, -, -, -)` | `limpiar` |
| `locate` | `(locate, x, y, -)` | `posicionar x, y` |
| `draw` | `(draw, valor, tipo, -)` | `dibujar valor` |
| `putc` | `(putc, codigo, -, -)` | `pintar codigo` |
| `wait` | `(wait, n, -, -)` | `esperar n` |
| `tecla` | `(tecla, -, -, tN)` | `tN = tecla()` |
| `rand` | `(rand, n, -, tN)` | `tN = aleatorio(n)` |

- `tecla` y `rand` generan un **temporal** (como cualquier expresión) para que su
  resultado se pueda encadenar en expresiones mayores.
- `draw` guarda el **tipo** del valor en `arg2` (igual que `imprimir`) para que la fase
  Z80 sepa si imprimir una cadena o un número.
- Se añadieron sus formas legibles a `tac_formatear`, de modo que la tabla de cuádruplas
  (consola y GUI) las muestra con sintaxis clara.

---

## 5. Generación de Código Z80 (`src/generador_z80.c`)

**Informe previo:** traducción de cada cuádrupla a ensamblador Z80 del Amstrad CPC;
variables/temporales como palabras de 16 bits; `imprimir` vía firmware `&BB5A`
(TXT_OUTPUT) **con salto de línea**; rutinas de soporte (`__mul16`, `__div16`,
`__mod16`, comparaciones, etc.); `org &4000`.

**Cambios:**

### 5.1 Uso nuevo del firmware del CPC

| Primitiva | Firmware / técnica |
|-----------|--------------------|
| `limpiar()` | `CHR$(12)` → `ld a,12 : call &BB5A` |
| `posicionar(x,y)` | control 31 → `ld a,31 : call &BB5A` seguido de columna y fila |
| `dibujar(...)` | como `imprimir` pero **sin** llamar a `__prnl` (sin salto de línea) |
| `pintar(cod)` | `ld a,l : call &BB5A` (carácter directo) |
| `esperar(n)` | rutina `__esperar` con `&BD19` **MC WAIT FLYBACK** (1 cuadro) |
| `tecla()` | rutina `__tecla` con `&BB09` **KM READ CHAR** (no bloqueante) |
| `aleatorio(n)` | rutina `__rand` (LCG) + `__umod`; semilla con `&BD0D` **KL TIME PLEASE** |

### 5.2 Rutinas de soporte (runtime) añadidas

Se emiten **solo si el programa las usa** (se detecta recorriendo las cuádruplas con el
nuevo helper `tac_usa`), para no engordar los programas que no las necesitan:

- `__tecla` — devuelve en `HL` el código de la tecla (0 si no hay ninguna).
- `__esperar` — espera en `HL` cuadros de vídeo, sincronizando el juego a ~50 Hz.
- `__rand` — generador congruencial lineal de 16 bits: `semilla = semilla*25173 + 13849`.
- `__umod` — `HL = HL mod DE` sin signo (reutiliza `__udiv16` existente).
- Variable `__seed` (`defw`) para la semilla; se **siembra** al arrancar con el reloj del
  sistema (`&BD0D`), de modo que cada partida es distinta.

### 5.3 Compatibilidad con el mini‑ensamblador

Todo el código nuevo usa **únicamente** instrucciones ya soportadas por `gui/z80asm.py`
(cargas, aritmética de 16 bits, `jr c/nz`, `push/pop`, `call &BBxx/&BDxx`, `ex de,hl`,
`defw`…). **No se modificó el ensamblador ni el generador de discos** (`cpcdsk.py`).

---

## 6. Juegos jugables e integración con la GUI

Cada juego tiene un **esquema de control propio y consistente**:

- **`ejemplos/spacewar.g5z80`** — *Spacewar* con nave, asteroides y **disparos**.
  Controles de **3 teclas**:

  | Tecla | Código | Acción |
  |-------|:------:|--------|
  | `O` | 111 / 79 | mover a la izquierda |
  | `P` | 112 / 80 | mover a la derecha |
  | `ESPACIO` | 32 | disparar |

  Destruye asteroides con el disparo (**+5 pts**) o esquívalos al pasar (**+1 pt**);
  cada golpe resta una vida; con 0 vidas → *GAME OVER*. El juego **acelera** con el tiempo.

- **`ejemplos/juego_adivina.g5z80`** — *Adivina el número* ahora es **interactivo**: la
  computadora elige un secreto 1‑100 con `aleatorio(100)+1` y **tú** lo adivinas usando
  **solo las teclas numéricas**:

  | Tecla | Código | Acción |
  |-------|:------:|--------|
  | `0`–`9` | 48–57 | escribir el número |
  | `ENTER` | 13 | probar el número |

  (Antes se "auto‑jugaba" por falta de teclado; ahora lee dígitos, los acumula
  `guess = guess*10 + (k-48)` y compara al pulsar ENTER, indicando *muy alto* / *muy bajo*
  / *correcto* y contando los intentos.)

- **`gui/server.py`** — se añadió *"🚀 Juego: Spacewar (jugable)"* como primer ejemplo del
  desplegable **Cargar ejemplo** (junto al de *Adivina el Número*).

### Pipeline verificado (sin regresiones)

| Programa | Léxico | Sint. | Sem. | TAC | Z80 | Ensamblado | .dsk |
|----------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| `spacewar.g5z80` (nuevo, 3 teclas + disparo) | ✔ | ✔ | ✔ | ✔ | ✔ | 1894 bytes | ✔ |
| `juego_adivina.g5z80` (interactivo, teclas numéricas) | ✔ | ✔ | ✔ | ✔ | ✔ | 1081 bytes | ✔ |
| `ejemplo1.g5z80` | ✔ | ✔ | ✔ | ✔ | ✔ | 355 bytes | ✔ |

Los ejemplos que **no** usan las primitivas nuevas (p. ej. `ejemplo1.g5z80`) compilan
**idénticamente** a antes, porque los runtimes de juego se emiten solo bajo demanda.

---

## 7. Resumen de archivos tocados

| Fase | Archivo(s) |
|------|-----------|
| Léxico | `src/lexer.l`, `src/tokens.c` |
| Sintáctico + AST | `src/parser.y`, `src/ast.h`, `src/ast.c`, `src/gui_salida.c` |
| Semántico | `src/semantico.c` |
| Código intermedio | `src/codigo_intermedio.c` |
| Generación Z80 | `src/generador_z80.c` |
| Juego / GUI | `ejemplos/spacewar.g5z80`, `gui/server.py` |

> No se tocaron `gui/z80asm.py` ni `gui/cpcdsk.py`: el código Z80 nuevo cabe dentro del
> subconjunto de instrucciones que el mini‑ensamblador ya soportaba.
