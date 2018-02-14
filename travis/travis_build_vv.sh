#!/bin/bash
set -ev
MAKE="make --jobs=$NUM_THREADS --keep-going"

#Prepare cmake arguments following the vv version
if [ "$C11" == "true" ]; then
  cmake_arg_str=" -DCMAKE_CXX_FLAGS=-std=c++11
  -DCLITK_BUILD_REGISTRATION=OFF
  -DCLITK_BUILD_SEGMENTATION=OFF
  -DCLITK_BUILD_TOOLS=OFF
  -DCLITK_BUILD_VV=ON
  -DCLITK_EXPERIMENTAL=OFF"
else
  cmake_arg_str=" -DCLITK_BUILD_REGISTRATION=OFF
  -DCLITK_BUILD_SEGMENTATION=OFF
  -DCLITK_BUILD_TOOLS=OFF
  -DCLITK_BUILD_VV=ON
  -DCLITK_EXPERIMENTAL=OFF"
fi

#CMAKE and MAKE
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake $cmake_arg_str ..
$MAKE
cd ..
build-wrapper-linux-x86-64 --out-dir bw-output make all
sonar-scanner
cd $cwd
