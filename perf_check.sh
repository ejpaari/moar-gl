#!/bin/bash

BUILD_DIR=../build-moar-gl-Desktop_Qt_5_4_1_GCC_64bit-Release
CPPCHECK_DIR=myapp/

function execute {
echo "Running valgrind with $1..."
valgrind --tool=$1 ./$BUILD_DIR/moar-gl
echo "valgrind with $1 complete"
}

execute cachegrind
echo ""
execute callgrind
