#!/bin/bash

cd "build"

echo "Cleaning up old build files..."
rm -rf *

echo "Configuring the build with CMake..."
cmake ../

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "Building the project..."
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful! Executable is ready to run."

echo "Running the application..."
./AppMain
