#/bin/sh

mkdir -p build && cd build
cmake .. -DBUILD_DEBUG=OFF
cmake --build . -j4
if [[ $? -ne 0 ]]; then
    echo "Build failed!"
    exit 1
fi
cd ..
