#!/bin/bash

# Set the working directory
cd "$(dirname "$0")"
echo "Working directory: $(pwd)"

# Check if there is an existing cs526 image
docker image ls | grep cs526

if [ $? -eq 1 ]; then
    echo "No cs526 image found. Please build it before continuing."
    echo "Run the build script first: ./docker/build.sh"
    exit 1
else
    echo "Found an existing cs526 image. Proceeding to run the container..."
fi

docker run --name cs526 --rm -it -v "$(pwd)/..:/home/cs526/cos_ssa" cs526 bash
