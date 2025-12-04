#!/bin/bash

# Create build directory
mkdir -p build
cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_TESTS=ON \
      -DUSE_SYSTEM_SQLITE=OFF \
      ..

# Build
cmake --build . --config Release --parallel 4

# Run tests
ctest --output-on-failure

# Install locally
# cmake --install . --prefix ../install