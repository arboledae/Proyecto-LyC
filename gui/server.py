#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Servidor web para la interfaz grafica del compilador Dummy (Grupo 5).

NO usa Flask ni dependencias externas: solo la biblioteca estandar de Python
(http.server + json + subprocess). Cero instalacion.

El compilador C se invoca en modo grafico:

    ./compilador --gui archivo.g5z80

y devuelve un flujo delimitado (sin JSON en C) con las secciones
@@TOKENS / @@AST / @@SIMBOLOS / @@ERRORES / @@ESTADO / @@END, que aqui se
traduce a JSON para el frontend. Asi se cubren las tres fases de analisis
(lexico, sintactico y semantico) en una sola pasada del compilador.

Ejecutar desde la raiz del proyecto:
    make            # compila el compilador (y el analizador lexico)
    make gui        # lanza este servidor
    # abrir http://localhost:5000
"""
import glob
import http.server
import json
import os
import socket
import subprocess
import sys
import tempfile
import threading
import time
import webbrowser

# El Python embebido del instalador no agrega la carpeta del script a
# sys.path (usa python._pth), asi que se agrega a mano para poder
# importar z80asm y cpcdsk.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import z80asm     # mini-ensamblador Z80        (gui/z80asm.py)
import cpcdsk     # generador de imagenes .dsk  (gui/cpcdsk.py)

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMPILADOR   = os.path.join(PROJECT_ROOT, "compilador")
# En Windows el binario es compilador.exe; lo detectamos sin romper Linux.
if os.name == "nt" or os.path.isfile(COMPILADOR + ".exe"):
    COMPILADOR += ".exe"
EJEMPLOS_DIR = os.path.join(PROJECT_ROOT, "ejemplos")
STATIC_DIR   = os.path.join(os.path.dirname(os.path.abspath(__file__)), "static")
PORT         = 5000

# Ejemplos ofrecidos en el desplegable (archivo -> etiqueta legible).
EJEMPLOS = [
    ("spacewar.g5z80",                "\U0001F680 Juego: Spacewar (jugable)"),
    ("juego_adivina.g5z80",           "\U0001F3AE Juego: Adivina el Numero"),
    ("ejemplo1.g5z80",                "Programa valido"),
    ("ejemplo_scopes.g5z80",          "Ambitos anidados (scopes)"),
    ("ejemplo_semantico_error.g5z80", "Errores semanticos"),
    ("ejemplo_con_error.g5z80",       "Error sintactico"),
]

EJEMPLO_POR_DEFECTO = '''inicio {
  num x = 10;
  dec promedio = 9.5;
  cuando (x > 5) {
    imprimir("Mayor que 5");
  } sino {
    imprimir("Menor o igual");
  }
  loop (x > 0) {
    x = x - 1;
  }
}
'''


def ejecutar_compilador(ruta_archivo, timeout=10):
    """Ejecuta `compilador --gui archivo` y devuelve (salida, error_str)."""
    try:
        proc = subprocess.run(
            [COMPILADOR, "--gui", ruta_archivo], cwd=PROJECT_ROOT,
            capture_output=True, text=True, timeout=timeout,
            encoding="utf-8", errors="replace")
        return (proc.stdout or "") + (proc.stderr or ""), None
    except FileNotFoundError:
        return "", "No se encontro el binario 'compilador'. Ejecuta 'make' primero."
    except subprocess.TimeoutExpired:
        return "", "Tiempo de ejecucion agotado."
    except Exception as e:                       # noqa: BLE001
        return "", "No se pudo ejecutar el compilador: %s" % e


def _int(s, defecto=0):
    try:
        return int(s)
    except (TypeError, ValueError):
        return defecto


def parsear_salida_gui(salida):
    """Convierte el flujo delimitado @@... del compilador en un dict JSON."""
    seccion = None
    tokens, ast, simbolos, errores = [], [], [], []
    intermedio, z80 = [], []
    sintactico_ok, semantico_estado = False, "na"
    intermedio_estado, z80_estado = "na", "na"

    for raw in salida.splitlines():
        ln = raw.rstrip()
        if ln.startswith("@@"):
            seccion = ln[2:].strip()
            continue
        if not ln and seccion != "Z80":
            continue

        if seccion == "TOKENS":
            p = ln.split("|", 2)
            if len(p) == 3:
                tokens.append({"linea": _int(p[0]), "token": p[1], "lexema": p[2]})

        elif seccion == "AST":
            p = ln.split("|", 1)
            if len(p) == 2:
                ast.append({"depth": _int(p[0]), "label": p[1]})

        elif seccion == "SIMBOLOS":
            p = ln.split("|")
            if len(p) >= 5:
                simbolos.append({"nombre": p[0], "tipo": p[1], "scope": p[2],
                                 "linea": p[3], "init": p[4]})

        elif seccion == "ERRORES":
            p = ln.split("|", 2)
            if len(p) == 3:
                errores.append({"tipo": p[0], "linea": _int(p[1]), "mensaje": p[2]})

        elif seccion == "INTERMEDIO":
            # n|op|arg1|arg2|res|texto  (texto = version legible)
            p = ln.split("|", 5)
            if len(p) == 6:
                intermedio.append({"n": _int(p[0]), "op": p[1], "arg1": p[2],
                                   "arg2": p[3], "res": p[4], "texto": p[5]})

        elif seccion == "Z80":
            z80.append(raw.rstrip("\r"))

        elif seccion == "ESTADO":
            p = ln.split("|", 1)
            if len(p) == 2 and p[0] == "sintactico":
                sintactico_ok = (p[1] == "ok")
            elif len(p) == 2 and p[0] == "semantico":
                semantico_estado = p[1]
            elif len(p) == 2 and p[0] == "intermedio":
                intermedio_estado = p[1]
            elif len(p) == 2 and p[0] == "z80":
                z80_estado = p[1]

    # quitar lineas vacias del final del listado Z80
    while z80 and not z80[-1].strip():
        z80.pop()

    return {
        "tokens": tokens,
        "ast": ast,
        "simbolos": simbolos,
        "errores": errores,
        "intermedio": intermedio,
        "z80": z80,
        "sintactico_ok": sintactico_ok,
        "semantico_ok": semantico_estado == "ok",
        "semantico_estado": semantico_estado,     # ok | error | na
        "intermedio_estado": intermedio_estado,   # ok | na
        "z80_estado": z80_estado,                 # ok | na
        "winape_disponible": buscar_winape() is not None,
        "salida_cruda": salida,
    }


# ── Ejecucion en WinAPE (emulador de Amstrad CPC) ────────────────
# El codigo Z80 del compilador se ensambla (z80asm), se empaqueta en
# una imagen de disco (cpcdsk) y se abre WinAPE con autoarranque:
#     WinApe.exe <imagen.dsk> /A:MAIN
proceso_winape = None     # ultima instancia lanzada por este servidor


def buscar_winape():
    """Devuelve la ruta de WinApe.exe o None si no esta instalado."""
    candidatos = []
    env = os.environ.get("WINAPE")
    if env:
        candidatos += [env, os.path.join(env, "WinApe.exe")]
    for base in (os.path.join(PROJECT_ROOT, "tools", "winape"),
                 os.path.join(PROJECT_ROOT, "winape")):
        candidatos += [os.path.join(base, "WinApe.exe"),
                       os.path.join(base, "WinAPE.exe")]
    for c in candidatos:
        if c and os.path.isfile(c):
            return c
    return None


def compilar_a_dsk(codigo):
    """Compila codigo Dummy hasta una imagen .dsk.
    Devuelve (dsk_bytes, resultado_analisis, mensaje_error)."""
    fd, tmppath = tempfile.mkstemp(suffix=".g5z80")
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as f:
            f.write(codigo)
        salida, error = ejecutar_compilador(tmppath)
    finally:
        try:
            os.remove(tmppath)
        except OSError:
            pass
    if error:
        return None, None, error

    res = parsear_salida_gui(salida)
    if not res["sintactico_ok"] or res["semantico_estado"] != "ok":
        return None, res, ("El programa tiene errores (revisa los "
                           "diagnosticos): no se genero codigo Z80.")
    if not res["z80"]:
        return None, res, "El compilador no emitio codigo Z80."

    try:
        binario, org = z80asm.ensamblar("\n".join(res["z80"]))
    except z80asm.ErrorAsm as e:
        return None, res, "Error al ensamblar el Z80: %s" % e
    dsk = cpcdsk.crear_dsk(binario, nombre="MAIN", ext="BIN",
                           carga=org, ejecucion=org)
    return dsk, res, None


def lanzar_winape(dsk_bytes):
    """Escribe la imagen .dsk y abre WinAPE con autoarranque RUN"MAIN."""
    global proceso_winape

    ruta_winape = buscar_winape()
    if not ruta_winape:
        return (None, "No se encontro WinApe.exe. Ejecuta "
                      "tools\\descargar_winape.ps1 (o define la variable "
                      "de entorno WINAPE con la ruta del emulador).")

    # una imagen nueva por ejecucion (WinAPE mantiene abierta la anterior)
    carpeta = tempfile.gettempdir()
    for viejo in glob.glob(os.path.join(carpeta, "dummy_g5_*.dsk")):
        try:
            os.remove(viejo)
        except OSError:
            pass                      # en uso por otra instancia
    ruta_dsk = os.path.join(carpeta, "dummy_g5_%d.dsk" % int(time.time() * 1000))
    with open(ruta_dsk, "wb") as f:
        f.write(dsk_bytes)

    # cerrar la instancia anterior lanzada por este servidor
    if proceso_winape and proceso_winape.poll() is None:
        try:
            proceso_winape.terminate()
        except OSError:
            pass

    proceso_winape = subprocess.Popen(
        [ruta_winape, ruta_dsk, "/A:MAIN"],
        cwd=os.path.dirname(ruta_winape))
    return ruta_dsk, None


