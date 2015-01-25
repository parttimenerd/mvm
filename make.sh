#!/bin/sh

if [ ! -e build ]; then
    mkdir build
fi

# set favorite compilers
export CC=clang
export CXX=clang++

# -V or -VV options are helpful for testing
cd build && cmake .. && make 
