#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX -DTinyMAT_QT5_SUPPORT=ON -DTinyMAT_BUILD_SHARED_LIBS=ON -DTinyMAT_BUILD_STATIC_LIBS=ON -DTinyMAT_OPENCV_SUPPORT=ON .. 
cmake --build . --target install
