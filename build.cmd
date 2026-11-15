@echo off
rem Builds release\OpenRGBGameSensePlugin.dll. Override QTDIR / VCVARS if installed elsewhere.
if not defined QTDIR  set QTDIR=C:\Qt\6.8.3\msvc2022_64
if not defined VCVARS set VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat

call "%VCVARS%" x64 || exit /b 1
cd /d "%~dp0"
git submodule update --init --depth 1 || exit /b 1
"%QTDIR%\bin\qmake.exe" OpenRGBGameSensePlugin.pro CONFIG+=release || exit /b 1
C:\Qt\jom\jom.exe || exit /b 1
