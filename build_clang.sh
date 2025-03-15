#!/bin/bash
set +xe
clang -o main main.c -L/third_party/clang/linux -lclang -I/third_party/clang/clang-c -lm