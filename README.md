# Compilador Dummy → Z80 (Amstrad CPC) — Grupo 5

Compilador completo del lenguaje **Dummy** (especificación Grupo 5, UNMSM) que cubre las **cinco fases** de compilación:

1. **Análisis léxico** (Flex)
2. **Análisis sintáctico + AST** (Bison)
3. **Análisis semántico** (tabla de símbolos, tipos, ámbitos)
4. **Código intermedio** (cuádruplas / tres direcciones)
5. **Generación de código Z80** (Amstrad CPC)

Incluye una **interfaz gráfica web** que visualiza cada fase, un **ensamblador Z80** y un **generador de discos .dsk** (ambos en Python), y un botón para **ejecutar el programa en WinAPE** (emulador de Amstrad CPC).

---

## ⬇️ Descargar e instalar (para los compañeros)

**No hace falta compilar nada.** Descarga el instalador ya listo (Python va embebido,
no necesitas instalar nada):

- En **Releases** → `CompiladorLyC-Setup.exe` (etiqueta *Instalador de Windows*), o
- En la pestaña **Actions** → última ejecución de *"Instalador de Windows"* → artefacto **`CompiladorLyC-Setup`**.

Ejecuta el `Setup.exe` (instala por usuario, **no pide administrador**), abre el acceso
directo **"Compilador LyC"** y se abre solo el navegador con la interfaz. Para cerrarlo,
cierra la ventana negra (consola) del servidor.

> **¿Cómo se genera ese instalador?** Automáticamente en **GitHub Actions**: pestaña
> *Actions* → *"Instalador de Windows"* → **Run workflow** (o empuja un tag de versión,
> p. ej. `git tag v1.0.0 && git push --tags`). El instalador queda como artefacto y como
> Release descargable. Para generarlo a mano en tu PC, ver [`installer/BUILD.md`](installer/BUILD.md).

> 📖 ¿Cómo está construido el compilador por dentro? Ver [`IMPLEMENTACION.md`](IMPLEMENTACION.md).

---

## 📂 Estructura del Proyecto

```text
Proyecto-LyC/
├── src/
│   ├── lexer.l                    # Reglas léxicas para Flex
│   ├── parser.y                   # Gramática Bison, acciones, main y modo --gui
│   ├── ast.h / ast.c              # AST: nodos, impresión indentada y liberación
│   ├── simbolos.h / simbolos.c    # Tabla de símbolos con ámbitos (scopes)
│   ├── semantico.h / semantico.c  # Verificación semántica + inferencia de tipos
│   ├── codigo_intermedio.h / .c   # Código intermedio: cuádruplas (TAC)
│   ├── generador_z80.h / .c       # Generación de ensamblador Z80 para Amstrad CPC
│   ├── gui_salida.h / gui_salida.c# Emisión delimitada @@TOKENS/@@AST/... para la GUI
│   ├── tokens.h / tokens.c        # Mapeo código→nombre de token (compartido)
│   └── main_lexer.c               # Driver independiente del analizador léxico
├── gui/                           # Interfaz gráfica web (Python stdlib, sin dependencias)
│   ├── server.py                  # Servidor HTTP: invoca `compilador --gui`, traduce a JSON
│   ├── z80asm.py                  # Mini-ensamblador Z80 (dos pasadas)
│   ├── cpcdsk.py                  # Generador de imágenes de disco .dsk (Amstrad CPC)
│   └── static/
│       ├── index.html             # Página principal de la interfaz
│       ├── style.css              # Estilos de la interfaz
│       └── app.js                 # Lógica del frontend (pestañas, renders, WinAPE)
├── ejemplos/
│   ├── ejemplo1.g5z80             # Programa válido
│   ├── ejemplo_scopes.g5z80       # Ámbitos (scopes) anidados
│   ├── ejemplo_con_error.g5z80    # Error sintáctico intencional
│   └── ejemplo_semantico_error.g5z80  # Errores semánticos
├── installer/                     # Instalador de Windows (Inno Setup)
│   ├── windows.iss                # Receta de Inno Setup
│   ├── construir.bat              # Automatiza la generación del instalador
│   └── BUILD.md                   # Instrucciones paso a paso
├── tools/
│   └── descargar_winape.ps1       # Descarga WinAPE (emulador Amstrad CPC)
├── .github/workflows/
│   └── windows-installer.yml      # CI: compila y genera el instalador automáticamente
├── docs/                          # Documentación e informes del curso (PDF)
├── Makefile                       # Automatiza bison/flex/gcc y lanza la GUI
├── IMPLEMENTACION.md              # Cómo funciona el compilador por dentro (por fase)
└── README.md
```

> Los archivos generados por Bison/Flex (`src/lex.yy.c`, `src/parser.tab.c/.h`) **no se
> versionan**: los regenera `make` (o el CI) a partir de `parser.y` y `lexer.l`.

---

## 🛠️ Prerrequisitos

