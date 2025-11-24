# Obtener la ruta del directorio donde está el script
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition

# Crear carpeta build si no existe
$BuildDir = Join-Path $ScriptDir "build"
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Activar entorno virtual (si no está activo)
if (-not $env:VIRTUAL_ENV) {
    Write-Host "No se detecta venv activo. Por favor activa tu entorno virtual primero."
    exit 1
}

# Instalar pybind11 si no está presente
try {
    python -c "import pybind11" 2>$null
} catch {
    Write-Host "Instalando pybind11 en el venv..."
    pip install pybind11
}

# Obtener ruta de pybind11 para CMake
$Pybind11CMakeDir = python -m pybind11 --cmakedir
$env:CMAKE_PREFIX_PATH = if ($env:CMAKE_PREFIX_PATH) { "$env:CMAKE_PREFIX_PATH;$Pybind11CMakeDir" } else { $Pybind11CMakeDir }


# Configurar y compilar con CMake
Write-Host "Configurando CMake..."
cmake -B $BuildDir -S $ScriptDir -G Ninja -Dpybind11_DIR=$PybindDir
if ($LASTEXITCODE -ne 0) { throw "Error configurando CMake" }

Write-Host "Compilando proyecto..."
cmake --build $BuildDir
if ($LASTEXITCODE -ne 0) { throw "Error compilando proyecto" }

# Copiar DLLs al build (ajusta la ruta si cambia tu MinGW)
$MinGWDir = "C:\Users\ThinkPad\Downloads\winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64ucrt-13.0.0-r3\mingw64\bin"
$Dlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")

foreach ($dll in $Dlls) {
    $src = Join-Path $MinGWDir $dll
    if (Test-Path $src) {
        Copy-Item $src -Destination $BuildDir -Force
    } else {
        Write-Warning "No se encontró $dll en $MinGWDir"
    }
}

# Añadir build a PYTHONPATH
$env:PYTHONPATH = if ($env:PYTHONPATH) { "$env:PYTHONPATH;$BuildDir" } else { $BuildDir }

# Ejecutar visualization.py
$VisualizationPath = Join-Path $ScriptDir "visualization.py"
if (Test-Path $VisualizationPath) {
    python $VisualizationPath
} else {
    Write-Error "No se encontró visualization.py en $ScriptDir"
}
