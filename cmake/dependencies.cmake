#=========================================================
# Find ITK (required)
find_package(ITK)
if(ITK_FOUND)
  include("${ITK_USE_FILE}")
else(ITK_FOUND)
  message(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
endif(ITK_FOUND)
#=========================================================

#=========================================================
# Find VTK (required)
find_package(VTK REQUIRED)
if(VTK_FOUND)
  include("${VTK_USE_FILE}")
  if(VTK_VERSION VERSION_LESS 5.8.0)
    set( VTK_LIBRARIES
      vtkCommon
      vtkRendering
      vtkIO
      vtkFiltering
      vtkGraphics
      vtkWidgets
      vtkImaging
      vtkHybrid
      )
  endif(VTK_VERSION VERSION_LESS 5.8.0)
  if(VTK_VERSION VERSION_LESS 5.6.0)
    set( VTK_LIBRARIES
      ${VTK_LIBRARIES}
      vtkQVTK
    )
  endif(VTK_VERSION VERSION_LESS 5.6.0)
else(VTK_FOUND)
  message(FATAL_ERROR "Please set VTK_DIR.")
endif(VTK_FOUND)
#=========================================================

#=========================================================
# Find gengetopt, will create a target exe if not found
set(CMAKE_MODULE_PATH "${CLITK_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Gengetopt)
#=========================================================

#=========================================================
# Find libstatgrab is installed, add clitkMemoryUsage.cxx in the library
if(NOT DEFINED CLITK_MEMORY_INFO OR CLITK_MEMORY_INFO)
  find_library(LIBSTATGRAB NAMES statgrab PATHS)
  if(${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
#  message("Install libstatgrab (http://www.i-scream.org/libstatgrab/) for memory usage information")
    set(CLITK_MEMORY_INFO OFF)
  else(${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
    set(CLITK_MEMORY_INFO ON)
  endif(${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
endif()
#=========================================================


#=========================================================
### Check if ITK was compiled with SYSTEM_GDCM = ON
set(CLITK_USE_SYSTEM_GDCM FALSE)
if(ITK_VERSION_MAJOR LESS "4")
  if(ITK_USE_SYSTEM_GDCM)
    set(CLITK_USE_SYSTEM_GDCM TRUE)
  endif(ITK_USE_SYSTEM_GDCM)
else()
  # ITK4 creates a target for each gdcm library when it compiles GDCM
  get_target_property(GDCMDICTTARG gdcmDICT TYPE )
  if(NOT GDCMDICTTARG)
    set(CLITK_USE_SYSTEM_GDCM TRUE)
  endif()
endif()

