include_directories(
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/tools
  ${GTEST_DIR}/include
)

FILE(GLOB srcs *.cxx)
ADD_EXECUTABLE(toolsTest ${srcs})
ADD_DEFINITIONS(-DTOOLS_PATH=\"${PROJECT_BINARY_DIR}/bin/\")
target_link_libraries(toolsTest vvLib ${vvExternalLibs} gtest)

# Add all tests found in the source code, calling the executable to run them
add_google_tests ( ${EXECUTABLE_OUTPUT_PATH}/toolsTest ${srcs})

SET(BUILDNAME ${BUILDNAME}_tools CACHE INTERNAL DOCSTRING)