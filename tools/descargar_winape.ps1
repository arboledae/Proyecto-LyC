# ============================================================================
#  Descarga e instala WinAPE (emulador de Amstrad CPC) en tools\winape\
#
#  Lo usan:
#    - el desarrollador (para el boton "Ejecutar en WinAPE" de la GUI),
#    - installer\construir.bat (para empaquetarlo en el instalador),
#    - el workflow de GitHub Actions.
#
#  Uso:  powershell -ExecutionPolicy Bypass -File tools\descargar_winape.ps1
# ============================================================================
$ErrorActionPreference = "Stop"
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

$destino = Join-Path $PSScriptRoot "winape"
if ((Test-Path (Join-Path $destino "WinApe.exe")) -or
    (Test-Path (Join-Path $destino "WinAPE.exe"))) {
    Write-Host "WinAPE ya esta instalado en $destino"
    exit 0
}

$zip = Join-Path $env:TEMP "WinAPE20B2.zip"
$urls = @(
    "http://www.winape.net/download/WinAPE20B2.zip",
    "https://www.winape.net/download/WinAPE20B2.zip"
)

$descargado = $false
foreach ($url in $urls) {
    try {
        Write-Host "Descargando WinAPE desde $url ..."
        Invoke-WebRequest -Uri $url -OutFile $zip -UseBasicParsing
        $descargado = $true
        break
    } catch {
        Write-Warning "Fallo con ${url}: $_"
    }
}
if (-not $descargado) {
    Write-Error ("No se pudo descargar WinAPE. Descargalo a mano de " +
                 "http://www.winape.net y descomprimelo en tools\winape\")
    exit 1
}

New-Item -ItemType Directory -Force $destino | Out-Null
Expand-Archive -Path $zip -DestinationPath $destino -Force
Remove-Item $zip -ErrorAction SilentlyContinue

if (Test-Path (Join-Path $destino "WinApe.exe")) {
    Write-Host "WinAPE instalado correctamente en $destino"
} else {
    Write-Error "El zip no contenia WinApe.exe"
    exit 1
}
