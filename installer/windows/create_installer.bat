:: this script must set QTDIR32 and QTDIR64 paths to the root of corresponding Qt builds. Example:
:: set QTDIR32=k:\Qt\5\5.4\msvc2013_opengl\
:: set QTDIR64=k:\Qt\5\5.4\msvc2013_64_opengl\

call set_qt_paths.bat
if not defined WIN_SDK set WIN_SDK=10.0.18362.0

RMDIR /S /Q binaries\

SETLOCAL

if "%programfiles(x86)%" == "%programfiles%" (
    set SYS64=%systemroot%\Sysnative
) else (
    set SYS64=%systemroot%\System32
)

if exist "%ProgramW6432%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    call "%ProgramW6432%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
) else (
    if exist "%ProgramW6432%\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvarsall.bat" (
        call "%ProgramW6432%\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvarsall.bat" amd64
    ) else (
        call "%ProgramW6432%\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
    )
)

:: X64
pushd ..\..\
del .qmake.stash
%QTDIR64%\bin\qmake.exe -tp vc -r
popd

msbuild ../../file-commander.sln /t:Build /p:Configuration=Release;Platform="x64";PlatformToolset=v143
if not %errorlevel% == 0 goto build_fail

xcopy /R /Y ..\..\bin\release\x64\FileCommander.exe binaries\64\
xcopy /R /Y ..\..\bin\release\x64\plugin_*.dll binaries\64\
xcopy /R /Y "3rdparty binaries"\64\* binaries\64\

SETLOCAL
SET PATH=%QTDIR64%\bin\
%QTDIR64%\bin\windeployqt.exe --dir binaries\64\Qt --force --release --no-compiler-runtime --no-angle --no-translations binaries\64\FileCommander.exe
FOR %%p IN (binaries\64\plugin_*.dll) DO %QTDIR64%\bin\windeployqt.exe --dir binaries\64\Qt --release --no-compiler-runtime --no-angle --no-translations %%p
ENDLOCAL

xcopy /R /Y %SYS64%\msvcp140.dll binaries\64\msvcr\
xcopy /R /Y %SYS64%\msvcp140_1.dll binaries\64\msvcr\
xcopy /R /Y %SYS64%\vcruntime140.dll binaries\64\msvcr\
xcopy /R /Y %SYS64%\vcruntime140_1.dll binaries\64\msvcr\

xcopy /R /Y "%programfiles(x86)%\Windows Kits\10\Redist\ucrt\DLLs\x64\*" binaries\64\msvcr\
if %ERRORLEVEL% GEQ 1 goto windows_sdk_not_found

del binaries\64\Qt\opengl*.*

"%programfiles(x86)%\Inno Setup 6\iscc" setup.iss

ENDLOCAL
exit /b 0

:build_fail
ENDLOCAL
echo Build failed
pause
exit /b 1

:windows_sdk_not_found
ENDLOCAL
echo Windows SDK not found (required for CRT DLLs)
pause
exit /b 1