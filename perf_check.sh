#!/bin/bash

BUILD_DIR=../build-moar-gl-Desktop_Qt_5_4_1_GCC_64bit-Debug
CPPCHECK_DIR=myapp/

echo "Running valgrind with cachegrind..."
VALGRIND_FILE=valgrind.txt
valgrind --tool=cachegrind ./$BUILD_DIR/moar-gl
echo "valgrind with cachegrind complete"
