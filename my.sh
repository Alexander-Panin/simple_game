#!/bin/bash

clang++ parser.cpp -std=c++11 -stdlib=libc++ -o parser
./lexic < decl | ./parser
