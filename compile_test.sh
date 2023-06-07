#!/bin/bash

# Path to the project directory
project_dir="./build"

# Build command (modify as per your project)
build_command="make"

# Perform the compilation time test
echo "Running compilation time test..."

total_time=0
start_time=$(date +%s.%N)

# Change to project directory
cd "$project_dir"

# Perform the build
eval "$build_command"

end_time=$(date +%s.%N)
elapsed_time=$(echo "$end_time - $start_time" | bc)

echo "Compilation time: $elapsed_time seconds"
