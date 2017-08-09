#!/bin/sh

g++ --std=c++11 -Wno-write-strings -DDEBUG_MODE -ltomcrypt -fopenmp -g -o main.bin main.cpp b64.cpp
#./main