#=========================================================

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

#=========================================================

SET( CMAKE_VERBOSE_MAKEFILE off)

#=========================================================

WRAP_GGO(clitkSegmentationFilters_GGO_C
  clitkFillMask.ggo
  clitkConnectedComponentLabeling.ggo
  clitkExtractLung.ggo
  clitkExtractPatient.ggo
  clitkExtractBones.ggo
  clitkExtractMediastinum.ggo
  clitkExtractLymphStations.ggo
)

SET(clitkSegmentationFilters_SRC
  ${clitkSegmentationFilters_GGO_C}
)  

ADD_LIBRARY(clitkSegmentationFilters STATIC ${clitkSegmentationFilters_SRC})

ADD_EXECUTABLE(clitkTestFilter clitkTestFilter.cxx clitkTestFilter_ggo.c)
TARGET_LINK_LIBRARIES(clitkTestFilter clitkCommon ITKIO clitkSegmentationFilters ITKStatistics)

## Segmentation part

ADD_EXECUTABLE(clitkConnectedComponentLabeling clitkConnectedComponentLabeling.cxx)
TARGET_LINK_LIBRARIES(clitkConnectedComponentLabeling clitkCommon ITKIO ITKStatistics clitkSegmentationFilters)

ADD_EXECUTABLE(clitkFillMask clitkFillMask.cxx clitkFillMaskGenericFilter.cxx)
TARGET_LINK_LIBRARIES(clitkFillMask clitkCommon ITKIO ITKStatistics clitkSegmentationFilters)

ADD_EXECUTABLE(clitkExtractPatient clitkExtractPatient.cxx)
TARGET_LINK_LIBRARIES(clitkExtractPatient clitkCommon ITKIO ITKStatistics clitkSegmentationFilters)

ADD_EXECUTABLE(clitkExtractLung clitkExtractLung.cxx)
TARGET_LINK_LIBRARIES(clitkExtractLung clitkCommon ITKIO ITKStatistics clitkSegmentationFilters)

ADD_EXECUTABLE(clitkExtractBones clitkExtractBones.cxx)
TARGET_LINK_LIBRARIES(clitkExtractBones clitkCommon ITKIO ITKStatistics clitkSegmentationFilters)

ADD_EXECUTABLE(clitkExtractMediastinum clitkExtractMediastinum.cxx)
TARGET_LINK_LIBRARIES(clitkExtractMediastinum clitkCommon ITKIO ITKStatistics clitkSegmentationFilters)

ADD_EXECUTABLE(clitkExtractLymphStations clitkExtractLymphStations.cxx)
TARGET_LINK_LIBRARIES(clitkExtractLymphStations clitkCommon ITKIO ITKStatistics clitkSegmentationFilters)
