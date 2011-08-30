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
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/common)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/tools)         #For _ggo.h includes from other directories
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/segmentation)  #For _ggo.h includes from other directories
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/registration)  #For _ggo.h includes from other directories
#=========================================================
