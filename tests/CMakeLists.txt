cmake_policy(SET CMP0005 NEW)
#=========================================================
#=========================================================
# Data tests path
FIND_PATH(CLITK_TEST_DATA_PATH Lung3D.mhd)
IF(NOT ${CLITK_TEST_DATA_PATH} MATCHES "NOTFOUND$")
 SET(CLITK_TEST_DATA_PATH "${CLITK_TEST_DATA_PATH}/")
ENDIF()
ADD_DEFINITIONS(-DCLITK_TEST_DATA_PATH="${CLITK_TEST_DATA_PATH}")
#=========================================================
#=========================================================
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests/bin)
SET(BUILDNAME ${CMAKE_SYSTEM_NAME}${CMAKE_OSX_ARCHITECTURES} CACHE INTERNAL DOCSTRING)
#=========================================================
#=========================================================
# Enabled options to test
IF(CLITK_BUILD_TOOLS)
  ADD_SUBDIRECTORY(tools)
ENDIF()
#IF(CLITK_BUILD_VV)
#  ADD_SUBDIRECTORY(vv)
#ENDIF()
# IF(CLITK_BUILD_SEGMENTATION)
#   ADD_SUBDIRECTORY(segmentation)
# ENDIF()
# 
# IF(CLITK_BUILD_REGISTATION)
#   ADD_SUBDIRECTORY(registration)
# ENDIF()
#ADD_SUBDIRECTORY(common)
#=========================================================

