#!/bin/bash
set -e
cd "$(dirname "$0")"

if [ ! -f external/vcpkg/bootstrap-vcpkg.sh ]; then
    echo "Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg.git external/vcpkg
fi

# bootstrap vcpkg 
./external/vcpkg/bootstrap-vcpkg.sh

cmake --preset Chip8
cmake --build out\build\Chip8