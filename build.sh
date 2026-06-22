#!/bin/bash

# Exit on error
set -e

echo "Compiling cache locality benchmark..."

# Compile with GCC, optimizing for speed (-O3) and using C++20
g++ -O3 -std=c++20 main.cpp -o hello

echo "Compilation successful! Run it with: ./hello"
