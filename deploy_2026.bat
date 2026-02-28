@echo off
set "SOURCE=C:\Users\user\Desktop\cpp\C4D_DollyZoom\sdk_2026\build\bin\Release\plugins\C4D_DollyZoom"
set "DEST=\\vmware-host\Shared Folders\plugins\C4D_DollyZoom"

echo.
echo [DEPLOYMENT] Copying C4D_DollyZoom via PowerShell...
echo [SOURCE] %SOURCE%
echo [DEST]   %DEST%
echo.

:: Using PowerShell Copy-Item which is less sensitive to HGFS metadata issues than xcopy/robocopy
:: $ErrorActionPreference = 'Stop' ensures that both Remove-Item and Copy-Item failures are caught
powershell -Command "$ErrorActionPreference = 'Stop'; try { if (Test-Path '%DEST%') { Remove-Item -Path '%DEST%' -Recurse -Force }; Copy-Item -Path '%SOURCE%' -Destination '%DEST%' -Recurse -Force } catch { exit 1 }"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Copy failed! Exit Code: %ERRORLEVEL%
    echo [ERROR] Cinema 4D is likely running on the remote machine and locking files.
    echo [ERROR] Please close Cinema 4D and try again.
    echo.
    ::pause
    exit /b %ERRORLEVEL%
)

echo [SUCCESS] Plugin files successfully copied/updated!
echo.
