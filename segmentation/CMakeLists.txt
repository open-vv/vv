#=========================================================

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

#=========================================================

SET( CMAKE_VERBOSE_MAKEFILE off)

#=========================================================

# WRAP_GGO(clitkSegmentationFilters_GGO_C
#   clitkFillMask.ggo
#   clitkConnectedComponentLabeling.ggo
#   clitkExtractLung.ggo
#   clitkExtractPatient.ggo
#   clitkExtractBones.ggo
#   clitkExtractMediastinum.ggo
#   clitkExtractLymphStations.ggo
# )

# SET(clitkSegmentationFilters_SRC
#   ${clitkSegmentationFilters_GGO_C}
# )  

# ADD_LIBRARY(clitkSegmentationFilters STATIC ${clitkSegmentationFilters_SRC})

# ADD_EXECUTABLE(clitkTestFilter clitkTestFilter.cxx clitkTestFilter_ggo.c)
# TARGET_LINK_LIBRARIES(clitkTestFilter clitkCommon ITKIO clitkSegmentationFilters ITKStatistics)

######################################### Segmentation part
WRAP_GGO(clitkConnectedComponentLabeling_GGO_C clitkConnectedComponentLabeling.ggo)
ADD_EXECUTABLE(clitkConnectedComponentLabeling clitkConnectedComponentLabeling.cxx ${clitkConnectedComponentLabeling_GGO_C})
TARGET_LINK_LIBRARIES(clitkConnectedComponentLabeling clitkCommon ITKIO)

WRAP_GGO(clitkFillMask_GGO_C clitkFillMask.ggo)
ADD_EXECUTABLE(clitkFillMask clitkFillMask.cxx clitkFillMaskGenericFilter.cxx ${clitkFillMask_GGO_C})
TARGET_LINK_LIBRARIES(clitkFillMask clitkCommon ITKIO)

WRAP_GGO(clitkExtractPatient_GGO_C clitkExtractPatient.ggo)
ADD_EXECUTABLE(clitkExtractPatient clitkExtractPatient.cxx ${clitkExtractPatient_GGO_C})
TARGET_LINK_LIBRARIES(clitkExtractPatient clitkCommon ITKIO)

WRAP_GGO(clitkExtractLung_GGO_C clitkExtractLung.ggo)
ADD_EXECUTABLE(clitkExtractLung clitkExtractLung.cxx ${clitkExtractLung_GGO_C})
TARGET_LINK_LIBRARIES(clitkExtractLung clitkCommon ITKIO)

WRAP_GGO(clitkExtractBones_GGO_C clitkExtractBones.ggo)
ADD_EXECUTABLE(clitkExtractBones clitkExtractBones.cxx ${clitkExtractBones_GGO_C})
TARGET_LINK_LIBRARIES(clitkExtractBones clitkCommon ITKIO)

WRAP_GGO(clitkExtractMediastinum_GGO_C clitkExtractMediastinum.ggo)
ADD_EXECUTABLE(clitkExtractMediastinum clitkExtractMediastinum.cxx ${clitkExtractMediastinum_GGO_C})
TARGET_LINK_LIBRARIES(clitkExtractMediastinum clitkCommon ITKIO )

WRAP_GGO(clitkExtractLymphStations_GGO_C clitkExtractLymphStations.ggo)
ADD_EXECUTABLE(clitkExtractLymphStations clitkExtractLymphStations.cxx ${clitkExtractLymphStations_GGO_C})
TARGET_LINK_LIBRARIES(clitkExtractLymphStations clitkCommon ITKIO )

WRAP_GGO(clitkMorphoMath_GGO_C clitkMorphoMath.ggo)
ADD_EXECUTABLE(clitkMorphoMath clitkMorphoMath.cxx clitkMorphoMathGenericFilter.cxx ${clitkMorphoMath_GGO_C})
TARGET_LINK_LIBRARIES(clitkMorphoMath ITKIO clitkCommon)

WRAP_GGO(clitkMorphoReconstruction_GGO_C clitkMorphoReconstruction.ggo)
ADD_EXECUTABLE(clitkMorphoReconstruction clitkMorphoReconstruction.cxx clitkMorphoReconstructionGenericFilter.cxx ${clitkMorphoReconstruction_GGO_C})
TARGET_LINK_LIBRARIES(clitkMorphoReconstruction clitkCommon ITKIO )

WRAP_GGO(clitkCalculateDistanceMap_GGO_C clitkCalculateDistanceMap.ggo)
ADD_EXECUTABLE(clitkCalculateDistanceMap clitkCalculateDistanceMap.cxx clitkCalculateDistanceMapGenericFilter.cxx ${clitkCalculateDistanceMap_GGO_C})
TARGET_LINK_LIBRARIES(clitkCalculateDistanceMap clitkCommon ITKIO)

WRAP_GGO(clitkLevelSetSegmentation_GGO_C clitkLevelSetSegmentation.ggo)
ADD_EXECUTABLE(clitkLevelSetSegmentation clitkLevelSetSegmentation.cxx clitkLevelSetSegmentationGenericFilter.cxx ${clitkLevelSetSegmentation_GGO_C})
TARGET_LINK_LIBRARIES(clitkLevelSetSegmentation clitkCommon ITKIO)

WRAP_GGO(clitkAnd_GGO_C clitkAnd.ggo)
ADD_EXECUTABLE(clitkAnd clitkAnd.cxx clitkAndGenericFilter.cxx ${clitkAnd_GGO_C})
TARGET_LINK_LIBRARIES(clitkAnd clitkCommon ITKIO )

WRAP_GGO(clitkRegionGrowing_GGO_C clitkRegionGrowing.ggo)
ADD_EXECUTABLE(clitkRegionGrowing clitkRegionGrowing.cxx clitkRegionGrowingGenericFilter.cxx ${clitkRegionGrowing_GGO_C})
TARGET_LINK_LIBRARIES(clitkRegionGrowing clitkCommon ITKIO)

WRAP_GGO(clitkDecomposeAndReconstruct_GGO_C clitkDecomposeAndReconstruct.ggo)
ADD_EXECUTABLE(clitkDecomposeAndReconstruct clitkDecomposeAndReconstruct.cxx clitkDecomposeAndReconstructGenericFilter.cxx ${clitkDecomposeAndReconstruct_GGO_C})
TARGET_LINK_LIBRARIES(clitkDecomposeAndReconstruct clitkCommon ITKIO)

WRAP_GGO(clitkMotionMask_GGO_C clitkMotionMask.ggo)
ADD_EXECUTABLE(clitkMotionMask clitkMotionMask.cxx clitkMotionMaskGenericFilter.cxx ${clitkMotionMask_GGO_C})
TARGET_LINK_LIBRARIES(clitkMotionMask clitkCommon ITKIO ITKStatistics )

WRAP_GGO(clitkFillImageRegion_GGO_C clitkFillImageRegion.ggo)
ADD_EXECUTABLE(clitkFillImageRegion clitkFillImageRegion.cxx clitkFillImageRegionGenericFilter.cxx ${clitkFillImageRegion_GGO_C})
TARGET_LINK_LIBRARIES(clitkFillImageRegion clitkCommon ITKIO)

