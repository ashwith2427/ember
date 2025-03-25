#!/bin/bash
set +xe

clang -g -MMD -MP -MF main.d -o main main.c -lm