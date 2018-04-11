#!/bin/bash
set -ev
MAKE="make --jobs=$NUM_THREADS --keep-going"

#Prepare cmake arguments following the ITK version
if [ "$C11" == "true" ]; then
  if [ "$ITK_VERSION" == "4.6" ]; then
    itk_repo_str=" --branch v4.6.0 https://github.com/InsightSoftwareConsortium/ITK.git --depth 1"
    cmake_arg_str=" -DCMAKE_CXX_FLAGS=-std=c++11 -DModule_ITKVtkGlue=ON -DVTK_DIR=$VTK_DIR -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF "
  elif [ "$ITK_VERSION" == "4.9.1" ]; then
    itk_repo_str=" --branch v4.9.1 https://github.com/InsightSoftwareConsortium/ITK.git --depth 1"
    cmake_arg_str=" -DCMAKE_CXX_FLAGS=-std=c++11 -DModule_ITKVtkGlue=ON -DVTK_DIR=$VTK_DIR -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF "
  elif [ "$ITK_VERSION" == "4.13.0" ]; then
    itk_repo_str=" --branch v4.13.0 https://github.com/InsightSoftwareConsortium/ITK.git --depth 1"
    cmake_arg_str=" -DCMAKE_CXX_FLAGS=-std=c++11 -DModule_ITKVtkGlue=ON -DVTK_DIR=$VTK_DIR -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF "
  fi
else
  if [ "$ITK_VERSION" == "4.6" ]; then
    itk_repo_str=" --branch v4.6.0 https://github.com/InsightSoftwareConsortium/ITK.git --depth 1"
    cmake_arg_str=" -DModule_ITKVtkGlue=ON -DVTK_DIR=$VTK_DIR -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF "
  elif [ "$ITK_VERSION" == "4.9.1" ]; then
    itk_repo_str=" --branch v4.9.1 https://github.com/InsightSoftwareConsortium/ITK.git --depth 1"
    cmake_arg_str=" -DModule_ITKVtkGlue=ON -DVTK_DIR=$VTK_DIR -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF "
  elif [ "$ITK_VERSION" == "4.13.0" ]; then
    itk_repo_str=" --branch v4.13.0 https://github.com/InsightSoftwareConsortium/ITK.git --depth 1"
    cmake_arg_str=" -DModule_ITKVtkGlue=ON -DVTK_DIR=$VTK_DIR -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF "
  fi
fi

if [ -d $ITK_SOURCE_DIR ]; then
echo $ITK_SOURCE_DIR exists
if [ ! -f $ITK_SOURCE_DIR/CMakeLists.txt ]; then
echo $ITK_SOURCE_DIR does not contain CMakeList.txt
rm -rf $ITK_SOURCE_DIR
fi
fi

#git clone in the ITK source folder
if [ ! -d $ITK_SOURCE_DIR ]; then
git clone $itk_repo_str $ITK_SOURCE_DIR
fi

#CMAKE and MAKE
mkdir -p $ITK_DIR
cd $ITK_DIR
cmake $cmake_arg_str $ITK_SOURCE_DIR
$MAKE
cd -

cd $cwd