### En Linux (Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install flex bison build-essential python3
```

### En Windows
- **MSYS2** con MinGW-w64 para obtener `gcc`, `flex`, `bison` y `make`.
- **Python 3** (para la interfaz gráfica web).
- Alternativamente, usa directamente el **instalador** (`CompiladorLyC-Setup.exe`) que empaqueta todo sin necesidad de instalar nada.

---

## 🚀 Compilación

### Con Make (recomendado)

```bash
make            # Compila el compilador completo (5 fases)
make lex        # Compila solo el analizador léxico independiente
make clean      # Limpia archivos generados
```

### Manual

**1. Generar archivos intermedios (Bison & Flex):**
```bash
bison -d src/parser.y -o src/parser.tab.c
flex -o src/lex.yy.c src/lexer.l
```

**2. Compilar el compilador completo (5 fases):**
```bash
gcc -Wall -Isrc src/parser.tab.c src/lex.yy.c src/ast.c \
    src/simbolos.c src/semantico.c src/codigo_intermedio.c \
    src/generador_z80.c src/gui_salida.c src/tokens.c -o compilador
```

**3. Compilar solo el analizador léxico:**
```bash
gcc -Wall -Isrc src/main_lexer.c src/lex.yy.c src/tokens.c -o analizador_lexico
```

> En Windows, agrega `.exe` al nombre del binario y usa `-static` para evitar dependencias de DLL:
> ```bash
> make CFLAGS="-Wall -Isrc -static"
> ```

---

## 🔍 Uso y Verificación

### 1. Analizador léxico independiente

```bash
./analizador_lexico ejemplos/ejemplo1.g5z80
```

**Salida esperada:**
```text
--- INICIANDO ANALISIS LEXICO DEL ARCHIVO ---

[Linea 1] Token: KW_INICIO (258) | Lexema: "inicio"
[Linea 1] Token: DELIM_LLAVE_IZQ (292) | Lexema: "{"
[Linea 2] Token: KW_NUM (264) | Lexema: "num"
...

--- ANALISIS LEXICO FINALIZADO ---
```

### 2. Compilador completo (consola)

```bash
./compilador ejemplos/ejemplo1.g5z80
```

Muestra las 5 fases en secuencia: tokens → AST → tabla de símbolos → cuádruplas TAC → código Z80.

### 3. Modo GUI (formato delimitado)

```bash
./compilador --gui ejemplos/ejemplo1.g5z80
```

Emite un flujo de texto delimitado por secciones `@@`:

```text
@@TOKENS
<linea>|<NOMBRE_TOKEN>|<lexema>
@@AST
<profundidad>|<etiqueta>
@@SIMBOLOS
<nombre>|<tipo>|<scope>|<linea>|<si|no>
@@ERRORES
<lexico|sintactico|semantico>|<linea>|<mensaje>
@@INTERMEDIO
<n>|<op>|<arg1>|<arg2>|<res>|<texto>
@@Z80
<líneas de ensamblador Z80>
@@ESTADO
sintactico|<ok|error>
semantico|<ok|error|na>
intermedio|<ok|na>
z80|<ok|na>
@@END
```

---

## 🧩 Fases del Compilador

### Fase 1 – Análisis Léxico
Expresiones regulares (Flex) que identifican tokens del lenguaje Dummy: palabras clave, literales, operadores, delimitadores e identificadores.

### Fase 2 – Análisis Sintáctico + AST
Gramática Libre de Contexto (Bison) que construye el Árbol de Sintaxis Abstracta con nodos tipados para declaraciones, asignaciones, expresiones, `cuando`/`sino`, `loop`, `imprimir`, etc.

### Fase 3 – Análisis Semántico
- **Tabla de símbolos** con manejo de ámbitos (scopes): `inicio {}` es el scope global (0); los cuerpos de `cuando`/`sino`/`loop` abren un nuevo ámbito.
- **Inferencia de tipos** por nodo de expresión (`num`, `dec`, `text`, `booleano`).
- **Verificaciones:** variable no declarada, doble declaración, tipos incompatibles, operaciones inválidas entre tipos, condición de `cuando`/`loop` debe ser booleana.

```bash
./compilador ejemplos/ejemplo_semantico_error.g5z80
```

```text
[ERROR SEMANTICO] Linea 3: Tipo incompatible: no se puede asignar 'text' a 'num' (variable 'x').
[ERROR SEMANTICO] Linea 4: Variable 'z' no declarada.
[ERROR SEMANTICO] Linea 5: Doble declaracion de la variable 'x' en el mismo ambito.
```

### Fase 4 – Código Intermedio (TAC)
Linealiza el AST en cuádruplas de tres direcciones (`op`, `arg1`, `arg2`, `res`) con temporales `t1..tn` y etiquetas `L1..Ln`. Operaciones: aritméticas, relacionales, lógicas, asignación, saltos condicionales e incondicionales, `imprimir`, `fin`.

### Fase 5 – Generación de Código Z80 (Amstrad CPC)
Traduce las cuádruplas TAC a ensamblador Z80 para el Amstrad CPC:
- Variables y temporales: palabras de 16 bits con signo.
- `imprimir` de texto y de números usa el firmware del CPC (`&BB5A` TXT_OUTPUT).
- Al terminar, el programa queda en un bucle infinito (no hay sistema operativo al que volver); la salida ya quedó impresa en pantalla con `imprimir()`.
- `org &4000`: se carga y ejecuta con `RUN"MAIN` en el emulador.

