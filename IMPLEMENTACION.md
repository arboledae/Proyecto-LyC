# Cómo funciona el Compilador Dummy — Guía de implementación

Este documento explica **cómo está construido** el compilador (no solo cómo usarlo — para
eso está el `README.md`). Está pensado para acompañar al informe del curso y cubrir las
fases que aún no tienen informe propio (Fase 3, 4 y 5, más la extensión DSK/WinAPE).

---

## 1. Panorama general

El compilador procesa un archivo `.g5z80` en **cinco fases secuenciales**, cada una
implementada en su propio módulo de `src/`, y todas encadenadas desde `src/parser.y`
(que además contiene la función `main`):

```
código fuente (.g5z80)
   │
   ▼
① LÉXICO           src/lexer.l → src/lex.yy.c (generado por Flex)
   │  tokens
   ▼
② SINTÁCTICO + AST  src/parser.y → src/parser.tab.c/.h (generado por Bison)
   │                src/ast.c/.h
   │  árbol de sintaxis abstracta (NodoAST)
   ▼
③ SEMÁNTICO         src/semantico.c/.h  +  src/simbolos.c/.h
   │  mismo AST, ya validado (tipos, ámbitos, variables)
   ▼
④ CÓDIGO INTERMEDIO src/codigo_intermedio.c/.h
   │  lista de cuádruplas (TAC)
   ▼
⑤ GENERACIÓN Z80    src/generador_z80.c/.h
   │  listado de ensamblador Z80 (texto)
   ▼
(fuera de C, en Python) gui/z80asm.py → binario → gui/cpcdsk.py → imagen .dsk → WinAPE
```

Cada fase solo depende de la salida de la anterior — el AST no sabe nada de tipos, el
TAC no sabe nada de Z80, etc. Esto es justamente lo que permite que el mismo binario
`compilador.exe` sirva tanto para la consola como para la interfaz web: lo único que
cambia es *cómo se imprime* el resultado de cada fase (ver sección 7).

---

## 2. Fase 1 — Análisis léxico (`src/lexer.l`)

Flex genera `src/lex.yy.c` a partir de las reglas de `lexer.l`. Cada regla reconoce un
lexema y devuelve el código de token correspondiente (definido por Bison en
`parser.tab.h`, por eso `lexer.l` incluye ese header).

Categorías de tokens:

| Categoría | Ejemplos | Token |
|---|---|---|
| Palabras clave de control | `inicio`, `cuando`, `sino`, `loop` | `KW_INICIO`, `KW_CUANDO`, `KW_SINO`, `KW_LOOP` |
| Palabras clave de I/O | `imprimir`, `retornar` | `KW_IMPRIMIR`, `KW_RETORNAR` |
| Tipos de dato | `num`, `dec`, `text` | `KW_NUM`, `KW_DEC`, `KW_TEXT` |
| Literales | `10`, `9.5`, `"hola"`, `verdadero`/`falso` | `LIT_ENTERO`, `LIT_DECIMAL`, `LIT_TEXT`, `LIT_BOOLEANO` |
| Identificadores | `x`, `promedio` | `IDENTIFICADOR` |
| Operadores | `+ - * / % > < >= <= == != && \|\| !` | `OP_*` |
| Delimitadores | `( ) { } ;` | `DELIM_*` |
| Comentarios | `%% texto %%` | se descartan, no generan token |

> **Nota sobre las palabras clave:** el lenguaje mezcla términos en español
> (`inicio`, `cuando`, `sino`, `imprimir`, `retornar`) con términos que se dejaron en
> inglés/abreviados (`loop`, `num`, `dec`, `text`). Las nueve están documentadas desde
> la Semana 2 (ver Cronograma e Informe de Análisis Léxico del curso).

El analizador léxico también puede ejecutarse solo, sin parser, con
`analizador_lexico.exe` (usa `src/main_lexer.c` + `lex.yy.c` + `tokens.c`), útil para
depurar el reconocimiento de tokens de forma aislada.

