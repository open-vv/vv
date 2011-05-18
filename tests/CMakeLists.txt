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
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests/bin)
SET(BUILDNAME ${CMAKE_SYSTEM_NAME}${CMAKE_OSX_ARCHITECTURES} CACHE INTERNAL DOCSTRING)
SET(CLITK_DATA_PATH ${PROJECT_BINARY_DIR}/tests/data/ CACHE STRING "dataTest path")
#todo  test if data test path exists

ADD_DEFINITIONS(-DCLITK_DATA_PATH=\"${CLITK_DATA_PATH}\")

add_library(gtest UNKNOWN IMPORTED)
set(GTEST_DIR ${ITK_DIR}/../gtest/ CACHE STRING "gtestDir")
#if windows gtestLibName=gtest.dll else libgtest.a
set(gtestLibName libgtest.a)
set(GTEST_LIB ${GTEST_DIR}/build/${gtestLibName})
#message(gtestlib name = ${GTEST_LIB})
set_property(TARGET gtest PROPERTY IMPORTED_LOCATION "${GTEST_LIB}")
include_directories(${GTEST_DIR}/include/)

IF(CLITK_BUILD_TOOLS)
  ADD_SUBDIRECTORY(tools)
ENDIF()
IF(CLITK_BUILD_VV)
  ADD_SUBDIRECTORY(vv)
ENDIF()
#   ADD_SUBDIRECTORY(segmentation)
#   ADD_SUBDIRECTORY(registration)
#   ADD_SUBDIRECTORY(common)


