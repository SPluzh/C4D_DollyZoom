@echo off
set "SOURCE=C:\Users\user\Desktop\cpp\C4D_DollyZoom\sdk_2026\build\bin\Release\plugins\C4D_DollyZoom"
set "DEST=\\vmware-host\Shared Folders\plugins\C4D_DollyZoom"

echo.
echo [DEPLOYMENT] Copying C4D_DollyZoom via PowerShell...
echo [SOURCE] %SOURCE%
echo [DEST]   %DEST%
echo.

:: Using PowerShell Copy-Item which is less sensitive to HGFS metadata issues than xcopy/robocopy
powershell -Command "if (Test-Path '%DEST%') { Remove-Item -Path '%DEST%' -Recurse -Force }; Copy-Item -Path '%SOURCE%' -Destination '%DEST%' -Recurse -Force"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Copy failed with exit code %ERRORLEVEL%.
    echo Please ensure Cinema 4D is closed and the network share is accessible.
    echo.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] Plugin files successfully copied/updated!
echo.
pause
