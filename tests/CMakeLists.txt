cmake_policy(SET CMP0005 NEW)
#=========================================================
#=========================================================
# Data tests path
find_path(CLITK_TEST_DATA_PATH Lung3D.mhd)
if(NOT ${CLITK_TEST_DATA_PATH} MATCHES "NOTFOUND$")
 set(CLITK_TEST_DATA_PATH "${CLITK_TEST_DATA_PATH}/")
endif()
add_definitions(-DCLITK_TEST_DATA_PATH="${CLITK_TEST_DATA_PATH}")
#=========================================================
#=========================================================
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests/bin)
set(BUILDNAME ${CMAKE_SYSTEM_NAME}${CMAKE_OSX_ARCHITECTURES} CACHE INTERNAL DOCSTRING)
#=========================================================
#=========================================================
# Enabled options to test
if(CLITK_BUILD_TOOLS)
  add_subdirectory(tools)
endif()
#if(CLITK_BUILD_VV)
#  add_subdirectory(vv)
#endif()
# if(CLITK_BUILD_SEGMENTATION)
#   add_subdirectory(segmentation)
# endif()
#
# if(CLITK_BUILD_REGISTATION)
#   add_subdirectory(registration)
# endif()
#add_subdirectory(common)
#=========================================================

