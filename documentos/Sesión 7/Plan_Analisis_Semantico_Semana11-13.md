# Plan de Implementación: Análisis Semántico + Interfaz Gráfica

**Curso:** Lenguajes y Compiladores — Grupo 5
**Lenguaje:** Dummy (especificación Grupo 5)
**Periodo:** Semanas 11, 12 y 13 (Fase 3 – Semántico + Entregable GUI)
**Docente:** Prudencio Vidal, Javier Antonio

---

## 0. Contexto y justificación

### 0.1 Estado actual del proyecto (hasta Semana 10)
El compilador del lenguaje Dummy (carpeta `Proyecto-LyC`) ya cuenta con:
- **Análisis léxico** (`src/lexer.l`) en Flex: reconoce keywords, identificadores, literales (entero, decimal, texto, booleano), operadores, delimitadores y comentarios `%% ... %%`.
- **Análisis sintáctico** (`src/parser_rd.c`): parser predictivo LL(1) por descenso recursivo escrito a mano (teoría top-down de la S9y10); gramática BNF, precedencia por niveles, manejo de errores con línea y mensaje.
- **AST** (`src/ast.h`, `src/ast.c`): nodos para programa, lista de sentencias, declaración, asignación, condicional, ciclo, impresión, retorno, op. binario/unario y literales; funciones `imprimir_ast` y `liberar_ast`.
- **Ejemplos** (`ejemplos/ejemplo1.g5z80`, `ejemplos/ejemplo_con_error.g5z80`).

### 0.2 Qué falta
- **Análisis semántico** (semanas 11 y 12 del cronograma): tabla de símbolos, manejo de scope, verificación de tipos, control de variables declaradas.
- **Interfaz gráfica** (pedido final del profesor): caja de texto + botones para visualizar el análisis léxico, sintáctico y semántico.

### 0.3 Nota sobre el cronograma
Según el cronograma, la **Semana 13** corresponde a *Fase 4 – generación de código (3 direcciones / Z80)*. No obstante, el profesor reorientó el entregable hacia una **interfaz gráfica para el análisis semántico**. Por ello, este plan prioriza **semántico + GUI**, dejando la generación de código como trabajo futuro/opcional.

---

## 1. Resumen del lenguaje Dummy (base semántica)

| Elemento | Detalle |
|---|---|
| Tipos declarables | `num` (entero), `dec` (decimal), `text` (cadena) |
| Literal booleano | `verdadero` / `falso` (no es tipo declarable; aparece en condiciones) |
| Estructuras | `inicio { }`, `cuando (cond) { } sino { }`, `loop (cond) { }`, `imprimir(expr)`, `retornar expr` |
| Bloques `{ }` | Crean ámbitos (scopes) anidados → base de la tabla de símbolos |
| Comentarios | `%% ... %%` (ignorados) |

---

## 2. Herramientas y librerías a instalar

### 2.1 Ya verificadas en el entorno
- `flex`, `bison`, `gcc`, `build-essential`, `make`, `pdftotext`.

### 2.2 Por instalar (solo para la GUI)
```bash
# Linux (Debian/Ubuntu)
sudo apt-get install python3 python3-pip make
pip3 install flask

# Windows
# Instalar Python 3 desde python.org y luego:
pip install flask
```
> No se requieren librerías C adicionales. La GUI es web (HTML/CSS/JS vanilla, sin framework) y el compilador sigue siendo C puro, manteniendo el trabajo de Flex/Bison intacto.

### 2.3 Estructura final del proyecto
```
Proyecto-LyC/
├── src/
│   ├── lexer.l            # (modificado) acumula tokens para modo JSON
│   ├── parser.y           # (modificado) integra análisis semántico + modo JSON
│   ├── ast.h / ast.c      # (sin cambios mayores)
│   ├── simbolos.h / .c    # NUEVO – tabla de símbolos + scope
│   ├── semantico.h / .c   # NUEVO – verificación semántica sobre el AST
│   ├── json.h / .c        # NUEVO – emisión de JSON estructurado
│   └── main_lexer.c
├── gui/                   # NUEVO – interfaz gráfica
│   ├── app.py             # backend Flask
│   ├── templates/index.html
│   └── static/ (css, js)
├── ejemplos/
├── Makefile               # NUEVO – automatiza bison/flex/gcc
└── README.md              # (actualizado)
```

---

## 3. FASE 1 — Tabla de símbolos (Semana 11)

**Archivos nuevos:** `src/simbolos.h`, `src/simbolos.c`

