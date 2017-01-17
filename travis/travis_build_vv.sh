#!/bin/bash
set -ev
MAKE="make --jobs=$NUM_THREADS --keep-going"

#Prepare cmake arguments following the vv version
cmake_arg_str=" -DCLITK_BUILD_REGISTRATION=OFF
-DCLITK_BUILD_SEGMENTATION=OFF
-DCLITK_BUILD_TOOLS=OFF
-DCLITK_BUILD_VV=ON
-DCLITK_EXPERIMENTAL=OFF"

#CMAKE and MAKE
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake $cmake_arg_str ..
$MAKE
cd -
cd $cwd
