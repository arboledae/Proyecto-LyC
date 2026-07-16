# Compilar el proyecto en Windows (WSL + Ubuntu)

Guía para compilar el **Compilador Dummy → Z80** en Windows usando **WSL + Ubuntu**,
que es el mismo entorno que usa el grupo (el binario `compilador` del repo es un
ELF de Linux 64-bit).

La cadena de compilación (`gcc`, `bison`, `flex`, `make`) **no existe de forma nativa
en Windows**; por eso usamos Ubuntu dentro de WSL. El código fuente sigue viviendo en
el disco de Windows y se accede desde WSL en `/mnt/c/...`.

---

## Paso 1 — Instalar Ubuntu en WSL

Abre **PowerShell como Administrador** (clic derecho → *Ejecutar como administrador*) y ejecuta:

```powershell
wsl --install -d Ubuntu
```

- Si te pide **reiniciar**, hazlo.
- Al abrir Ubuntu por primera vez, crea tu **usuario y contraseña de Linux**
  (los que quieras). La contraseña **no se muestra** mientras la escribes: es normal.

> Si `wsl` no estuviera disponible, primero habilita la característica con
> `wsl --install` (sin `-d`) y reinicia.

---

## Paso 2 — Instalar la cadena de compilación

Dentro de la terminal de **Ubuntu**:

```bash
sudo apt-get update
sudo apt-get install -y flex bison build-essential python3
```

Esto instala:

| Herramienta        | Para qué sirve                                  |
|--------------------|--------------------------------------------------|
| `flex`             | Genera el analizador léxico (`lex.yy.c`)         |
| `bison`            | Genera el analizador sintáctico (`parser.tab.c/h`) |
| `build-essential`  | `gcc` + `make` + librerías estándar de C          |
| `python3`          | Servidor de la interfaz gráfica (GUI)             |

---

## Paso 3 — Compilar

El proyecto está en el disco de Windows; desde WSL se accede vía `/mnt/c/...`:

```bash
cd "/mnt/c/Users/Steveen/Documents/Lenguaje y compiladores/Proyecto-LyC"
make clean    # borra el binario Linux viejo y los archivos generados
make          # bison + flex + gcc  →  genera ./compilador
```

Objetivos disponibles del `Makefile`:

```bash
make          # Compilador completo (5 fases) → ./compilador
make lex      # Solo el analizador léxico     → ./analizador_lexico
make clean    # Limpia archivos generados
make gui      # Compila y lanza la GUI en http://localhost:5000
```

---

## Paso 4 — Probar / lanzar

```bash
./compilador ejemplos/juego_adivina.g5z80    # ejecuta el compilador sobre un ejemplo
make gui                                      # abre la interfaz en http://localhost:5000
```

Para cerrar la GUI: `Ctrl+C` en la terminal.

---

## Notas

- Los archivos generados por Bison/Flex (`src/lex.yy.c`, `src/parser.tab.c`,
  `src/parser.tab.h`) **no se versionan**: los regenera `make` a partir de
  `parser.y` y `lexer.l`. No los edites a mano.
- Los archivos locales `src/main.c` y `src/parser_rd.c/h` **no** los usa el
  `Makefile` (el `main` está en `parser.y`), así que no afectan a la compilación.
- El binario resultante es de **Linux**: se ejecuta dentro de WSL, no con doble clic
  desde Windows. Para un ejecutable de Windows sin instalar nada, usa el instalador
  `CompiladorLyC-Setup.exe` de la pestaña *Releases* / *Actions* del repo.

---

## Alternativa: MSYS2 + MinGW-w64 (compilación nativa de Windows)

Si en lugar de WSL prefieres un `.exe` nativo de Windows:

```powershell
winget install MSYS2.MSYS2
```

Luego, en la shell **UCRT64** de MSYS2:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc bison flex make
cd "/c/Users/Steveen/Documents/Lenguaje y compiladores/Proyecto-LyC"
make
```
