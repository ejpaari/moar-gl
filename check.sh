#!/bin/bash

BUILD_DIR=../build-moar-gl-Desktop_Qt_5_4_1_GCC_64bit-Debug
CPPCHECK_DIR=myapp/

echo "Running valgrind..."
VALGRIND_FILE=valgrind.txt
valgrind --leak-check=full ./$BUILD_DIR/moar-gl 2> $VALGRIND_FILE
echo "valgrind complete"

echo "Running cppcheck..."
CPPCHECK_FILE=cppcheck.txt
cppcheck --enable=all engine/ $CPPCHECK_DIR 2> $CPPCHECK_FILE
echo "cppcheck complete"

echo ""
echo "valgrind:"
echo | grep "LEAK SUMMARY" -A 8 $VALGRIND_FILE

echo ""
echo "cppcheck:"
if [[ -s $CPPCHECK_FILE ]] ; then
    cat $CPPCHECK_FILE
else
    echo "No errors"
fi;