---

## 3. Fase 2 — Sintáctico + AST (`src/parser.y`, `src/ast.c`)

### 3.1. Gramática y precedencia

Bison genera `parser.tab.c`/`.h` a partir de la gramática BNF de `parser.y`. La
precedencia de operadores (de menor a mayor) es:

```
%left  OP_OR
%left  OP_AND
%left  OP_IGUAL OP_DIFERENTE OP_MAYOR OP_MENOR OP_MAYOR_IGUAL OP_MENOR_IGUAL
%left  OP_SUMA OP_RESTA
%left  OP_MULT OP_DIV OP_MOD
%right OP_NOT
```

`condicion` (usada en `cuando (...)` y `loop (...)`) admite agrupar con paréntesis:
`condicion: ... | DELIM_PAR_IZQ condicion DELIM_PAR_DER`. Esto permite escribir
`!(x == 0)` o `(a > b) && (c < d)`. Genera un único conflicto reduce/reduce, acotado al
caso `(verdadero)`/`(falso)` aislado entre paréntesis (ambiguo entre "condición" y
"expresión"); Bison lo resuelve a favor de "condición", que es la lectura correcta —
queda documentado con un comentario en `parser.y` junto a `%start`.

### 3.2. El AST (`src/ast.h`)

Cada nodo es un `NodoAST` con un `tipo` (enum `TipoNodo`), dos campos de valor
(`str_val`, `int_val`) y **tres punteros genéricos** (`izq`, `der`, `extra`) que se
reinterpretan según el tipo de nodo:

| Tipo de nodo | `izq` | `der` | `extra` |
|---|---|---|---|
| `NODO_PROGRAMA` | lista de sentencias | — | — |
| `NODO_LISTA_SENT` | sentencia actual | — | siguiente de la lista |
| `NODO_DECLARACION` | — | expresión inicial (o `NULL`) | — (tipo/nombre en `str_val`) |
| `NODO_ASIGNACION` | — | expresión | — |
| `NODO_CONDICIONAL` | condición | bloque `cuando` | bloque `sino` (o `NULL`) |
| `NODO_CICLO` | condición | cuerpo del `loop` | — |
| `NODO_OP_BINARIO`/`NODO_OP_UNARIO` | operando(s) | — | (operador en `str_val`) |

Las acciones semánticas de `parser.y` construyen el árbol directamente (no hay un paso
intermedio); `imprimir_ast()` lo recorre en preorden para la consola y
`emitir_ast_gui()` (en `gui_salida.c`) para la interfaz web.

---

## 4. Fase 3 — Análisis semántico (`src/semantico.c`, `src/simbolos.c`)

### 4.1. Tabla de símbolos con ámbitos (`simbolos.c`)

Cada bloque `{ }` abre un **ámbito** con un id numérico creciente: `inicio { }` es el
ámbito global (`0`); cada cuerpo de `cuando`/`sino`/`loop` recibe el siguiente id
disponible. La visibilidad de una variable se resuelve con una **pila de ámbitos
activos** — es visible si su ámbito sigue en la pila en ese punto del recorrido (del
más interno al más externo), igual que en C. Los símbolos **no se destruyen** al cerrar
un ámbito (se conservan para poder mostrar la tabla completa en la interfaz), pero
`buscar()` solo mira los ámbitos que siguen abiertos, así que la corrección del chequeo
no se ve afectada.

### 4.2. Verificaciones (`semantico.c`)

Recorre el mismo AST y acumula errores (no aborta al primero) para:

- Variable no declarada (`"Variable 'x' no declarada."`).
- Doble declaración en el mismo ámbito.
- Tipo incompatible en declaración o asignación (`num` vs `text` vs `dec`).
- Operación aritmética o comparación inválida entre tipos.
- La condición de `cuando`/`loop` debe ser de tipo `booleano`.

