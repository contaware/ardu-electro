@echo off

REM Success & Failure counters
set /A "success_count=0"
set /A "failure_count=0"

REM Test whether arduino-cli.exe is in PATH
arduino-cli version
if %ERRORLEVEL% neq 0 (
    echo Hint: please add arduino-cli.exe to your PATH
    goto :end
)

REM Remove old log file if existing
set logfile=%~dpn0_log.txt
if exist "%logfile%" del "%logfile%"

REM This batch file's directory with trailing backslash
set batchdir=%~dp0

REM Recursively loop through all .ino files
for /R "%batchdir%" %%A in (*.ino) do call :compile "%%~A"

REM Do end script
:end
echo Total: Success %success_count%  Failure %failure_count%
pause
goto :eof


REM Compile subroutine
:compile
set sketchpath=%~1

REM We need the sketch directory path without trailing backslash
for %%i in ("%sketchpath%") do set sketchdir=%%~dpi
if not "%sketchdir:~-1%"=="\" set sketchdir=%sketchdir%\
set sketchdirnobackslash=%sketchdir:~0,-1%

REM The sketch name
for %%i in ("%sketchpath%") do set sketchname=%%~ni

REM Determine fqbn from sketch name
call :setfqbn %sketchname%

REM Compile with arduino-cli
echo ************************************************************************>> "%logfile%" 2>&1
echo   %sketchname%>> "%logfile%" 2>&1
echo ************************************************************************>> "%logfile%" 2>&1
echo COMPILE WITH "%fqbn%">> "%logfile%" 2>&1
arduino-cli compile --no-color --fqbn %fqbn% "%sketchdirnobackslash%">> "%logfile%" 2>&1
if %ERRORLEVEL% equ 0 (
    echo Compiled "%sketchname%" with %fqbn%
    set /A "success_count+=1"
    exit /B
)
echo FAILED compiling "%sketchpath%" with %fqbn%
set /A "failure_count+=1"
exit /B


REM fqbn determination subroutine
:setfqbn
echo %sketchname% | find /I "_UNOR4WiFi">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:renesas_uno:unor4wifi
    exit /B
)
echo %sketchname% | find /I "_NanoESP32">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:esp32:nano_nora
    exit /B
)
echo %sketchname% | find /I "_328P">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:avr:uno
    exit /B
)
echo %sketchname% | find /I "_STM32F1">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=STMicroelectronics:stm32:GenF1
    exit /B
)
echo %sketchname% | find /I "_STM32F4">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=STMicroelectronics:stm32:GenF4
    exit /B
)
echo %sketchname% | find /I "_RENESAS">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:renesas_uno:unor4wifi
    exit /B
)
echo %sketchname% | find /I "_MKR">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:samd:mkrwifi1010
    exit /B
)
echo %sketchname% | find /I "_ESP">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:esp32:nano_nora
    exit /B
)
echo %sketchname% | find /I "_D1mini">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=esp8266:esp8266:d1_mini
    exit /B
)
echo %sketchname% | find /I "_WiFi">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:samd:mkrwifi1010
    exit /B
)
echo %sketchname% | find /I "_I2S">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:samd:arduino_zero_edbg
    exit /B
)
echo %sketchname% | find /I "_DAC">nul
if %ERRORLEVEL% equ 0 (
    set fqbn=arduino:renesas_uno:unor4wifi
    exit /B
)
set fqbn=arduino:avr:leonardo
exit /B
