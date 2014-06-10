#=========================================================
# make clitk libraries*
configure_file(clitkConfiguration.h.in ${PROJECT_BINARY_DIR}/clitkConfiguration.h)

set(clitkCommon_SRC
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
  clitkMatrix.cxx
  vvImage.cxx
  vvImageReader.cxx
  vvImageWriter.cxx
)
if(CLITK_PRIVATE_FEATURES)
  set(clitkCommon_SRC ${clitkCommon_SRC}
    ${PROJECT_SOURCE_DIR}/private_features/clitkUsfImageIO.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkUsfImageIOFactory.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkUSVoxImageIO.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkUSVoxImageIOFactory.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkSvlImageIO.cxx
    ${PROJECT_SOURCE_DIR}/private_features/clitkSvlImageIOFactory.cxx
    )
endif(CLITK_PRIVATE_FEATURES)

### Declare clitkCommon library
add_library(clitkCommon STATIC ${clitkCommon_SRC})

if(CLITK_MEMORY_INFO)
  target_link_libraries(clitkCommon statgrab)
endif(CLITK_MEMORY_INFO)


#=========================================================
if(CLITK_USE_SYSTEM_GDCM)
  find_package(GDCM REQUIRED)
  include(${GDCM_USE_FILE})
  target_link_libraries(clitkCommon vtkgdcm gdcmDICT gdcmMSFF)
endif()
#=========================================================

target_link_libraries(clitkCommon ${VTK_LIBRARIES} ${ITK_LIBRARIES})

add_library(clitkDicomRTStruct STATIC
  clitkDicomRT_Contour.cxx
  clitkDicomRT_ROI.cxx
  clitkDicomRT_StructureSet.cxx
  clitkDicomRTStruct2ImageFilter.cxx
  )

target_link_libraries(clitkDicomRTStruct clitkCommon)

#add_library(clitkCommonShared SHARED ${clitkCommon_SRC})
#set_target_properties(clitkCommonShared PROPERTIES COMPILE_FLAGS -fPIC)

