#!/bin/bash

# Linux build script for OpenAI Harmony C++ library

echo "Building OpenAI Harmony C++ Library for Linux"
echo "=============================================="

# Check for required dependencies
check_dependency() {
    if ! command -v $1 &> /dev/null; then
        echo "Error: $1 is not installed"
        echo "Please install $1 and try again"
        exit 1
    fi
}

echo "Checking dependencies..."
check_dependency cmake
check_dependency pkg-config

# Check for required libraries
echo "Checking for required libraries..."

# Check for nlohmann/json
if ! pkg-config --exists nlohmann_json; then
    echo "Warning: nlohmann/json not found via pkg-config"
    echo "Please install nlohmann-json3-dev (Ubuntu/Debian) or nlohmann-json-devel (RHEL/CentOS)"
fi

# Check for OpenSSL
if ! pkg-config --exists openssl; then
    echo "Error: OpenSSL not found"
    echo "Please install libssl-dev (Ubuntu/Debian) or openssl-devel (RHEL/CentOS)"
    exit 1
fi

# Check for CURL
if ! pkg-config --exists libcurl; then
    echo "Error: CURL not found"
    echo "Please install libcurl4-openssl-dev (Ubuntu/Debian) or libcurl-devel (RHEL/CentOS)"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_TESTS=ON

if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    exit 1
fi

# Build the project
echo "Building..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

echo ""
echo "Build completed successfully!"
echo ""
echo "Executables are located in:"
echo "  build/harmony_example"
echo "  build/interactive_example"
echo ""
echo "Shared library is located in:"
echo "  build/libopenai_harmony.so"
echo ""
echo "To run the examples:"
echo "  cd build"
echo "  ./harmony_example"
echo "  ./interactive_example"

cd ..
