$ErrorActionPreference = "Stop"
$vswhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"

Write-Host "Locating Visual Studio's CMake..."
$cmake = & $vswhere -latest -products * -find Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe

if (-not $cmake) {
    Write-Host "CMake not found!"
    exit 1
}

Write-Host "Found CMake at: $cmake"
Write-Host "Running CMake configure for SDK 2025..."
& $cmake -B sdk_2025/build -S sdk_2025 -G "Visual Studio 17 2022" -A x64

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "Running MSBuild (CMake build)..."
& $cmake --build sdk_2025/build --config Release

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$xdl64Path = "sdk_2025\build\bin\Release\plugins\C4D_DollyZoom\C4D_DollyZoom.xdl64"
if (Test-Path $xdl64Path) {
    Write-Host "Plugin build successful: $xdl64Path"
    $pdbPath = "sdk_2025\build\bin\Release\plugins\C4D_DollyZoom\C4D_DollyZoom.pdb"
    if (Test-Path $pdbPath) {
        Write-Host "Removing unnecessary .pdb file..."
        Remove-Item -Path $pdbPath -Force
    }
}

Write-Host "Build complete successfully!"
