#!/bin/bash

# Simple test runner script
echo "=== Building and Running Key-Value Store Tests ==="

# Build the project
echo "Building project..."
make debug

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Check if test executable exists
if [ ! -f "./build/debug/test/tests" ]; then
    echo "Test executable not found!"
    exit 1
fi

# Run tests
echo "Running tests..."
./build/debug/test/tests

echo "Test run completed."