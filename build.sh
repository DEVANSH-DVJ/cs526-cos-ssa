#/bin/sh

# If the script is run with the argument "clean", it will remove the build directory.
if [[ $1 == "clean" ]]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# Check if the build directory exists, build only new changes
if [[ -d "build" ]]; then
    echo "Build directory exists. Rebuiding only for new changes..."
    cd build
else
    echo "Build directory does not exist. Creating..."
    mkdir -p build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi

cmake --build . -j4
if [[ $? -ne 0 ]]; then
    echo "Build failed!"
    exit 1
fi
cd ..
