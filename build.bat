@echo off 
setlocal
cd /d %~dp0

:: Clone vcpkg repo if missing.
if not exist external\vcpkg\bootstrap-vcpkg.bat (
    echo Cloning vcpkg... 
    git clone https://github.com/microsoft/vcpkg.git external\vcpkg
)

:: Bootstrap vcpkg
cd external\vcpkg 
call bootstrap-vcpkg.bat
cd ../..

cmake --preset Chip8
:: Build the project
cmake --build out\build\Chip8