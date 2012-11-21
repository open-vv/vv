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

IF(NOT ${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
  TARGET_LINK_LIBRARIES(clitkCommon statgrab)
ENDIF(NOT ${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")

### Check if ITK was compiled with SYSTEM_GDCM = ON and set the gdcm libraries accordingly
SET(CLITK_USE_SYSTEM_GDCM FALSE)
IF(ITK_VERSION_MAJOR LESS "4")
  IF(ITK_USE_SYSTEM_GDCM)
    SET(CLITK_USE_SYSTEM_GDCM TRUE) 
  ENDIF(ITK_USE_SYSTEM_GDCM)
ELSE(ITK_VERSION_MAJOR LESS "4")
  SET(GDCM_INCLUDE_INSTALL ".*include/gdcm-.*")
  SET(GDCM_INCLUDE_BUILD ".*/Source/MediaStorageAndFileFormat")
  IF(ITKGDCM_INCLUDE_DIRS MATCHES ${GDCM_INCLUDE_BUILD} OR ITKGDCM_INCLUDE_DIRS MATCHES ${GDCM_INCLUDE_INSTALL})
    SET(CLITK_USE_SYSTEM_GDCM TRUE)
  ENDIF(ITKGDCM_INCLUDE_DIRS MATCHES ${GDCM_INCLUDE_BUILD} OR ITKGDCM_INCLUDE_DIRS MATCHES ${GDCM_INCLUDE_INSTALL})
ENDIF(ITK_VERSION_MAJOR LESS "4")

IF(CLITK_USE_SYSTEM_GDCM)
  SET(CLITK_GDCM_LIBRARIES gdcmDSED) 
ENDIF(CLITK_USE_SYSTEM_GDCM)

TARGET_LINK_LIBRARIES(clitkCommon vtkCommon vtkImaging ${ITK_LIBRARIES} ${CLITK_GDCM_LIBRARIES})

ADD_LIBRARY(clitkDicomRTStruct STATIC
  clitkDicomRT_Contour.cxx
  clitkDicomRT_ROI.cxx
  clitkDicomRT_StructureSet.cxx
  clitkDicomRTStruct2ImageFilter.cxx
)

TARGET_LINK_LIBRARIES(clitkDicomRTStruct vtkHybrid ${CLITK_GDCM_LIBRARIES}) 

#ADD_LIBRARY(clitkCommonShared SHARED ${clitkCommon_SRC})
#SET_TARGET_PROPERTIES(clitkCommonShared PROPERTIES COMPILE_FLAGS -fPIC)

