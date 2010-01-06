#=========================================================
# Compiled Image types
# Comment/uncomment desired image types

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGETYPE_CHAR")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGETYPE_UCHAR")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGETYPE_SHORT")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGETYPE_USHORT")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGETYPE_INT")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGETYPE_UINT")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGETYPE_FLOAT")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGETYPE_DOUBLE")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${ITK_DIR}/gdcm/src/")

#=========================================================
# Compiled Image Dimension 
# Comment/uncomment desired image dimensions

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGEDIM_1")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGEDIM_2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGEDIM_3")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DIMAGEDIM_4")

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
  clitkOrientation.cxx
  clitkSignal.cxx
  vvImage.cxx
  clitkImageToImageGenericFilter.cxx
  )  

ADD_LIBRARY(clitkCommon STATIC ${clitkCommon_SRC})

#ADD_LIBRARY(clitkCommonShared SHARED ${clitkCommon_SRC})
#SET_TARGET_PROPERTIES(clitkCommonShared PROPERTIES COMPILE_FLAGS -fPIC)
