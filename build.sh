#!/bin/bash
# Build script for gMCP - gRPC-based Model Context Protocol

set -e

echo "=== gMCP Build Script ==="

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Run CMake
echo "Running CMake configuration..."
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_STANDARD=20

# Build
echo "Building gMCP..."
make -j$(nproc)

echo ""
echo "=== Build Complete ==="
echo "Server executable: build/gmcp_server"
echo "Client executable: build/gmcp_client"
echo ""
echo "To run the server: ./build/gmcp_server"
echo "To run the client: ./build/gmcp_client"
