#=========================================================
# CLITK = Command Line ITK
project(clitk)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
if(NOT DEFINED CLITK_SOURCE_DIR)
  set(CLITK_SOURCE_DIR ${PROJECT_SOURCE_DIR})
endif(NOT DEFINED CLITK_SOURCE_DIR)
#=========================================================

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(MSVC)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj" )
endif(MSVC)

#=========================================================
include(${CLITK_SOURCE_DIR}/cmake/common.cmake)
include(${CLITK_SOURCE_DIR}/cmake/dependencies.cmake)
include(${CLITK_SOURCE_DIR}/cmake/build_opt.cmake)
#=========================================================
#=========================================================

# Select what is compiled
add_subdirectory(${CLITK_SOURCE_DIR}/common ${PROJECT_BINARY_DIR}/common)
add_subdirectory(${CLITK_SOURCE_DIR}/tools ${PROJECT_BINARY_DIR}/tools)
add_subdirectory(${CLITK_SOURCE_DIR}/segmentation ${PROJECT_BINARY_DIR}/segmentation)
add_subdirectory(${CLITK_SOURCE_DIR}/registration ${PROJECT_BINARY_DIR}/registration)
add_subdirectory(${CLITK_SOURCE_DIR}/cluster_tools ${PROJECT_BINARY_DIR}/cluster_tools)
