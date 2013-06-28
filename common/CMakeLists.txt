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
  clitkGateAsciiImageIO.cxx
  clitkGateAsciiImageIOFactory.cxx
  clitkVoxImageIO.cxx  
  clitkVoxImageIOFactory.cxx
  clitkVfImageIO.cxx  
  clitkVfImageIOFactory.cxx
  clitkXdrImageIOReader.cxx  
  clitkXdrImageIOWriter.cxx  
  clitkXdrImageIOFactory.cxx
  rtkHisImageIO.cxx  
  rtkHisImageIOFactory.cxx
  rtkHndImageIO.cxx
  rtkHndImageIOFactory.cxx
  rtkEdfImageIO.cxx
  rtkEdfImageIOFactory.cxx
  rtkXRadImageIO.cxx
  rtkXRadImageIOFactory.cxx
  rtkImagXImageIO.cxx
  rtkImagXImageIOFactory.cxx
  rtkImagXXMLFileReader.cxx
  clitkEsrfHstImageIO.cxx  
  clitkEsrfHstImageIOFactory.cxx
  clitkEsrfHstXMLFileReader.cxx
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
IF(CLITK_PRIVATE_FEATURES)
  SET(clitkCommon_SRC ${clitkCommon_SRC}
    ${PROJECT_SOURCE_DIR}/private_features/clitkUsfImageIO.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkUsfImageIOFactory.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkUSVoxImageIO.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkUSVoxImageIOFactory.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkSvlImageIO.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkSvlImageIOFactory.cxx
    )
ENDIF(CLITK_PRIVATE_FEATURES)

### Declare clitkCommon library
ADD_LIBRARY(clitkCommon STATIC ${clitkCommon_SRC})

IF(CLITK_MEMORY_INFO)
  TARGET_LINK_LIBRARIES(clitkCommon statgrab)
ENDIF(CLITK_MEMORY_INFO)


#=========================================================
IF (CLITK_USE_SYSTEM_GDCM)
  FIND_PACKAGE(GDCM REQUIRED)
  include(${GDCM_USE_FILE})
  TARGET_LINK_LIBRARIES(clitkCommon vtkgdcm)
  # gdcmDICT gdcmMSFF)
ENDIF()
#=========================================================

TARGET_LINK_LIBRARIES(clitkCommon ${VTK_LIBRARIES} ${ITK_LIBRARIES})

ADD_LIBRARY(clitkDicomRTStruct STATIC
  clitkDicomRT_Contour.cxx
  clitkDicomRT_ROI.cxx
  clitkDicomRT_StructureSet.cxx
  clitkDicomRTStruct2ImageFilter.cxx
  )

TARGET_LINK_LIBRARIES(clitkDicomRTStruct clitkCommon)

#ADD_LIBRARY(clitkCommonShared SHARED ${clitkCommon_SRC})
#SET_TARGET_PROPERTIES(clitkCommonShared PROPERTIES COMPILE_FLAGS -fPIC)

