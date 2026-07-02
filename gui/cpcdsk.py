#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Generador de imagenes de disco Amstrad CPC (.dsk) para el Compilador
Dummy (Grupo 5).

Crea una imagen en el formato estandar CPCEMU (el que usan WinAPE y el
resto de emuladores) con un unico archivo binario con cabecera AMSDOS,
listo para cargarse con  RUN"MAIN  dentro del emulador.

Geometria usada (formato DATA):
  - 40 pistas x 1 cara x 9 sectores de 512 bytes (ids &C1..&C9)
  - directorio en los 4 primeros sectores de la pista 0 (bloques 0-1)
  - los archivos ocupan bloques de 1 KB a partir del bloque 2
"""

PISTAS       = 40
SECTORES     = 9
TAM_SECTOR   = 512
PRIMER_ID    = 0xC1
TAM_BLOQUE   = 1024                       # 2 sectores por bloque
RELLENO      = 0xE5


def _cabecera_amsdos(nombre, ext, binario, carga, ejecucion):
    """Cabecera AMSDOS de 128 bytes para un archivo binario."""
    h = bytearray(128)
    h[0] = 0                                        # numero de usuario
    h[1:9]   = nombre.upper().ljust(8)[:8].encode("ascii")
    h[9:12]  = ext.upper().ljust(3)[:3].encode("ascii")
    h[0x12] = 2                                     # tipo 2 = binario
    h[0x15] = carga & 0xFF
    h[0x16] = (carga >> 8) & 0xFF
    h[0x18] = len(binario) & 0xFF
    h[0x19] = (len(binario) >> 8) & 0xFF
    h[0x1A] = ejecucion & 0xFF
    h[0x1B] = (ejecucion >> 8) & 0xFF
    h[0x40] = len(binario) & 0xFF                   # longitud real (24 bits)
    h[0x41] = (len(binario) >> 8) & 0xFF
    h[0x42] = 0
    suma = sum(h[0:0x43]) & 0xFFFF                  # checksum de la cabecera
    h[0x43] = suma & 0xFF
    h[0x44] = (suma >> 8) & 0xFF
    return bytes(h)


def crear_dsk(binario, nombre="MAIN", ext="BIN", carga=0x4000, ejecucion=0x4000):
    """Devuelve los bytes de una imagen .dsk con el archivo indicado."""
    datos = _cabecera_amsdos(nombre, ext, binario, carga, ejecucion) + bytes(binario)

    # ── Sectores logicos del disco (LSN 0 .. 359) ──
    n_lsn = PISTAS * SECTORES
    sector = [bytearray([RELLENO] * TAM_SECTOR) for _ in range(n_lsn)]

    # ── Colocar el archivo en bloques de 1 KB desde el bloque 2 ──
    registros = (len(datos) + 127) // 128           # registros CP/M de 128 bytes
    n_bloques = (len(datos) + TAM_BLOQUE - 1) // TAM_BLOQUE
    bloques = list(range(2, 2 + n_bloques))
    if bloques and bloques[-1] >= (n_lsn // 2):
        raise ValueError("El programa no cabe en el disco")

    for i, blq in enumerate(bloques):
        trozo = datos[i * TAM_BLOQUE:(i + 1) * TAM_BLOQUE]
        lsn = blq * 2                               # 2 sectores por bloque
        sector[lsn][0:len(trozo[:TAM_SECTOR])] = trozo[:TAM_SECTOR]
        if len(trozo) > TAM_SECTOR:
            resto = trozo[TAM_SECTOR:]
            sector[lsn + 1][0:len(resto)] = resto

    # ── Directorio CP/M: entradas de 32 bytes en LSN 0..3 ──
    # Cada entrada (extent) referencia hasta 16 bloques (16 KB).
    entradas = []
    nombre83 = nombre.upper().ljust(8)[:8] + ext.upper().ljust(3)[:3]
    quedan = registros
    for extent in range((n_bloques + 15) // 16 or 1):
        e = bytearray(32)
        e[0] = 0                                    # usuario 0
        e[1:12] = nombre83.encode("ascii")
        e[12] = extent                              # numero de extent
        e[15] = min(quedan, 128)                    # registros en este extent
        quedan -= e[15]
        for j, blq in enumerate(bloques[extent * 16:(extent + 1) * 16]):
            e[16 + j] = blq
        entradas.append(bytes(e))

    directorio = b"".join(entradas)
    directorio += bytes([RELLENO]) * (4 * TAM_SECTOR - len(directorio))
    for i in range(4):
        sector[i][:] = directorio[i * TAM_SECTOR:(i + 1) * TAM_SECTOR]

    # ── Contenedor .dsk (CPCEMU estandar) ──
    tam_pista = 0x100 + SECTORES * TAM_SECTOR       # Track-Info + datos

    disco = bytearray()
    cab = bytearray(256)
    cab[0:34]  = b"MV - CPCEMU Disk-File\r\nDisk-Info\r\n"
    cab[34:48] = b"Compilador-G5 "                  # creador (14 bytes)
    cab[48] = PISTAS
    cab[49] = 1                                     # caras
    cab[50] = tam_pista & 0xFF
    cab[51] = (tam_pista >> 8) & 0xFF
    disco += cab

    for pista in range(PISTAS):
        ti = bytearray(256)
        ti[0:12] = b"Track-Info\r\n"
        ti[16] = pista                              # numero de pista
        ti[17] = 0                                  # cara
        ti[20] = 2                                  # tamano de sector (2 = 512)
        ti[21] = SECTORES
        ti[22] = 0x4E                               # GAP#3
        ti[23] = RELLENO                            # byte de relleno
        for s in range(SECTORES):
            base = 24 + s * 8
            ti[base + 0] = pista
            ti[base + 1] = 0
            ti[base + 2] = PRIMER_ID + s            # id del sector (&C1..)
            ti[base + 3] = 2
            ti[base + 4] = 0                        # FDC ST1
            ti[base + 5] = 0                        # FDC ST2
            ti[base + 6] = TAM_SECTOR & 0xFF
            ti[base + 7] = (TAM_SECTOR >> 8) & 0xFF
        disco += ti
        for s in range(SECTORES):
            disco += sector[pista * SECTORES + s]

    return bytes(disco)


if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Uso: python cpcdsk.py programa.bin salida.dsk")
        sys.exit(1)
    with open(sys.argv[1], "rb") as f:
        binario = f.read()
    dsk = crear_dsk(binario)
    with open(sys.argv[2], "wb") as f:
        f.write(dsk)
    print("OK: %s (%d bytes)" % (sys.argv[2], len(dsk)))
