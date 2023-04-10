#!/bin/bash

# Set your project's source directory and build directory
SOURCE_DIR="/home/erik/Desktop/Project-Alice"
BUILD_DIR="/home/erik/Desktop/Project-Alice/build"

# Function to build the project and measure the compilation time
build_and_time() {
    # rm -rf "$BUILD_DIR"
    # mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake "$SOURCE_DIR" -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang-13 -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++-13
    start=$(date +%s.%N)
    cmake --build . --target Alice -j
    # make # Or use 'ninja', depending on your build system
    end=$(date +%s.%N)
    elapsed=$(echo "$end - $start" | bc)
    echo "Elapsed time: $elapsed seconds"
}

# Build with precompiled headers
# echo "Building with precompiled headers..."
build_and_time

# # Disable precompiled headers in CMakeLists.txt
# sed -i 's/^target_precompile_headers/# &/' "$SOURCE_DIR/CMakeLists.txt"

# # Build without precompiled headers
# echo "Building without precompiled headers..."
# build_and_time

# # Re-enable precompiled headers in CMakeLists.txt
# sed -i 's/^# *target_precompile_headers/target_precompile_headers/' "$SOURCE_DIR/CMakeLists.txt"