### 3.1 Estructura de un símbolo
```c
typedef struct Simbolo {
    char *nombre;     // nombre de la variable/función
    char *tipo;       // "num", "dec", "text", "booleano", o "func"
    int   scope;      // nivel de ámbito (0 = global)
    int   linea;      // línea de declaración
    int   inicializado;
    struct Simbolo *siguiente;
} Simbolo;
```

### 3.2 Gestión de scopes (pila de ámbitos)
- `int scope_actual;` — contador que **sube** al entrar a un bloque `{` y **baja** al salir `}`.
- `entrar_scope()` → `scope_actual++`.
- `salir_scope()` → elimina símbolos del scope actual y `scope_actual--` (opcional: conservar para impresión final).

### 3.3 Funciones API
| Función | Acción |
|---|---|
| `declarar(nombre, tipo, linea)` | Agrega símbolo al scope actual. Error si ya existe en el mismo scope. |
| `buscar(nombre)` | Busca en scope actual y externos (visibilidad). |
| `buscar_local(nombre)` | Busca solo en el scope actual. |
| `entrar_scope()` / `salir_scope()` | Gestión de bloques. |
| `imprimir_tabla()` | Volcado para consola y JSON. |

### 3.4 Alcance
- El bloque `inicio { }` es el scope global (0).
- Los cuerpos de `cuando`, `sino` y `loop` abren un nuevo scope.
- (Preparado para funciones: registro de nombre, tipo de retorno y parámetros — la gramática actual no declara funciones, pero la estructura lo permite.)

---

## 4. FASE 2 — Verificación semántica (Semana 12)

**Archivos nuevos:** `src/semantico.h`, `src/semantico.c`

Se recorre el AST construido por el analizador por descenso recursivo y se ejecutan las verificaciones. Cada verificación registra errores en una lista con `{ linea, mensaje }`.

### 4.1 Inferencia de tipos de expresiones
Cada nodo de expresión retorna un tipo inferido:
| Nodo | Tipo inferido |
|---|---|
| `NODO_ENTERO` | `num` |
| `NODO_DECIMAL` | `dec` |
| `NODO_TEXTO` | `text` |
| `NODO_BOOLEANO` | `booleano` |
| `NODO_IDENTIFICADOR` | tipo declarado en la tabla (error si no existe) |
| `NODO_OP_BINARIO` aritmético (`+ - * / %`) | `num` si ambos `num`; `dec` si alguno `dec`; error si `text`/`booleano` |
| `NODO_OP_BINARIO` relacional (`> < == != >= <=`) | `booleano` (operandos `num`/`dec`) |
| `NODO_OP_BINARIO` lógico (`&& \|\|`) | `booleano` (operandos `booleano`) |
| `NODO_OP_UNARIO` (`!`, `-`) | `booleano` / `num`·`dec` |

### 4.2 Reglas semánticas a verificar
1. **Uso de variable no declarada** → `buscar(nombre)` falla.
2. **Doble declaración en el mismo scope** → `buscar_local(nombre)` existe.
3. **Asignación incompatible**: el tipo de la expresión debe ser asignable al tipo de la variable.
4. **Declaración con inicialización incompatible** (ej. `num x = "hola"`).
5. **Operación inválida entre tipos** (ej. `text + num`, `text > num`).
6. **Condición de `cuando`/`loop` debe ser `booleano`**.
7. **`imprimir`** acepta cualquier tipo (solo advertencia si es `void`).
8. (Opcional) **`retornar`** con tipo coherente.

### 4.3 Reglas de compatibilidad de tipos
- `num` ↔ `dec`: compatibles (widening `num→dec`).
- `text`: solo asignable a `text`.
- `booleano`: solo asignable a `booleano` y válido en condiciones.

### 4.4 Reporte de errores
```c
typedef struct ErrorSem {
    int   linea;
    char *mensaje;
    struct ErrorSem *siguiente;
} ErrorSem;
```
Salida de ejemplo:
```
[ERROR SEMANTICO] Linea 5: Variable 'y' no declarada.
[ERROR SEMANTICO] Linea 8: Tipo incompatible: no se puede asignar 'text' a 'num'.
```

---

## 5. FASE 3 — Integración en el parser

**Archivos:** `src/main.c` (driver) + `src/parser_rd.c` (descenso recursivo)

- Tras construir el AST raíz, llamar a `analizar_semantico(raiz)` **antes** de `liberar_ast(raiz)`.
- Flujo final del `main`:
  1. Lexer → tokens.
  2. Parser → AST.
  3. `analizar_semantico(raiz)` → tabla de símbolos + errores.
  4. Imprimir AST, tabla de símbolos y errores semánticos.
  5. Liberar memoria.
