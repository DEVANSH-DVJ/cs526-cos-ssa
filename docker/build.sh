#!/bin/bash

# Set the working directory
cd "$(dirname "$0")"
echo "Working directory: $(pwd)"

# Check if there is an existing cs526 image
docker image ls | grep cs526

if [ $? -eq 1 ]; then
    echo "No existing cs526 image was found"
else
    echo "An existing cs526 image was found"
    # Try removing has the image
    docker image rm cs526
    if [ $? -eq 1 ]; then
        echo "An existing cs526 image has a container running. Please stop the container before building a new image."
        docker container ls -a | grep cs526
        exit 1
    fi
fi

# Build the cs526 image
docker build -t cs526 .

# Remove dangling images
docker image prune -f
