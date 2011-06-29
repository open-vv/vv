cmake_policy(SET CMP0005 NEW)
#=========================================================

#=========================================================
# Data tests path

OPTION(CLITK_DATA_PATH ${CMAKE_CURRENT_BINARY_DIR}/data)
ADD_DEFINITIONS(-DCLITK_DATA_PATH=\"${CLITK_DATA_PATH}/\")
#=========================================================
#=========================================================
# Get the data tests
IF(WIN32)
  SET(scriptExt bat)
ELSE(UNIX)
  SET(scriptExt sh)
ENDIF()
EXEC_PROGRAM(${CMAKE_CURRENT_SOURCE_DIR}/fetch_data_test.${scriptExt} ARGS ${CLITK_DATA_PATH})
#=========================================================
# Google test macro
macro(ADD_GOOGLE_TESTS executable)
  foreach ( source ${ARGN} )
    file(READ "${source}" contents)
    string(REGEX MATCHALL "TEST_?F?\\(([A-Za-z_0-9 ,]+)\\)" found_tests ${contents})
    foreach(hit ${found_tests})
      string(REGEX REPLACE ".*\\(([A-Za-z_0-9]+)[, ]*([A-Za-z_0-9]+)\\).*" "\\1.\\2" test_name ${hit})
      add_test(${test_name} ${executable} --gtest_filter=${test_name})
    endforeach(hit)
  endforeach()
endmacro()
#=========================================================
#=========================================================
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests/bin)
SET(BUILDNAME ${CMAKE_SYSTEM_NAME}${CMAKE_OSX_ARCHITECTURES} CACHE INTERNAL DOCSTRING)
#=========================================================
#=========================================================
# GoogleTest framemwork
FIND_PATH(GTEST_DIR gtest)
INCLUDE_DIRECTORIES(${GTEST_DIR}/include/)

FIND_LIBRARY(GTEST_LIB NAMES gtest PATHS ${GTEST_DIR}/build/)
ADD_LIBRARY(gtest UNKNOWN IMPORTED)
SET_PROPERTY(TARGET gtest PROPERTY IMPORTED_LOCATION "${GTEST_LIB}")
#=========================================================
#=========================================================
# Enabled options to test
IF(CLITK_BUILD_TOOLS)
  ADD_SUBDIRECTORY(tools)
ENDIF()
IF(CLITK_BUILD_VV)
  ADD_SUBDIRECTORY(vv)
ENDIF()
# IF(CLITK_BUILD_SEGMENTATION)
#   ADD_SUBDIRECTORY(segmentation)
# ENDIF()
# 
# IF(CLITK_BUILD_REGISTATION)
#   ADD_SUBDIRECTORY(registration)
# ENDIF()
#ADD_SUBDIRECTORY(common)
#=========================================================

