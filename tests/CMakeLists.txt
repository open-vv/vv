# Tests are organized to mirror the clitk directory tree. 
# Eacho subdirectory has its own CMakeLists.txt with the compiling directives

IF (BUILD_TESTING)

  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests/bin)

  FIND_PATH(CLITK_DATA_PATH data)
  IF (CLITK_DATA_PATH STREQUAL "CLITK_DATA_PATH-NOTFOUND")
    MESSAGE(FATAL_ERROR "Data path must be given when tests are enabled.")
  ENDIF (CLITK_DATA_PATH STREQUAL "CLITK_DATA_PATH-NOTFOUND")

  ADD_DEFINITIONS(-DCLITK_DATA_PATH='"${CLITK_DATA_PATH}"')

  ADD_SUBDIRECTORY(tools)
  #ADD_SUBDIRECTORY(segmentation)
  #ADD_SUBDIRECTORY(registration)
  #ADD_SUBDIRECTORY(common)

ENDIF(BUILD_TESTING)
