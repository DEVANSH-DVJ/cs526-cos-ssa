#!/bin/bash

# Set the working directory
cd "$(dirname "$0")"
echo "Working directory: $(pwd)"

# Check if there is an existing cs526 image
docker image ls | grep cs526

if [ $? -eq 1 ]; then
    echo "No existing cs526 image was found. Please build the image first."
    echo "Run the build script first: ./build.sh"
    exit 1
else
    echo "An existing cs526 image was found"
fi

docker run --name cs526 --rm -it -v "$(pwd)/..:/home/cs526/cos_ssa" cs526 bash