- **Modo GUI (`--gui`):** NO se usa JSON. El compilador emite una salida de texto **delimitada simple** (campos separados por `|`) que es trivial de generar en C con `printf` y que Python/JS parsean con un `split`. Esto evita añadir código de serialización JSON en C y mantiene el compilador simple. La salida legible (AST, tabla, errores) se conserva para consola.

### 5.1 Formato delimitado de salida para la GUI (sin JSON)
Sección por sección, líneas con campos separados por `|`:
```
@@TOKENS
2|KW_NUM|num
2|IDENTIFICADOR|x
2|OP_ASIGNACION|=
2|LIT_ENTERO|10
@@AST
PROGRAMA
  DECLARACION|num|x
    VALOR_INICIAL
      LIT_ENTERO|10
@@SIMBOLOS
x|num|0|2|si
promedio|dec|0|3|si
@@ERRORES
5|Variable 'y' no declarada.
8|Tipo incompatible: no se puede asignar 'text' a 'num'.
@@END
```
> El frontend lee las marcas `@@TOKENS`, `@@AST`, `@@SIMBOLOS`, `@@ERRORES` y construye las vistas gráficas. Si en el futuro se prefiere JSON, puede generarse desde Python a partir de este formato, sin tocar el C.

---

## 6. FASE 4 — Interfaz gráfica (entregable del profesor)

**Arquitectura:** App web con **backend Flask** (Python) que invoca el binario C `compilador --gui` (formato delimitado, **sin JSON**) y **frontend HTML/CSS/JS vanilla** que renderiza los resultados.

### 6.0 Todo debe ser gráfico (incluido cualquier autómata)
El profesor pidió una interfaz **gráfica**. Por ello, todas las representaciones en la GUI serán visuales:
- **Léxico:** tabla de tokens coloreada por categoría. Si se muestran autómatas (AFD/AFN de los PDFs), se renderizan como **diagramas gráficos** (SVG/HTML), nunca como texto.
- **Sintáctico:** árbol AST dibujado como **árbol visual** (nodos colapsables en HTML o SVG).
- **Semántico:** tabla de símbolos como tabla HTML y errores como lista coloreada por línea.
> Nota: el análisis semántico en sí **no usa autómatas** (los autómatas pertenecen al léxico). Si se incorporan autómatas en alguna vista, su representación será obligatoriamente gráfica.

### 6.1 Backend — `gui/app.py`
- Ruta `GET /` → sirve `index.html`.
- Ruta `POST /analizar` → recibe `{ codigo }`, lo escribe a un archivo temporal `.g5z80`, ejecuta `./compilador --gui tmp.g5z80`, captura la salida delimitada y la devuelve al frontend.
- Maneja errores de compilación y timeouts.

### 6.2 Frontend — `gui/templates/index.html` + `static/`
- **Panel izquierdo:** `textarea` con código de ejemplo precargado (botón “Cargar ejemplo”).
- **Botones:** `Analizar Léxico`, `Analizar Sintáctico`, `Analizar Semántico`, `Analizar Todo`.
- **Panel derecho (pestañas):**
  - **Léxico:** tabla de tokens (`línea`, `token`, `lexema`) con colores por categoría.
  - **Sintáctico:** árbol AST renderizado como árbol anidado colapsable (o SVG).
  - **Semántico:** tabla de símbolos (`nombre`, `tipo`, `scope`, `línea`, `init`) + lista de errores con línea y color (rojo error / verde OK).
- Estilo limpio y responsivo (CSS propio, sin dependencias externas/CDN para funcionar offline).

### 6.3 Ejecución de la GUI
```bash
# 1. Compilar el compilador C
make
# 2. Levantar la interfaz
cd gui && python3 app.py
# 3. Abrir http://localhost:5000
```

---

## 7. FASE 5 — Automatización y documentación

- **`Makefile`** (nuevo): targets `all` (bison+flex+gcc), `clean`, `run`, `gui`.
- **`README.md`** actualizado con instrucciones del semántico y la GUI.
- **Informe final** en `documentos/Sesión 7/` (Semana 11–13) siguiendo el formato de los avances anteriores.

---

## 8. Orden de ejecución (checklist)

1. [ ] Crear `Makefile` y verificar que el build actual funciona.
2. [ ] `src/simbolos.h` / `simbolos.c` — tabla de símbolos + scopes.
3. [ ] `src/semantico.h` / `semantico.c` — verificaciones + inferencia de tipos.
4. [ ] Modificar `src/parser.y` — llamar `analizar_semantico(raiz)`; imprimir tabla y errores.
5. [ ] Probar semántico en consola con `ejemplo1.g5z80` y casos con error.
6. [ ] Modificar `lexer.l`/`parser.y` para emitir formato delimitado en modo `--gui` (sin JSON).
7. [ ] `gui/app.py` (Flask) — endpoint `/analizar`.
8. [ ] `gui/templates/index.html` + `static/` — interfaz con 3 vistas.
9. [ ] Pruebas integrales: cargar ejemplo, pulsar botones, verificar las 3 vistas.
10. [ ] Actualizar `README.md` y redactar informe final en `documentos/Sesión 7/`.

