#!/bin/sh

g++ --std=c++11 -Wno-write-strings -DDEBUG_MODE -fopenmp -g -o main main.cpp b64.cpp
./main