#!/bin/sh

g++ --std=c++11 -DDEBUG_MODE -fopenmp -g -o main main.cpp b64.cpp
./main