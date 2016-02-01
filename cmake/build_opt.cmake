#=========================================================
#Support for the CTest dashboard testing system
option(BUILD_TESTING "Build the testing tree" OFF)
if(BUILD_TESTING)
  option(CLITK_BUILD_TESTING "Test ITK" ON)
  include(CTest)
endif(BUILD_TESTING)
#=========================================================


#=========================================================
# If the user choose to build documentation, then search for Doxygen executables.
option(BUILD_DOXYGEN "Build Doxygen Documentation" OFF)
if(BUILD_DOXYGEN)
  find_package(Doxygen)
  add_subdirectory(${CLITK_SOURCE_DIR}/Doxygen ${PROJECT_BINARY_DIR}/Doxygen)
endif(BUILD_DOXYGEN)
#=========================================================

# Compilation options
option(CLITK_EXPERIMENTAL "Enable experimental software and features" OFF)
option(CLITK_BUILD_TOOLS "Build command-line tools" OFF)
option(CLITK_BUILD_SEGMENTATION "Build command-line segmentation tools" OFF)
option(CLITK_BUILD_REGISTRATION "Build command-line registration tools" OFF)

option(CLITK_BUILD_VV "Build vv the 4D visualizer (requires VTK and QT)" ON)

if(CLITK_BUILD_VV)
    set(vv_QT_VERSION "4" CACHE STRING "Expected Qt version")
    set_property(CACHE vv_QT_VERSION PROPERTY STRINGS 4 5)
    if(NOT (vv_QT_VERSION VERSION_EQUAL "4" OR vv_QT_VERSION VERSION_EQUAL "5"))
      message(FATAL_ERROR "Expected value for vv_QT_VERSION is either '4' or '5'")
    endif()

  add_subdirectory(${CLITK_SOURCE_DIR}/vv ${PROJECT_BINARY_DIR}/vv)
endif(CLITK_BUILD_VV)

#=========================================================
# Build test when vv has been compiled
if(BUILD_TESTING)
  add_subdirectory(${CLITK_SOURCE_DIR}/tests ${PROJECT_BINARY_DIR}/tests)
endif(BUILD_TESTING)


