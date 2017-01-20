#!/bin/bash
set -ev
MAKE="make --jobs=$NUM_THREADS --keep-going"

if [ "$QT_VERSION" == "4.8.7" ]; then
  QT_MAJOR_VERSION=4
elif [ "$QT_VERSION" == "5.5.1" ]; then
  QT_MAJOR_VERSION=5
fi

#Prepare cmake arguments following the VTK version
if [ "$VTK_VERSION" == "5.10" ]; then
vtk_repo_str=" --branch v5.10.0 https://github.com/Kitware/VTK.git --depth 1"
cmake_arg_str=" $cmake_arg_str 
  -DBUILD_SHARED_LIBS=1
  -DCMAKE_BUILD_TYPE=Release
  -DBUILD_TESTING=0
  -DBUILD_EXAMPLES=0
  -DBUILD_DOCUMENTATION=0
  -DVTK_USE_QT=ON
  -DVTK_USE_RENDERING=ON
  -DVTK_USE_VIEWS=ON
  -DVTK_QT_VERSION="$QT_MAJOR_VERSION" "
elif [ "$VTK_VERSION" == "6.3" ]; then
vtk_repo_str=" --branch v6.3.0 https://github.com/Kitware/VTK.git --depth 1"
cmake_arg_str=" $cmake_arg_str 
  -DBUILD_SHARED_LIBS=1
  -DCMAKE_BUILD_TYPE=Release
  -DBUILD_TESTING=0
  -DBUILD_EXAMPLES=0
  -DBUILD_DOCUMENTATION=0
  -DVTK_Group_StandAlone=ON
  -DVTK_Group_Imaging=OFF
  -DVTK_Group_MPI=OFF
  -DVTK_Group_Rendering=ON
  -DVTK_Group_Tk=OFF
  -DVTK_Group_Views=OFF
  -DVTK_Group_Web=OFF
  -DModule_vtkGUISupportQt=ON
  -DModule_vtkGUISupportQtOpenGL=ON
  -DModule_vtkGUISupportQtSQL=ON
  -DModule_vtkRenderingQt=ON
  -DModule_vtkViewsQt=ON
  -DVTK_QT_VERSION="$QT_MAJOR_VERSION" "
elif [ "$VTK_VERSION" == "7.0.0" ]; then
vtk_repo_str=" --branch v7.0.0 https://github.com/Kitware/VTK.git --depth 1"
cmake_arg_str=" $cmake_arg_str
  -DBUILD_SHARED_LIBS=1
  -DCMAKE_BUILD_TYPE=Release
  -DBUILD_TESTING=0
  -DBUILD_EXAMPLES=0
  -DBUILD_DOCUMENTATION=0
  -DVTK_Group_StandAlone=ON
  -DVTK_Group_Imaging=OFF
  -DVTK_Group_MPI=OFF
  -DVTK_Group_Rendering=ON
  -DVTK_Group_Tk=OFF
  -DVTK_Group_Views=OFF
  -DVTK_Group_Web=OFF
  -DVTK_RENDERING_BACKEND=OpenGL
  -DModule_vtkGUISupportQt=ON
  -DModule_vtkGUISupportQtOpenGL=ON
  -DModule_vtkGUISupportQtSQL=ON
  -DModule_vtkRenderingQt=ON
  -DModule_vtkViewsQt=ON
  -DVTK_QT_VERSION="$QT_MAJOR_VERSION" "
fi

if [ -d $VTK_SOURCE_DIR ]; then
echo $VTK_SOURCE_DIR exists
if [ ! -f $VTK_SOURCE_DIR/CMakeLists.txt ]; then
echo $VTK_SOURCE_DIR does not contain CMakeList.txt
rm -rf $VTK_SOURCE_DIR
fi
fi

#git clone in the VTK source folder
if [ ! -d "$VTK_SOURCE_DIR" ]; then
git clone $vtk_repo_str ${VTK_SOURCE_DIR}
fi

#CMAKE and MAKE
mkdir -p ${VTK_DIR}
cd ${VTK_DIR}
cmake $cmake_arg_str ${VTK_SOURCE_DIR}
$MAKE
