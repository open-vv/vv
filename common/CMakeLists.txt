#=========================================================
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
#=========================================================

#=========================================================
# make clitk libraries*
CONFIGURE_FILE(clitkConfiguration.h.in ${PROJECT_BINARY_DIR}/clitkConfiguration.h)

SET(clitkCommon_SRC
  clitkCommon.cxx 
  clitkListOfPair.cxx
  clitkTimer.cxx
  clitkImageCommon.cxx
  clitkTransformUtilities.cxx
  clitkIO.cxx
  clitkVoxImageIO.cxx  
  clitkVoxImageIOFactory.cxx
  clitkVfImageIO.cxx  
  clitkVfImageIOFactory.cxx
  clitkHisImageIO.cxx  
  clitkHisImageIOFactory.cxx
  clitkXdrImageIOReader.cxx  
  clitkXdrImageIOWriter.cxx  
  clitkXdrImageIOFactory.cxx
  clitkHndImageIO.cxx
  clitkHndImageIOFactory.cxx
  clitkDicomRTDoseIO.cxx
  clitkDicomRTDoseIOFactory.cxx
  clitkOrientation.cxx
  vvImage.cxx
  clitkImageToImageGenericFilterBase.cxx
  clitkExceptionObject.cxx
  clitkFilterBase.cxx
  clitkMemoryUsage.cxx
)  

### Declare clitkCommon library
ADD_LIBRARY(clitkCommon STATIC ${clitkCommon_SRC})

### if LIBSTATGRAB is installed, add clitkMemoryUsage.cxx in the library
IF(CLITK_MEMORY_INFO)
  FIND_PROGRAM(LIBSTATGRAB NAMES statgrab PATHS)
  IF (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
    MESSAGE( FATAL_ERROR "libstatgrab NOT FOUND -> please install http://www.i-scream.org/libstatgrab/")
  ELSE (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
    TARGET_LINK_LIBRARIES(clitkCommon statgrab)
  ENDIF (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")  
ENDIF(CLITK_MEMORY_INFO)

TARGET_LINK_LIBRARIES(clitkCommon vtkCommon vtkImaging vtkHybrid ITKBasicFilters)

ADD_LIBRARY(clitkDicomRTStruct STATIC
  clitkDicomRT_Contour.cxx
  clitkDicomRT_ROI.cxx
  clitkDicomRT_StructureSet.cxx
  clitkDicomRT_ROI_ConvertToImageFilter.cxx
)

#ADD_LIBRARY(clitkCommonShared SHARED ${clitkCommon_SRC})
#SET_TARGET_PROPERTIES(clitkCommonShared PROPERTIES COMPILE_FLAGS -fPIC)
