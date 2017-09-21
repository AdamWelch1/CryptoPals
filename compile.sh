#!/bin/sh

#Debug
g++ --std=c++11 -Wno-write-strings -DDEBUG_MODE -ltomcrypt -fopenmp -g -o main.bin main.cpp b64.cpp

#Optimized
#g++ --std=c++11 -Wno-write-strings -DDEBUG_MODE -ltomcrypt -fopenmp -m64 -finline-functions -ffunction-cse -fexpensive-optimizations -fdelete-null-pointer-checks -fomit-frame-pointer -O3 -Ofast -faggressive-loop-optimizations -o main.bin main.cpp b64.cpp

#./main