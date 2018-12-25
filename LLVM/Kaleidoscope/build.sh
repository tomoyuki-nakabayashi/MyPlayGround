#!/bin/bash
clang++ -g src/toy.cpp `llvm-config-6.0 --cxxflags --ldflags --system-libs --libs core mcjit native` -O3 -o toy -rdynamic -Wl,--export-dynamic
