#!/bin/bash

# Navigate to the build directory
cd "/Users/Nicholas/Desktop/Embedded_Software_Homework/build"

# Remove previous build files to ensure a clean build
echo "Cleaning up old build files..."
rm -rf *

# Run CMake to configure the project
echo "Configuring the build with CMake..."
cmake ..

# Check if CMake configuration was successful
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project with Make
echo "Building the project..."
make

# Check if the make build was successful
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful! Executable is ready to run."

# Optionally, you can run the executable
echo "Running the application..."
./AppMain
