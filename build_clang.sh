#!/bin/bash
set +xe

clang -g -fsanitize=address -MMD -MP -MF main.d -o main main.c -lm