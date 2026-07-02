#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Mini-ensamblador Z80 para el Compilador Dummy (Grupo 5).

Ensambla el listado que produce el generador de codigo del compilador
(src/generador_z80.c) y devuelve el binario listo para empaquetarse en
una imagen .dsk de Amstrad CPC (ver cpcdsk.py) y ejecutarse en WinAPE.

Cubre el subconjunto de instrucciones que emite el generador (cargas,
aritmetica de 16 bits, saltos, llamadas, rotaciones, bit, defb/defw...).
Es de dos pasadas: la primera calcula las direcciones de las etiquetas
y la segunda emite los bytes definitivos.

Uso directo (depuracion):   python z80asm.py programa.asm salida.bin
"""

R8    = {"b": 0, "c": 1, "d": 2, "e": 3, "h": 4, "l": 5, "(hl)": 6, "a": 7}
R16   = {"bc": 0, "de": 1, "hl": 2, "sp": 3}    # pares con SP
R16PP = {"bc": 0, "de": 1, "hl": 2, "af": 3}    # pares para push/pop
CC    = {"nz": 0, "z": 1, "nc": 2, "c": 3, "po": 4, "pe": 5, "p": 6, "m": 7}
ROT   = {"rlc": 0x00, "rrc": 0x08, "rl": 0x10, "rr": 0x18,
         "sla": 0x20, "sra": 0x28, "srl": 0x38}
SIMPLES = {"nop": 0x00, "rlca": 0x07, "rrca": 0x0F, "rla": 0x17, "rra": 0x1F,
           "cpl": 0x2F, "scf": 0x37, "ccf": 0x3F, "halt": 0x76,
           "ex de,hl": 0xEB, "di": 0xF3, "ei": 0xFB, "ret": 0xC9, "exx": 0xD9}


class ErrorAsm(Exception):
    pass


def _es_mem(op):
    """(algo) pero no (hl): referencia a memoria por direccion."""
    return op.startswith("(") and op.endswith(")") and op != "(hl)"


class Ensamblador:
    def __init__(self):
        self.simbolos = {}
        self.org = 0x4000

    # ── utilidades ──────────────────────────────────────────────
    def valor(self, txt, pase, linea):
        """Numero (&hex, 0x, decimal) o etiqueta."""
        t = txt.strip()
        neg = t.startswith("-")
        if neg:
            t = t[1:].strip()
        try:
            if t.startswith("&") or t.startswith("#"):
                v = int(t[1:], 16)
            elif t.lower().startswith("0x"):
                v = int(t, 16)
            elif t.isdigit():
                v = int(t)
            elif pase == 1:
                v = 0                        # etiqueta aun no resuelta
            else:
                v = self.simbolos[t]
        except (ValueError, KeyError):
            raise ErrorAsm("linea %d: no se puede resolver '%s'" % (linea, txt))
        return -v if neg else v

    # ── una instruccion -> lista de bytes ───────────────────────
    def instr(self, mnem, ops, direccion, pase, linea):
        m = mnem.lower()
        # o  = operandos originales (para etiquetas, sensibles a mayusculas)
        # ol = en minusculas (para reconocer registros y condiciones)
        o  = [x.strip() for x in ops] if ops else []
        ol = [x.lower() for x in o]

        def n8(t):
            return self.valor(t, pase, linea) & 0xFF

        def n16(t):
            v = self.valor(t, pase, linea) & 0xFFFF
            return [v & 0xFF, v >> 8]

        def rel(t):
            if pase == 1:
                return 0
            destino = self.valor(t, pase, linea)
            d = destino - (direccion + 2)
            if d < -128 or d > 127:
                raise ErrorAsm("linea %d: salto relativo fuera de rango (%d)" % (linea, d))
            return d & 0xFF

        entera = (m + " " + ",".join(ol)).strip()

        if entera in SIMPLES:
            return [SIMPLES[entera]]
        if m in SIMPLES and not o:
            return [SIMPLES[m]]

        if m == "ld":
            d, f = ol[0], ol[1]
            dr, fr = o[0], o[1]
            # ld r,r'
            if d in R8 and f in R8:
                return [0x40 | (R8[d] << 3) | R8[f]]
            # ld (hl),n
            if d == "(hl)" and f not in R8 and not _es_mem(f):
                return [0x36, n8(fr)]
            # ld r,n
            if d in R8 and not _es_mem(f) and f not in R16:
                return [0x06 | (R8[d] << 3), n8(fr)]
            # ld a,(nn) / ld (nn),a
            if d == "a" and _es_mem(f):
                return [0x3A] + n16(fr[1:-1])
            if _es_mem(d) and f == "a":
                return [0x32] + n16(dr[1:-1])
            # ld hl,(nn) / ld (nn),hl
            if d == "hl" and _es_mem(f):
                return [0x2A] + n16(fr[1:-1])
            if _es_mem(d) and f == "hl":
                return [0x22] + n16(dr[1:-1])
            # ld de/bc/sp,(nn) y ld (nn),de/bc/sp  (prefijo ED)
            if d in ("bc", "de", "sp") and _es_mem(f):
                return [0xED, 0x4B | (R16[d] << 4)] + n16(fr[1:-1])
            if _es_mem(d) and f in ("bc", "de", "sp"):
                return [0xED, 0x43 | (R16[f] << 4)] + n16(dr[1:-1])
            # ld rr,nn
            if d in R16:
                return [0x01 | (R16[d] << 4)] + n16(fr)
            raise ErrorAsm("linea %d: forma de ld no soportada: %s" % (linea, entera))

        if m == "add":
            if ol[0] == "hl" and ol[1] in R16:
                return [0x09 | (R16[ol[1]] << 4)]
            if ol[0] == "a" and ol[1] in R8:
                return [0x80 | R8[ol[1]]]
            if ol[0] == "a":
                return [0xC6, n8(o[1])]
        if m == "adc":
            if ol[0] == "hl" and ol[1] in R16:
                return [0xED, 0x4A | (R16[ol[1]] << 4)]
            if ol[0] == "a" and ol[1] in R8:
                return [0x88 | R8[ol[1]]]
            if ol[0] == "a":
                return [0xCE, n8(o[1])]
        if m == "sbc":
            if ol[0] == "hl" and ol[1] in R16:
                return [0xED, 0x42 | (R16[ol[1]] << 4)]
            if ol[0] == "a" and ol[1] in R8:
                return [0x98 | R8[ol[1]]]
            if ol[0] == "a":
                return [0xDE, n8(o[1])]
        if m == "sub":
            if ol[0] in R8:
                return [0x90 | R8[ol[0]]]
            return [0xD6, n8(o[0])]

        if m in ("and", "or", "xor", "cp"):
            base = {"and": 0xA0, "xor": 0xA8, "or": 0xB0, "cp": 0xB8}[m]
            imm  = {"and": 0xE6, "xor": 0xEE, "or": 0xF6, "cp": 0xFE}[m]
            if ol[0] in R8:
                return [base | R8[ol[0]]]
            return [imm, n8(o[0])]

        if m == "inc":
            if ol[0] in R16:
                return [0x03 | (R16[ol[0]] << 4)]
            if ol[0] in R8:
                return [0x04 | (R8[ol[0]] << 3)]
        if m == "dec":
            if ol[0] in R16:
                return [0x0B | (R16[ol[0]] << 4)]
            if ol[0] in R8:
                return [0x05 | (R8[ol[0]] << 3)]

        if m == "push" and ol[0] in R16PP:
            return [0xC5 | (R16PP[ol[0]] << 4)]
        if m == "pop" and ol[0] in R16PP:
            return [0xC1 | (R16PP[ol[0]] << 4)]

        if m == "jp":
            if len(o) == 1:
                return [0xC3] + n16(o[0])
            return [0xC2 | (CC[ol[0]] << 3)] + n16(o[1])
        if m == "jr":
            if len(o) == 1:
                return [0x18, rel(o[0])]
            if ol[0] not in ("nz", "z", "nc", "c"):
                raise ErrorAsm("linea %d: jr solo admite nz/z/nc/c" % linea)
            return [0x20 | (CC[ol[0]] << 3), rel(o[1])]
        if m == "djnz":
            return [0x10, rel(o[0])]
        if m == "call":
            if len(o) == 1:
                return [0xCD] + n16(o[0])
            return [0xC4 | (CC[ol[0]] << 3)] + n16(o[1])
        if m == "ret" and o:
            return [0xC0 | (CC[ol[0]] << 3)]

        if m in ROT and ol[0] in R8:
            return [0xCB, ROT[m] | R8[ol[0]]]
        if m in ("bit", "set", "res"):
            base = {"bit": 0x40, "res": 0x80, "set": 0xC0}[m]
            b = self.valor(o[0], pase, linea)
            return [0xCB, base | (b << 3) | R8[ol[1]]]

        raise ErrorAsm("linea %d: instruccion no soportada: %s" % (linea, entera))

    # ── ensamblado completo (dos pasadas) ───────────────────────
    def ensamblar(self, texto):
        lineas = texto.splitlines()
        binario = bytearray()

        for pase in (1, 2):
            direccion = self.org
            binario = bytearray()
            for num, cruda in enumerate(lineas, 1):
                ln = cruda.split(";", 1)[0].strip()
                if not ln:
                    continue

                # etiqueta(s) al inicio de la linea
                while True:
                    partes = ln.split(None, 1)
                    if partes and partes[0].endswith(":"):
                        nombre = partes[0][:-1]
                        if pase == 1:
                            if nombre in self.simbolos:
                                raise ErrorAsm("linea %d: etiqueta duplicada '%s'" % (num, nombre))
                            self.simbolos[nombre] = direccion
                        ln = partes[1] if len(partes) > 1 else ""
                        if not ln:
                            break
                    else:
                        break
                if not ln:
                    continue

                partes = ln.split(None, 1)
                mnem = partes[0].lower()
                resto = partes[1] if len(partes) > 1 else ""

                if mnem == "org":
                    self.org = self.valor(resto, pase, num)
                    direccion = self.org
                    continue

                if mnem in ("defb", "db"):
                    datos = [self.valor(x, pase, num) & 0xFF
                             for x in resto.split(",") if x.strip() != ""]
                    binario.extend(datos)
                    direccion += len(datos)
                    continue

                if mnem in ("defw", "dw"):
                    for x in resto.split(","):
                        if x.strip() == "":
                            continue
                        v = self.valor(x, pase, num) & 0xFFFF
                        binario.extend([v & 0xFF, v >> 8])
                        direccion += 2
                    continue

                if mnem in ("defs", "ds"):
                    cuantos = self.valor(resto, pase, num)
                    binario.extend([0] * cuantos)
                    direccion += cuantos
                    continue

                ops = [x for x in resto.split(",")] if resto else []
                bs = self.instr(mnem, ops, direccion, pase, num)
                binario.extend(bs)
                direccion += len(bs)

        return bytes(binario)


def ensamblar(texto):
    """Ensambla el listado y devuelve (binario, direccion_de_carga)."""
    a = Ensamblador()
    binario = a.ensamblar(texto)
    return binario, a.org


if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Uso: python z80asm.py entrada.asm salida.bin")
        sys.exit(1)
    with open(sys.argv[1], encoding="utf-8") as f:
        texto = f.read()
    binario, org = ensamblar(texto)
    with open(sys.argv[2], "wb") as f:
        f.write(binario)
    print("OK: %d bytes, org=&%04X" % (len(binario), org))
