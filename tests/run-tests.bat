@echo off
:: Builds and runs the coreProps.json parser self-check against the plugin's real source file.
if not defined QTDIR  set QTDIR=C:\Qt\6.8.3\msvc2022_64
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul || exit /b 1
set SRC=%~dp0..
set ORGB=%SRC%\OpenRGB
set OUT=%TEMP%\gamesense-plugin-tests
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /std:c++17 /EHsc /utf-8 /Zc:__cplusplus /permissive- /D_WIN32_WINNT=0x0A00 /DNOMINMAX /DQT_NO_VERSION_TAGGING /U NDEBUG ^
  /I "%SRC%" /I "%ORGB%" /I "%ORGB%\RGBController" /I "%ORGB%\dependencies\json" /I "%ORGB%\dependencies\httplib" ^
  /I "%QTDIR%\include" /I "%QTDIR%\include\QtCore" /I "%QTDIR%\include\QtGui" /I "%QTDIR%\include\QtWidgets" ^
  "%~dp0gamesense_parse_test.cpp" "%SRC%\SteelSeriesGameSense.cpp" ^
  /Fo"%OUT%\\" /Fe"%OUT%\gamesense_parse_test.exe" ws2_32.lib || exit /b 1
"%OUT%\gamesense_parse_test.exe"
