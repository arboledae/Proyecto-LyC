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
import http.server
import json
import os
import socket
import subprocess
import tempfile
import threading
import webbrowser

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
            capture_output=True, text=True, timeout=timeout)
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
    sintactico_ok, semantico_estado = False, "na"

    for raw in salida.splitlines():
        ln = raw.rstrip()
        if ln.startswith("@@"):
            seccion = ln[2:].strip()
            continue
        if not ln:
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

        elif seccion == "ESTADO":
            p = ln.split("|", 1)
            if len(p) == 2 and p[0] == "sintactico":
                sintactico_ok = (p[1] == "ok")
            elif len(p) == 2 and p[0] == "semantico":
                semantico_estado = p[1]

    return {
        "tokens": tokens,
        "ast": ast,
        "simbolos": simbolos,
        "errores": errores,
        "sintactico_ok": sintactico_ok,
        "semantico_ok": semantico_estado == "ok",
        "semantico_estado": semantico_estado,   # ok | error | na
        "salida_cruda": salida,
    }


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

    def do_POST(self):
        if self.path != "/analizar":
            self._send(404, "No encontrado", "text/plain")
            return

        length = int(self.headers.get("Content-Length", 0))
        try:
            payload = json.loads(self.rfile.read(length).decode("utf-8"))
            codigo = payload.get("codigo", "")
        except Exception as e:                   # noqa: BLE001
            self._send_json(400, {"error": "JSON invalido: %s" % e})
            return

        fd, tmppath = tempfile.mkstemp(suffix=".g5z80")
        try:
            with os.fdopen(fd, "w") as f:
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


def elegir_puerto(preferido):
    """Devuelve `preferido` si esta libre; si no, pide un puerto al sistema."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.bind(("127.0.0.1", preferido))
            return preferido
        except OSError:
            s.bind(("127.0.0.1", 0))
            return s.getsockname()[1]


def main():
    puerto = elegir_puerto(PORT)
    url = "http://localhost:%d" % puerto
    print("=" * 60)
    print("  Interfaz grafica del Compilador Dummy  -  Grupo 5")
    print("  Abre en el navegador:  %s" % url)
    print("  (cierra esta ventana para detener el servidor)")
    print("=" * 60)
    if not os.path.isfile(COMPILADOR):
        print("[AVISO] No se encontro 'compilador'. Ejecuta 'make' primero.")
    server = http.server.HTTPServer(("127.0.0.1", puerto), Handler)
    # Abrir el navegador predeterminado una vez que el servidor este escuchando.
    threading.Timer(1.0, lambda: webbrowser.open(url)).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nServidor detenido.")


if __name__ == "__main__":
    main()