---

## 🖥️ Interfaz Gráfica Web

Interfaz web para **verificar las cinco fases** de forma gráfica. **No requiere dependencias** (usa el servidor HTTP estándar de Python).

### Ejecutar
```bash
make gui
```
Abre automáticamente **http://localhost:5000** en el navegador.

### Uso
1. Escribe código o elige uno del desplegable **Cargar ejemplo**.
2. Pulsa **Léxico**, **Sintáctico**, **Semántico**, **Intermedio** o **⚡ Analizar Todo**.
3. En el panel derecho, navega entre las pestañas:
   - **Léxico:** tabla de tokens con colores por categoría y conteos.
   - **Sintáctico:** árbol AST visual anidado y colapsable.
   - **Semántico:** tabla de símbolos coloreada por ámbito + diagnósticos.
   - **Intermedio:** tabla de cuádruplas TAC con resaltado por tipo de operación.
   - **Z80:** listado de ensamblador con resaltado de sintaxis, botón para ejecutar en WinAPE y descargar `.dsk`.
4. La **barra de pipeline** (① Léxico → ② Sintáctico → ③ Semántico → ④ Intermedio → ⑤ Z80) resume el estado de cada fase.
5. El botón **🕹️ Ejecutar en WinAPE** compila, ensambla (z80asm.py), empaqueta en un disco (cpcdsk.py) y abre el emulador.

> La vista activa y el ejemplo se pueden enlazar: `…/?archivo=ejemplo_scopes.g5z80#intermedio`.

---

## 🕹️ Ejecución en WinAPE (Emulador Amstrad CPC)

El compilador genera código Z80 que se puede ejecutar en un Amstrad CPC real o emulado.

### Pipeline de ejecución
```
Código Dummy → compilador (5 fases) → Z80 ASM → z80asm.py → binario → cpcdsk.py → .dsk → WinAPE
```

### Herramientas Python
- **`gui/z80asm.py`** — Mini-ensamblador Z80 de dos pasadas. Cubre el subconjunto de instrucciones que emite el generador.
- **`gui/cpcdsk.py`** — Genera imágenes de disco `.dsk` en formato CPCEMU estándar (40 pistas, 9 sectores, cabecera AMSDOS).

### Instalar WinAPE

#### En Windows
```powershell
powershell -ExecutionPolicy Bypass -File tools\descargar_winape.ps1
```
O descárgalo manualmente de [winape.net](http://www.winape.net) y descomprímelo en `tools\winape\`.

#### En Linux (Ubuntu / Debian)
Para poder usar el botón **"Ejecutar en WinAPE"** en Linux, es necesario contar con `wine` y `unzip`.

1. Instala los prerrequisitos:
   ```bash
   sudo apt install wine unzip
   ```
2. Descarga y configura el emulador ejecutando el script automatizado:
   ```bash
   chmod +x tools/descargar_winape.sh
   ./tools/descargar_winape.sh
   ```

El script configurará un wrapper en `tools/winape/WinApe.exe` que traducirá de forma transparente las rutas de archivo de Unix a Wine para que el emulador pueda leer las imágenes de disco `.dsk` temporales sin errores de lectura.

---

## 📦 Instalador de Windows

Se genera un **instalador** (`CompiladorLyC-Setup.exe`) que empaqueta todo lo necesario para que los usuarios solo ejecuten el `Setup.exe`:

- `compilador.exe` y `analizador_lexico.exe` (enlazados estáticamente)
- Python embebido (sin necesidad de instalar Python)
- Interfaz gráfica web (GUI)
- Ensamblador Z80 y generador de discos
- Ejemplos
- WinAPE (emulador)

Al instalar, crea un acceso directo **"Compilador LyC"** que al abrirlo arranca el servidor y abre el navegador automáticamente.

### Generar el instalador localmente
Ver instrucciones detalladas en [`installer/BUILD.md`](installer/BUILD.md).

Resumen rápido:
1. Compila con MSYS2: `make CFLAGS="-Wall -Isrc -static"`
2. Coloca Python embebido en `installer\python\`
3. Ejecuta `installer\construir.bat`
4. El instalador queda en `installer\Output\CompiladorLyC-Setup.exe`

### Generar con GitHub Actions (CI)
El workflow `.github/workflows/windows-installer.yml` automatiza todo el proceso:
- Se lanza **manualmente** desde la pestaña Actions (botón "Run workflow").
- O **automáticamente** al crear un tag de versión (`git tag v1.0.0 && git push --tags`).
- El instalador queda disponible como **artefacto descargable** en la ejecución y como **Release** del repositorio.

---

## 🧹 Limpieza

```bash
make clean
```

O manualmente:
```bash
rm -f src/lex.yy.c src/parser.tab.c src/parser.tab.h compilador analizador_lexico
```

---
