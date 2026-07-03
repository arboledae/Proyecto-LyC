# Cómo generar el instalador de Windows (`CompiladorLyC-Setup.exe`)

> **La forma fácil es GitHub Actions**, que hace todo esto automáticamente:
> pestaña *Actions* → *"Instalador de Windows"* → **Run workflow** (o empuja un tag
> `v1.0.0`). El instalador queda como artefacto y como Release descargable.
> Esta guía es solo para generarlo **a mano en tu propia máquina Windows**.

Estos pasos se hacen **una sola vez en tu máquina Windows** para producir el
instalador. Una vez generado, tus compañeros solo ejecutan el `Setup.exe`: no
necesitan instalar nada (Python va embebido). Al abrir el programa instalado se
levanta el servidor local y **se abre solo el navegador predeterminado** con la
interfaz.

---

## 1. Programas necesarios en la máquina de build (solo tú)

| Programa | Para qué | Descarga |
|----------|----------|----------|
| **MSYS2** | Compilar `compilador.exe` (gcc + flex + bison) | https://www.msys2.org/ |
| **Python embebido (64-bit)** | Se empaqueta dentro del instalador | https://www.python.org/downloads/windows/ → *Windows embeddable package (64-bit)* |
| **Inno Setup 6** | Generar el `Setup.exe` | https://jrsoftware.org/isdl.php |

---

## 2. Compilar `compilador.exe` con MSYS2

1. Instala MSYS2 y ábrelo desde el menú Inicio como **"MSYS2 MINGW64"**
   (importante que sea el de *MINGW64*, no el `MSYS` normal).

2. Instala las herramientas (una sola vez):

   ```bash
   pacman -S --needed mingw-w64-x86_64-gcc flex bison make
   ```

3. Ve a la carpeta del proyecto (las unidades se escriben `/c/...`):

   ```bash
   cd "/c/Users/TU_USUARIO/ruta/Proyecto-LyC"
   ```

4. Compila con **enlazado estático** (evita errores de "falta una DLL" en las
   PCs de tus compañeros):

   ```bash
   make clean
   make        CFLAGS="-Wall -Isrc -static"
   make lex    CFLAGS="-Wall -Isrc -static"
   ```

   Esto genera `compilador.exe` y `analizador_lexico.exe` en la raíz del proyecto.

> Comprobación rápida: `./compilador.exe ejemplos/ejemplo1.g5z80` debe mostrar el análisis.

---

## 3. Colocar el Python embebido

1. Descarga el *Windows embeddable package (64-bit)* (un `.zip`, ej. Python 3.12).
2. Descomprime su **contenido** dentro de `installer\python\`, de modo que quede:

   ```
   installer\python\python.exe
   installer\python\python312.dll
   installer\python\... (resto de archivos del zip)
   ```

> No hace falta `pip` ni instalar paquetes: la GUI usa solo la biblioteca estándar.

---

## 4. Generar el instalador

1. Instala **Inno Setup 6**.
2. Haz doble clic en **`installer\construir.bat`**.
   - Verifica que estén `compilador.exe` y `installer\python\python.exe`.
   - Llama a Inno Setup con `installer\windows.iss`.
3. El instalador queda en:

   ```
   installer\Output\CompiladorLyC-Setup.exe
   ```

> Alternativa manual: abre `installer\windows.iss` con Inno Setup y pulsa
> **Build → Compile** (F9).

---

## 5. Distribuir

Envía **`CompiladorLyC-Setup.exe`** a tus compañeros. Ellos:

1. Ejecutan el `Setup.exe` (no pide permisos de administrador; instala por usuario).
2. Queda un acceso directo **"Compilador LyC"** (menú Inicio y, opcionalmente, escritorio).
3. Al abrirlo se inicia el servidor y **se abre solo el navegador** con el programa.
4. Para cerrarlo, cierran la pequeña ventana negra (consola) del servidor.

---

## Resumen de qué hace cada archivo de `installer/`

- **`windows.iss`** — receta de Inno Setup: qué se empaqueta y qué accesos directos se crean.
- **`construir.bat`** — automatiza el paso 4 (verifica requisitos y llama a Inno Setup).
- **`BUILD.md`** — este documento.
- **`python\`** *(lo creas tú)* — Python embebido; no se versiona en git.
- **`Output\`** *(lo genera Inno Setup)* — aquí aparece el `Setup.exe`.