def leer_ejemplo(archivo):
    """Lee un ejemplo del directorio ejemplos/ de forma segura."""
    nombre = os.path.basename(archivo or "")
    ruta = os.path.join(EJEMPLOS_DIR, nombre)
    if nombre and os.path.isfile(ruta):
        with open(ruta, encoding="utf-8") as f:
            return f.read()
    return EJEMPLO_POR_DEFECTO


class Handler(http.server.BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def _send(self, code, data, ctype):
        if isinstance(data, str):
            data = data.encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", ctype + "; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def _send_json(self, code, obj):
        self._send(code, json.dumps(obj, ensure_ascii=False), "application/json")

    def do_GET(self):
        ruta = self.path.split("?", 1)[0]

        if ruta in ("/", "/index.html"):
            try:
                html = open(os.path.join(STATIC_DIR, "index.html"), encoding="utf-8").read()
            except FileNotFoundError:
                html = "<h1>Falta static/index.html</h1>"
            self._send(200, html, "text/html")
            return

        if ruta == "/ejemplos":
            self._send_json(200, [{"archivo": a, "nombre": n} for a, n in EJEMPLOS])
            return

        if ruta == "/ejemplo":
            qs = self.path.split("?", 1)[1] if "?" in self.path else ""
            archivo = ""
            for par in qs.split("&"):
                if par.startswith("archivo="):
                    archivo = par[len("archivo="):]
            self._send(200, leer_ejemplo(archivo), "text/plain")
            return

        rel = ruta.lstrip("/")
        destino = os.path.join(STATIC_DIR, rel)
        if os.path.isfile(destino) and os.path.abspath(destino).startswith(STATIC_DIR):
            ext = os.path.splitext(destino)[1].lower()
            ct = {".js": "application/javascript", ".css": "text/css"}.get(
                ext, "application/octet-stream")
            with open(destino, "rb") as f:
                self._send(200, f.read(), ct)
        else:
            self._send(404, "No encontrado", "text/plain")

    def _leer_codigo(self):
        """Lee el JSON {codigo: ...} del cuerpo de la peticion."""
        length = int(self.headers.get("Content-Length", 0))
        payload = json.loads(self.rfile.read(length).decode("utf-8"))
        return payload.get("codigo", "")

    def do_POST(self):
        try:
            codigo = self._leer_codigo()
        except Exception as e:                   # noqa: BLE001
            self._send_json(400, {"error": "JSON invalido: %s" % e})
            return

        # ── Analisis (lexico + sintactico + semantico + TAC + Z80) ──
        if self.path == "/analizar":
            fd, tmppath = tempfile.mkstemp(suffix=".g5z80")
            try:
                with os.fdopen(fd, "w", encoding="utf-8") as f:
                    f.write(codigo)
                salida, error = ejecutar_compilador(tmppath)
                if error:
                    self._send_json(200, {"error": error})
                    return
                self._send_json(200, parsear_salida_gui(salida))
            finally:
                try:
                    os.remove(tmppath)
                except OSError:
                    pass
            return

        # ── Compilar y ejecutar en WinAPE ──
        if self.path == "/ejecutar":
            dsk, res, error = compilar_a_dsk(codigo)
            if error:
                self._send_json(200, {"ok": False, "error": error,
                                      "resultado": res})
                return
            ruta_dsk, error = lanzar_winape(dsk)
            if error:
                self._send_json(200, {"ok": False, "error": error,
                                      "resultado": res})
                return
            self._send_json(200, {
                "ok": True,
                "mensaje": "WinAPE abierto: cargando RUN\"MAIN (%d bytes)" % len(dsk),
                "dsk": os.path.basename(ruta_dsk),
                "resultado": res,
            })
            return

        # ── Descargar la imagen .dsk generada ──
        if self.path == "/dsk":
            dsk, res, error = compilar_a_dsk(codigo)
            if error:
                self._send_json(200, {"ok": False, "error": error})
                return
            self.send_response(200)
            self.send_header("Content-Type", "application/octet-stream")
            self.send_header("Content-Disposition",
                             'attachment; filename="programa.dsk"')
            self.send_header("Content-Length", str(len(dsk)))
            self.end_headers()
            self.wfile.write(dsk)
            return

        self._send(404, "No encontrado", "text/plain")


def hay_servidor_escuchando(puerto):
    """True si algo YA esta escuchando en 127.0.0.1:puerto.

    Se comprueba con una conexion real (connect) y no intentando bind:
    en Windows http.server activa SO_REUSEADDR, asi que un bind sobre un
    puerto ya ocupado por otro servidor tiene exito de todos modos (dos
    procesos escuchando el mismo puerto), lo que haria inutil detectar el
    conflicto con try/except. connect() si distingue de forma fiable."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(0.3)
        return s.connect_ex(("127.0.0.1", puerto)) == 0


def crear_servidor(preferido, intentos=10):
    """Levanta el HTTPServer en `preferido`; si ya hay otro servidor ahi
    prueba preferido+1, +2, ... (puertos predecibles, no aleatorios) para
    que el usuario pueda encontrarlo facilmente. Devuelve (servidor,
    puerto, conflicto) donde `conflicto` indica que el puerto preferido ya
    estaba en uso (normalmente por OTRA instancia del compilador abierta)."""
    for i in range(intentos):
        puerto = preferido + i
        if hay_servidor_escuchando(puerto):
            continue                      # ya hay un servidor ahi: saltar
        try:
            # ThreadingHTTPServer (un hilo por peticion): el HTTPServer
            # normal es de un solo hilo, asi que un cliente lento o una
            # pestaña del navegador que deja una conexion a medias bloquea
            # TODAS las demas peticiones. La GUI dispara varias a la vez y
            # ademas lanzar WinAPE no debe congelar los analisis.
            servidor = http.server.ThreadingHTTPServer(("127.0.0.1", puerto), Handler)
            return servidor, puerto, (i > 0)
        except OSError:
            continue
    raise SystemExit("No se pudo abrir ningun puerto entre %d y %d."
                     % (preferido, preferido + intentos - 1))


def main():
    server, puerto, conflicto = crear_servidor(PORT)
    url = "http://localhost:%d" % puerto
    print("=" * 60)
    print("  Interfaz grafica del Compilador Dummy  -  Grupo 5")
    if conflicto:
        # El puerto por defecto estaba ocupado: casi siempre es otra copia
        # (p.ej. la version instalada en 'C:\\Program Files\\Compilador LyC')
        # que sigue abierta. Avisamos fuerte para que el usuario no confunda
        # este servidor con el otro al escribir localhost:5000 de memoria.
        print("  [AVISO] El puerto %d ya estaba ocupado por OTRA instancia." % PORT)
        print("          Puede ser una copia instalada o una ventana anterior.")
        print("          Este servidor esta en el puerto %d." % puerto)
    print("  Abre en el navegador:  %s" % url)
    print("  (cierra esta ventana para detener el servidor)")
    print("=" * 60)
    if not os.path.isfile(COMPILADOR):
        print("[AVISO] No se encontro 'compilador'. Ejecuta 'make' primero.")
    # Abrir el navegador predeterminado una vez que el servidor este escuchando.
    threading.Timer(1.0, lambda: webbrowser.open(url)).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nServidor detenido.")


if __name__ == "__main__":
    main()
