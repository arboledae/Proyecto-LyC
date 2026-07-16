#!/bin/bash
# ============================================================================
#  Descarga e instala WinAPE (emulador de Amstrad CPC) en tools/winape/
#  y configura un wrapper de Wine para que funcione de manera nativa en Linux.
# ============================================================================

set -e

# Directorios
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DESTINO="$DIR/winape"
ZIP_FILE="/tmp/WinAPE20B2.zip"
URL="https://www.winape.net/download/WinAPE20B2.zip"

echo "=== Configuración de WinAPE para Linux (Ubuntu) ==="

# 1. Verificar si Wine está instalado
if ! command -v wine &> /dev/null; then
    echo "⚠️ [ADVERTENCIA] 'wine' no está instalado."
    echo "Para poder ejecutar WinAPE en Linux, necesitas instalar Wine."
    echo "Puedes instalarlo ejecutando:"
    echo "    sudo apt update && sudo apt install -y wine"
    echo ""
fi

# 2. Verificar unzip
if ! command -v unzip &> /dev/null; then
    echo "Error: 'unzip' no está instalado. Instálalo con 'sudo apt install unzip' e intenta de nuevo."
    exit 1
fi

# 3. Comprobar si ya está instalado
if [ -f "$DESTINO/WinApe_real.exe" ] && [ -f "$DESTINO/WinApe.exe" ]; then
    echo "WinAPE ya está instalado y configurado en $DESTINO"
    exit 0
fi

# 4. Descargar WinAPE
echo "Descargando WinAPE desde $URL ..."
if command -v wget &> /dev/null; then
    wget -O "$ZIP_FILE" "$URL"
elif command -v curl &> /dev/null; then
    curl -L -o "$ZIP_FILE" "$URL"
else
    echo "Error: No se encontró wget ni curl. Instala uno de ellos para descargar."
    exit 1
fi

# 5. Descomprimir
echo "Descomprimiendo en $DESTINO ..."
mkdir -p "$DESTINO"
unzip -o "$ZIP_FILE" -d "$DESTINO"
rm -f "$ZIP_FILE"

# 6. Renombrar el ejecutable original
if [ -f "$DESTINO/WinApe.exe" ]; then
    mv "$DESTINO/WinApe.exe" "$DESTINO/WinApe_real.exe"
elif [ -f "$DESTINO/WinAPE.exe" ]; then
    mv "$DESTINO/WinAPE.exe" "$DESTINO/WinApe_real.exe"
else
    echo "Error: El archivo zip no contenía WinApe.exe"
    exit 1
fi

# 7. Crear el wrapper de Wine
echo "Creando wrapper de Wine en $DESTINO/WinApe.exe ..."
cat << 'EOF' > "$DESTINO/WinApe.exe"
#!/bin/bash
# Wrapper para ejecutar WinAPE a través de Wine en Linux
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Traducir rutas de Unix a Windows para Wine
ARGS=()
for arg in "$@"; do
    if [[ "$arg" == /* ]]; then
        # Es una ruta absoluta de Unix
        if command -v winepath &> /dev/null; then
            ARGS+=("$(winepath -w "$arg")")
        else
            ARGS+=("$arg")
        fi
    else
        ARGS+=("$arg")
    fi
done

wine "$DIR/WinApe_real.exe" "${ARGS[@]}"
EOF

# Dar permisos de ejecución al wrapper
chmod +x "$DESTINO/WinApe.exe"

echo "✅ WinAPE configurado correctamente."
echo "Ahora puedes ejecutar la interfaz con: make gui"
echo "Y hacer clic en 'Ejecutar en WinAPE' para abrirlo automáticamente."
