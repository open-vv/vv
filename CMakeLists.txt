#=========================================================
# CLITK = Command Line ITK
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
PROJECT(clitk)
SET(CLITK_SOURCE_DIR ${PROJECT_SOURCE_DIR})
#=========================================================

#=========================================================
INCLUDE(cmake/common.cmake)
INCLUDE(cmake/dependencies.cmake)
INCLUDE(cmake/build_opt.cmake)
#=========================================================
#=========================================================

# Select what is compiled
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(segmentation)
ADD_SUBDIRECTORY(registration)
