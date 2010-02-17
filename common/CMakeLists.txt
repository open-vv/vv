#=========================================================
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${ITK_DIR}/gdcm/src/")

#=========================================================
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
#=========================================================

#=========================================================
# make clitk libraries

SET(clitkCommon_SRC
  clitkCommon.cxx 
  clitkListOfPair.cxx
  clitkTimer.cxx
  clitkImageCommon.cxx
  clitkVoxImageIO.cxx  
  clitkVoxImageIOFactory.cxx
  clitkVfImageIO.cxx  
  clitkVfImageIOFactory.cxx
  clitkHisImageIO.cxx  
  clitkHisImageIOFactory.cxx
  clitkOrientation.cxx
  vvImage.cxx
  clitkImageToImageGenericFilter.cxx
  )  

ADD_LIBRARY(clitkCommon STATIC ${clitkCommon_SRC})

#ADD_LIBRARY(clitkCommonShared SHARED ${clitkCommon_SRC})
#SET_TARGET_PROPERTIES(clitkCommonShared PROPERTIES COMPILE_FLAGS -fPIC)
