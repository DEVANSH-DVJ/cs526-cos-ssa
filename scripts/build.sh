#!/bin/bash

# Set the working directory
cd "$(dirname "$0")"/..

# Ensure build directory exists
if [[ -d "build" ]]; then
    echo "Build directory exists."
else
    echo "Build directory does not exist. Creating..."
    mkdir -p build
fi

cd build

# (Re)generate Ninja build files out-of-source
cmake .. -DCMAKE_BUILD_TYPE=Release
if [[ $? -ne 0 ]]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Run the build
make -j4
if [[ $? -ne 0 ]]; then
    echo "Build failed!"
    exit 1
fi

echo "Build succeeded."
