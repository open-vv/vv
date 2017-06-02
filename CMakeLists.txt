#=========================================================
# CLITK = Command Line ITK
project(clitk)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0007 NEW)
    cmake_policy(SET CMP0053 NEW)
endif(COMMAND cmake_policy)
if(NOT DEFINED CLITK_SOURCE_DIR)
  set(CLITK_SOURCE_DIR ${PROJECT_SOURCE_DIR})
endif(NOT DEFINED CLITK_SOURCE_DIR)
# Default build type
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
#=========================================================

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(MSVC)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj" )
endif(MSVC)

if(${CLITK_USE_PACS_CONNECTION})
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgdcmMEXD" )
endif(${CLITK_USE_PACS_CONNECTION})

#=========================================================
include(${CLITK_SOURCE_DIR}/cmake/common.cmake)
include(${CLITK_SOURCE_DIR}/cmake/dependencies.cmake)
include(${CLITK_SOURCE_DIR}/cmake/build_opt.cmake)
#=========================================================
#=========================================================
SET(vvPacsConnection true)
# Select what is compiled
add_subdirectory(${CLITK_SOURCE_DIR}/common ${PROJECT_BINARY_DIR}/common)
add_subdirectory(${CLITK_SOURCE_DIR}/tools ${PROJECT_BINARY_DIR}/tools)
add_subdirectory(${CLITK_SOURCE_DIR}/segmentation ${PROJECT_BINARY_DIR}/segmentation)
add_subdirectory(${CLITK_SOURCE_DIR}/registration ${PROJECT_BINARY_DIR}/registration)
add_subdirectory(${CLITK_SOURCE_DIR}/cluster_tools ${PROJECT_BINARY_DIR}/cluster_tools)
