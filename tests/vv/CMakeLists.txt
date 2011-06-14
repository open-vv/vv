include_directories(
  ${PROJECT_SOURCE_DIR}/vv
  ${PROJECT_BINARY_DIR}/vv
  ${QT_QTGUI_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}
  ${QT_QTNETWORK_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}/../
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/tools
  ${PROJECT_SOURCE_DIR}/segmentation
  ${QT_LIBRARY_DIR}
  ${GTEST_DIR}/include
)
FILE(GLOB srcs *.cxx)
ADD_EXECUTABLE(vvTest ${srcs})
target_link_libraries(vvTest vvLib gtest)

# Add all tests found in the source code, calling the executable to run them
add_google_tests ( ${EXECUTABLE_OUTPUT_PATH}/vvTest ${srcs})
SET(BUILDNAME ${BUILDNAME}_vv CACHE INTERNAL DOCSTRING)
