#=========================================================
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${ITK_DIR}/gdcm/src/")

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
  clitkOrientation.cxx
  vvImage.cxx
  clitkImageToImageGenericFilterBase.cxx
  clitkFilterBase.cxx
)  

ADD_LIBRARY(clitkCommon STATIC ${clitkCommon_SRC})
TARGET_LINK_LIBRARIES(clitkCommon vtkCommon vtkImaging vtkHybrid ITKBasicFilters)

ADD_LIBRARY(clitkDicomRTStruct STATIC
  clitkDicomRT_Contour.cxx
  clitkDicomRT_ROI.cxx
  clitkDicomRT_StructureSet.cxx
  clitkDicomRT_ROI_ConvertToImageFilter.cxx
)

#ADD_LIBRARY(clitkCommonShared SHARED ${clitkCommon_SRC})
#SET_TARGET_PROPERTIES(clitkCommonShared PROPERTIES COMPILE_FLAGS -fPIC)
