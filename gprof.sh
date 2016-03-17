#!/bin/bash

BUILD_DIR=../build-moar-gl-Desktop_Qt_5_4_1_GCC_64bit-Release
cp ./$BUILD_DIR/gmon.out gmon.out
gprof ./$BUILD_DIR/moar-gl | gprof2dot -ws | dot -Tpng -o gprof.png