Si `hay_errores_sem()` es verdadero, el compilador **no** genera código intermedio ni
Z80 — por diseño, un programa con errores semánticos nunca llega a la Fase 4/5 (esto es
justamente lo que valida `gui/server.py` antes de ensamblar: ver sección 8).

---

## 5. Fase 4 — Código intermedio / TAC (`src/codigo_intermedio.c`)

Traduce el AST ya validado a una lista enlazada de **cuádruplas** `(op, arg1, arg2,
res)` con temporales `t1..tn` y etiquetas `L1..Ln`. Ejemplos de traducción:

**`cuando`/`sino`** — un salto condicional hacia el bloque `sino` (o el final si no
hay), y un `goto` al final tras ejecutar el bloque verdadero:

```
      ifFalse cond, L_sino     (o L_fin si no hay sino)
      ...bloque si...
      goto L_fin
L_sino:
      ...bloque sino...
L_fin:
```

**`loop`** — la condición se reevalúa en cada vuelta (patrón "evaluar arriba, saltar
abajo"):

```
L_ini:
      ifFalse cond, L_fin
      ...cuerpo...
      goto L_ini
L_fin:
```

Las variables se identifican de forma única entre ámbitos (`x` del ámbito 2 se llama
`x.2` internamente) para que dos variables con el mismo nombre en bloques distintos no
choquen al pasar a Z80.

---

## 6. Fase 5 — Generación de código Z80 (`src/generador_z80.c`)

### 6.1. Modelo de memoria

- El programa se ensambla en `org &4000` (memoria libre del Amstrad CPC, por debajo de
  donde vive normalmente BASIC).
- Cada variable y cada temporal del TAC se reserva como una palabra de 16 bits con
  signo (`v_0`, `v_1`, ..., `t_1`, `t_2`, ...) en una sección de datos al final del
  listado.
- Las cadenas de texto se deduplican en un *pool* (`str_0`, `str_1`, ...) y se
  transliteran de UTF-8 a el charset del CPC (acentos → letra sin tilde, `¡`/`¿` → `!`/`?`).

### 6.2. Traducción de cuádruplas a Z80

Cada cuádrupla del TAC se traduce a un puñado de instrucciones que cargan operandos en
`HL`/`DE` y llaman a una rutina de soporte cuando la operación no es trivial:

| Operación TAC | Estrategia Z80 |
|---|---|
| `+`, `-` | `add hl,de` / `sbc hl,de` directo |
| `*`, `/`, `%` | llaman a `__mul16` / `__div16` / `__mod16` (multiplicación por desplazamiento, división binaria restauradora) |
| `<`, `>`, `<=`, `>=` | todas se resuelven con una sola rutina `__lt` intercambiando operandos (`a>b` ⇔ `b<a`) e invirtiendo el resultado para `<=`/`>=` |
| `==`, `!=` | `__eq` (resta y compara con cero), invertido para `!=` |
| `&&`, `\|\|` | AND/OR bit a bit sobre el byte bajo (los booleanos son 0/1) |
| `imprimir` | `__prstr` (cadena) o `__prnum` (entero con signo, convierte a decimal ASCII dígito por dígito) usando el firmware `&BB5A` (TXT_OUTPUT) |

### 6.3. Qué pasa al terminar el programa

No hay sistema operativo al que "volver": el programa Z80 generado siempre termina en
`__fin` con un bucle infinito (`jp __stop`). La única salida visible es lo que se haya
impreso con `imprimir(...)` — el programa se queda ahí, congelado, esperando (esto es
intencional y normal en código bare-metal para CPC; **antes** el compilador dibujaba
además un cuadro de color sin relación con el programa como remanente de pruebas —
se eliminó por confuso e innecesario).

---

## 7. Modo `--gui`: cómo el C le habla a Python

`compilador.exe --gui archivo.g5z80` no imprime el formato "bonito" de consola: emite un
flujo de texto delimitado por secciones `@@NOMBRE`, con `|` como separador de campos
dentro de cada sección (sin JSON en C, para no añadir una librería):

```
@@TOKENS      <linea>|<TOKEN>|<lexema>
@@AST         <profundidad>|<etiqueta>
@@SIMBOLOS    <nombre>|<tipo>|<scope>|<linea>|<si|no>
@@ERRORES     <lexico|sintactico|semantico>|<linea>|<mensaje>
@@INTERMEDIO  <n>|<op>|<arg1>|<arg2>|<res>|<texto legible>
@@Z80         <líneas de ensamblador, tal cual>
@@ESTADO      sintactico|ok      (o "error")
              semantico|ok       (o "error"/"na" si no llegó a esa fase)
              intermedio|ok      (o "na")
              z80|ok             (o "na")
@@END
```

`gui/server.py` (`parsear_salida_gui()`) parsea esto línea por línea y arma el JSON que
consume `gui/static/app.js`. Cada sección la emite el módulo responsable de esa fase:
`gui_salida.c` para tokens/AST, `simbolos.c` para la tabla de símbolos,
`semantico.c`/`parser.y` para errores, `codigo_intermedio.c` para el TAC y
`generador_z80.c` para el listado Z80.

---

## 8. De Z80 a WinAPE: el pipeline completo

`compilador.exe` solo llega hasta *texto* en ensamblador Z80 — no genera binarios. Eso
lo hacen dos scripts de Python, sin dependencias externas:

```
listado Z80 (texto)
   │
   ▼  gui/z80asm.py   — mini-ensamblador Z80 de dos pasadas
binario .bin
   │
   ▼  gui/cpcdsk.py   — empaqueta en formato de disco CPCEMU
imagen .dsk (40 pistas × 9 sectores × 512 B, cabecera AMSDOS)
   │
   ▼  WinApe.exe <imagen.dsk> /A:MAIN
se abre el emulador y auto-ejecuta RUN"MAIN
```

`gui/server.py` (`compilar_a_dsk()`) encadena todo esto y **primero valida** que
`sintactico_ok` y `semantico_estado == "ok"` antes de ensamblar — un programa con
errores nunca llega a generar un `.dsk`. `buscar_winape()` localiza `WinApe.exe` en
`tools/winape/` (o en la variable de entorno `WINAPE`); si no lo encuentra, la interfaz
avisa que falta ejecutar `tools\descargar_winape.ps1`.

---

## 9. Construcción del proyecto

```
make            # bison + flex + gcc → compilador.exe (5 fases)
make lex        # solo el analizador léxico independiente
make gui        # compila todo y lanza gui/server.py en http://localhost:5000
make clean      # borra generados (parser.tab.*, lex.yy.c, binarios)
```

El `Makefile` encadena las dependencias correctamente: `parser.y` → (bison) →
`parser.tab.c/.h` → (flex, que necesita los códigos de token) → `lex.yy.c` → (gcc) →
`compilador.exe`. El instalador de Windows (`installer/windows.iss`) empaqueta los
binarios ya compilados, un Python embebido, la GUI y WinAPE en un único `Setup.exe`.

---

## 10. Resumen de archivos por fase

| Fase | Archivos |
|---|---|
| 1. Léxico | `src/lexer.l`, `src/tokens.c/.h`, `src/main_lexer.c` |
| 2. Sintáctico + AST | `src/parser.y`, `src/ast.c/.h` |
| 3. Semántico | `src/simbolos.c/.h`, `src/semantico.c/.h` |
| 4. Código intermedio | `src/codigo_intermedio.c/.h` |
| 5. Generación Z80 | `src/generador_z80.c/.h` |
| Puente hacia la GUI | `src/gui_salida.c/.h` |
| Interfaz web | `gui/server.py`, `gui/static/*` |
| Ensamblador + disco | `gui/z80asm.py`, `gui/cpcdsk.py` |
| Emulador | `tools/winape/`, `tools/descargar_winape.ps1` |
