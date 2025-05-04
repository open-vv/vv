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
mark_as_advanced(CLITK_EXPERIMENTAL)
option(CLITK_MEMORY_INFO "Enable memory info (need libstatgrab >= v.0.90)" OFF)
mark_as_advanced(CLITK_MEMORY_INFO)
option(CLITK_EXTERNAL_GDCM "Enable features using external GDCM" OFF)
mark_as_advanced(CLITK_EXTERNAL_GDCM)
option(CLITK_BUILD_TOOLS "Build command-line tools" OFF)
option(CLITK_BUILD_SEGMENTATION "Build command-line segmentation tools" OFF)
option(CLITK_BUILD_REGISTRATION "Build command-line registration tools" OFF)

option(CLITK_BUILD_VV "Build vv the 4D visualizer (requires VTK and QT)" ON)

if(CLITK_BUILD_VV)
 #if(VTK_VERSION VERSION_LESS 6.0.0)
 #   set(vv_QT_VERSION "4" CACHE INTERNAL "Expected Qt version")
 #else()
 #   if(VTK_QT_VERSION VERSION_LESS 5)
 #      set(vv_QT_VERSION "4" CACHE INTERNAL "Expected Qt version")
 #   else()
 #      set(vv_QT_VERSION "5" CACHE INTERNAL "Expected Qt version")
 #   endif()
 #endif()
 # set(vv_QT_VERSION "5" CACHE INTERNAL "Expected Qt version")
 #find_package(Qt4)
 #find_package(Qt5 COMPONENTS Core)
 if(VTK_QT_VERSION VERSION_LESS 5)
   set(vv_QT_VERSION "4" CACHE INTERNAL "Expected Qt version")
 elseif(VTK_QT_VERSION VERSION_LESS 6)
   set(vv_QT_VERSION "5" CACHE INTERNAL "Expected Qt version")
 else()
   message(FATAL_ERROR "VTK needs to be compiled with Qt4 or Qt5 is needed if you want to build vv.")
 endif()
 add_subdirectory(${CLITK_SOURCE_DIR}/vv ${PROJECT_BINARY_DIR}/vv)
endif(CLITK_BUILD_VV)

#=========================================================
# Build test when vv has been compiled
if(BUILD_TESTING)
  add_subdirectory(${CLITK_SOURCE_DIR}/tests ${PROJECT_BINARY_DIR}/tests)
endif(BUILD_TESTING)


