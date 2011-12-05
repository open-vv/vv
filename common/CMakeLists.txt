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
  clitkImageToImageGenericFilterBase.cxx
  clitkExceptionObject.cxx
  clitkFilterBase.cxx
  clitkMemoryUsage.cxx
  vvImage.cxx
  vvImageReader.cxx
  vvImageWriter.cxx
)

IF(UNIX)
  SET(clitkCommon_SRC
    ${clitkCommon_SRC}
    clitkGateAsciiImageIO.cxx
    clitkGateAsciiImageIOFactory.cxx
  )
ENDIF(UNIX)

### Declare clitkCommon library
ADD_LIBRARY(clitkCommon STATIC ${clitkCommon_SRC})

IF(NOT ${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
  TARGET_LINK_LIBRARIES(clitkCommon statgrab)
ENDIF(NOT ${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")

TARGET_LINK_LIBRARIES(clitkCommon vtkCommon vtkImaging ${ITK_LIBRARIES})

ADD_LIBRARY(clitkDicomRTStruct STATIC
  clitkDicomRT_Contour.cxx
  clitkDicomRT_ROI.cxx
  clitkDicomRT_StructureSet.cxx
  clitkDicomRTStruct2ImageFilter.cxx
)

TARGET_LINK_LIBRARIES(clitkDicomRTStruct vtkHybrid) 

#ADD_LIBRARY(clitkCommonShared SHARED ${clitkCommon_SRC})
#SET_TARGET_PROPERTIES(clitkCommonShared PROPERTIES COMPILE_FLAGS -fPIC)

