#=========================================================
MACRO (DD in)
    MESSAGE(${in}=${${in}})
ENDMACRO(DD)
#=========================================================

#=========================================================
#Set a reasonable build mode default if the user hasn't set any
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

#=========================================================
INCLUDE_DIRECTORIES(${CLITK_SOURCE_DIR}/itk
  ${CLITK_SOURCE_DIR}/filters
  ${CLITK_SOURCE_DIR}/segmentation
  ${CLITK_SOURCE_DIR}/registration
  ${CLITK_SOURCE_DIR}/tools
  ${CLITK_SOURCE_DIR}/common
  ${PROJECT_BINARY_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/tools)         #For _ggo.h includes from other directories
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/segmentation)  #For _ggo.h includes from other directories
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/registration)  #For _ggo.h includes from other directories
#=========================================================

#=========================================================
# Building in the source tree is forbidden
IF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
  MESSAGE(FATAL_ERROR "Building in the source tree is not allowed ! Quit; remove the file 'CMakeCache.txt' and the folder 'CMakeFiles' an
d build outside the sources (for example 'mkdir build ; cmake <CLITK_DIR>'.")
ENDIF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
#=========================================================

#=========================================================
# Remove some MS Visual c++ flags
IF(MSVC)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
ENDIF(MSVC)
#=========================================================
