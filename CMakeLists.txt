#=========================================================
# CLITK = Command Line ITK
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
PROJECT(clitk)
IF(NOT DEFINED CLITK_SOURCE_DIR)
  SET(CLITK_SOURCE_DIR ${PROJECT_SOURCE_DIR})
ENDIF(NOT DEFINED CLITK_SOURCE_DIR)
#=========================================================

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#=========================================================
INCLUDE(${CLITK_SOURCE_DIR}/cmake/common.cmake)
INCLUDE(${CLITK_SOURCE_DIR}/cmake/dependencies.cmake)
INCLUDE(${CLITK_SOURCE_DIR}/cmake/build_opt.cmake)
#=========================================================
#=========================================================

# Select what is compiled
ADD_SUBDIRECTORY(${CLITK_SOURCE_DIR}/common ${PROJECT_BINARY_DIR}/common)
ADD_SUBDIRECTORY(${CLITK_SOURCE_DIR}/tools ${PROJECT_BINARY_DIR}/tools)
ADD_SUBDIRECTORY(${CLITK_SOURCE_DIR}/segmentation ${PROJECT_BINARY_DIR}/segmentation)
ADD_SUBDIRECTORY(${CLITK_SOURCE_DIR}/registration ${PROJECT_BINARY_DIR}/registration)
