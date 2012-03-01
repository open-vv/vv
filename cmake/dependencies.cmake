#=========================================================
# Find ITK (required)
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE("${ITK_USE_FILE}")
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)
#=========================================================

#=========================================================
# Find VTK (required)
FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
  INCLUDE("${VTK_USE_FILE}")
  IF(VTK_VERSION VERSION_LESS 5.8.0)
    SET ( VTK_LIBRARIES
      vtkCommon
      vtkRendering
      vtkIO
      vtkFiltering
      vtkGraphics
      vtkWidgets
      vtkImaging
      vtkHybrid
      vtkQVTK
      )
  ENDIF(VTK_VERSION VERSION_LESS 5.8.0)
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(VTK_FOUND)
#=========================================================

#=========================================================
# Find gengetopt, will create a target exe if not found
SET(CMAKE_MODULE_PATH "${CLITK_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
FIND_PACKAGE(Gengetopt)
#=========================================================

#=========================================================
# Find libstatgrab is installed, add clitkMemoryUsage.cxx in the library
FIND_LIBRARY(LIBSTATGRAB NAMES statgrab PATHS)
IF (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
#  MESSAGE("Install libstatgrab (http://www.i-scream.org/libstatgrab/) for memory usage information")
  SET(CLITK_MEMORY_INFO OFF)
ELSE (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
  LINK_DIRECTORIES(${LIBSTATGRAB}/../lib)
  LINK_DIRECTORIES(${LIBSTATGRAB}/../lib64)
  INCLUDE_DIRECTORIES(${LIBSTATGRAB}/../include)
  SET(CLITK_MEMORY_INFO ON)
ENDIF (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")  
#=========================================================
