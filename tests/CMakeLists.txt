# Tests are organized to mirror the clitk directory tree. 
# Eacho subdirectory has its own CMakeLists.txt with the compiling directives

IF (CLITK_BUILD_TESTING)

  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests/bin)

  # default data path is searched in the project source tree
  FIND_PATH(CLITK_DATA_PATH data PATHS ${PROJECT_SOURCE_DIR})
  IF (CLITK_DATA_PATH STREQUAL "CLITK_DATA_PATH-NOTFOUND" OR CLITK_DATA_PATH STREQUAL "")
    MESSAGE("Data path must be given when tests are enabled.")
  ENDIF (CLITK_DATA_PATH STREQUAL "CLITK_DATA_PATH-NOTFOUND" OR CLITK_DATA_PATH STREQUAL "")

  set(CLITK_DATA_PATH ${CLITK_DATA_PATH}/data)
  ADD_DEFINITIONS(-DCLITK_DATA_PATH='"${CLITK_DATA_PATH}"')

  ADD_SUBDIRECTORY(tools)
  #ADD_SUBDIRECTORY(segmentation)
  #ADD_SUBDIRECTORY(registration)
  #ADD_SUBDIRECTORY(common)

ENDIF(CLITK_BUILD_TESTING)