---

## 9. Casos de prueba semántica sugeridos

**Válido:**
```
inicio {
  num x = 10;
  dec y = 2.5;
  cuando (x > 5) {
    imprimir(x);
  } sino {
    imprimir(y);
  }
}
```
**Con errores semánticos:**
```
inicio {
  num x = 10;
  x = "hola";          // tipo incompatible
  imprimir(z);         // variable no declarada
  num x = 5;           // doble declaración
  cuando (x) { }       // condición no booleana
}
```

---

*Plan elaborado a partir del cronograma (S3), el informe del lenguaje (S2), los avances S9/S10 y el código fuente actual del proyecto.*

---

## 10. Implementación inmediata (en ejecución ahora)

A continuación se detalla lo que se va a implementar **en este momento** sobre el proyecto `Proyecto-LyC`: las **Fase 1 (tabla de símbolos)** y **Fase 2 (verificación semántica)**, dejando el compilador con análisis semántico funcional y probado en consola. La interfaz gráfica (Fase 4) y el modo `--gui` (Fase 3) quedan para un segundo paso.

### 10.1 Cambios en archivos existentes
- **`src/ast.h`**: añadir campo `int linea;` a `NodoAST` para poder reportar errores semánticos con número de línea.
- **`src/ast.c`**: `nuevo_nodo()` registra `n->linea = yylineno;` (mediante `extern int yylineno;`), de modo que cada nodo queda marcado con la línea donde se construyó. No se rompe la compilación del lexer independiente porque `ast.c` solo se enlaza con el compilador completo.
- **`src/parser.y`**: tras construir el AST raíz, llamar a `analizar_semantico(raiz)` (antes de `liberar_ast`), imprimir la **tabla de símbolos** y la **lista de errores semánticos**. Incluir `simbolos.h` y `semantico.h`.

### 10.2 Archivos nuevos — Fase 1: Tabla de símbolos
**`src/simbolos.h`** y **`src/simbolos.c`**
- `struct Simbolo { char *nombre; char *tipo; int scope; int linea; int inicializado; struct Simbolo *siguiente; }`.
- Pila de scopes con `entrar_scope()` / `salir_scope()` (el bloque `inicio{}` es scope 0; los cuerpos de `cuando`/`sino`/`loop` abren un nuevo scope).
- API: `tabla_init()`, `declarar()`, `buscar()` (visibilidad), `buscar_local()`, `marcar_inicializado()`, `imprimir_tabla()`, `liberar_tabla()`, `scope_actual()`.
- `declarar()` retorna error si la variable ya existe en el scope actual (doble declaración).

### 10.3 Archivos nuevos — Fase 2: Verificación semántica
**`src/semantico.h`** y **`src/semantico.c`**
- Recorrido recursivo del AST (`analizar_semantico(raiz)`).
- Inferencia de tipos por nodo de expresión (`num`, `dec`, `text`, `booleano`).
- Verificaciones:
  1. Variable usada pero no declarada.
  2. Doble declaración en el mismo scope.
  3. Asignación con tipo incompatible.
  4. Declaración con inicialización incompatible.
  5. Operación inválida entre tipos (`text + num`, `text > num`, `booleano && num`).
  6. Condición de `cuando`/`loop` debe ser `booleano`.
  7. `imprimir` y `retornar`: se valida que la expresión sea correcta.
- Reglas de compatibilidad: `num`↔`dec` (widening); `text` solo con `text`; `booleano` solo con `booleano`.
- Lista enlazada de `ErrorSem { int linea; char *mensaje; }` con `imprimir_errores_sem()` y `hay_errores_sem()`.

### 10.4 Automatización
- **`Makefile`** nuevo con targets: `all` (bison→flex→gcc), `lex` (solo lexer), `run` (ejecuta con un ejemplo), `clean`.

### 10.5 Pruebas a realizar (validación en vivo)
1. Compilar con `make`.
2. Ejecutar `./compilador ejemplos/ejemplo1.g5z80` → debe mostrar AST, tabla de símbolos y **0 errores semánticos**.
3. Crear `ejemplos/ejemplo_semantico_error.g5z80` con errores (variable no declarada, doble declaración, tipo incompatible, condición no booleana) y verificar que se reportan con la línea correcta.
