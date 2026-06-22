@echo off
:: Try standard VS 2022 Community path, then VS 2022 Build Tools path
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
) else (
    echo Visual Studio 2022 vcvars64.bat not found in standard paths. Please build manually using the commands in README.md.
    exit /b 1
)

cl /O2 /std:c++20 /EHsc main.cpp /Fe:hello.exe
