#=========================================================

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

#=========================================================

#Add your ggo files to this list if they are used by vv
FOREACH(clitkTool clitkExtractLung clitkExtractPatient)
    WRAP_GGO(${clitkTool}_GGO_C ${clitkTool}.ggo)
    SET(GGO_C_FILES ${GGO_C_FILES} ${${clitkTool}_GGO_C})
ENDFOREACH(clitkTool)
ADD_LIBRARY(clitkSegmentationGgoLib clitkFilterWithAnatomicalFeatureDatabaseManagement.cxx clitkAnatomicalFeatureDatabase.cxx ${GGO_C_FILES})

#=========================================================

IF(CLITK_BUILD_SEGMENTATION)
    WRAP_GGO(clitkConnectedComponentLabeling_GGO_C clitkConnectedComponentLabeling.ggo)
    ADD_EXECUTABLE(clitkConnectedComponentLabeling clitkConnectedComponentLabeling.cxx ${clitkConnectedComponentLabeling_GGO_C})
    TARGET_LINK_LIBRARIES(clitkConnectedComponentLabeling clitkCommon ITKIO)

    WRAP_GGO(clitkFillMask_GGO_C clitkFillMask.ggo)
    ADD_EXECUTABLE(clitkFillMask clitkFillMask.cxx ${clitkFillMask_GGO_C})
    TARGET_LINK_LIBRARIES(clitkFillMask clitkCommon ITKIO)

    ADD_EXECUTABLE(clitkExtractPatient clitkExtractPatient.cxx ${clitkExtractPatient_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractPatient clitkCommon ITKIO clitkSegmentationGgoLib)

    ADD_EXECUTABLE(clitkExtractLung clitkExtractLung.cxx  ${clitkExtractLung_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractLung clitkSegmentationGgoLib clitkCommon ITKIO)

    WRAP_GGO(clitkExtractAirwaysTreeInfo_GGO_C clitkExtractAirwaysTreeInfo.ggo)
    ADD_EXECUTABLE(clitkExtractAirwaysTreeInfo clitkExtractAirwaysTreeInfo.cxx ${clitkExtractAirwaysTreeInfo_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractAirwaysTreeInfo clitkSegmentationGgoLib clitkCommon ITKIO)

    WRAP_GGO(clitkExtractBones_GGO_C clitkExtractBones.ggo)
    ADD_EXECUTABLE(clitkExtractBones clitkExtractBones.cxx ${clitkExtractBones_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractBones clitkCommon ITKIO clitkSegmentationGgoLib)

    WRAP_GGO(clitkExtractMediastinum_GGO_C clitkExtractMediastinum.ggo)
    ADD_EXECUTABLE(clitkExtractMediastinum clitkExtractMediastinum.cxx ${clitkExtractMediastinum_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractMediastinum clitkCommon clitkSegmentationGgoLib ITKStatistics ITKIO)

    WRAP_GGO(clitkExtractLymphStations_GGO_C clitkExtractLymphStations.ggo)
    ADD_EXECUTABLE(clitkExtractLymphStations clitkExtractLymphStations.cxx clitkFilterWithAnatomicalFeatureDatabaseManagement.cxx ${clitkExtractLymphStations_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractLymphStations clitkSegmentationGgoLib clitkCommon ITKIO ITKStatistics)

    WRAP_GGO(clitkMorphoMath_GGO_C clitkMorphoMath.ggo)
    ADD_EXECUTABLE(clitkMorphoMath clitkMorphoMath.cxx ${clitkMorphoMath_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMorphoMath clitkCommon ITKIO)

    WRAP_GGO(clitkMorphoReconstruction_GGO_C clitkMorphoReconstruction.ggo)
    ADD_EXECUTABLE(clitkMorphoReconstruction clitkMorphoReconstruction.cxx clitkMorphoReconstructionGenericFilter.cxx ${clitkMorphoReconstruction_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMorphoReconstruction clitkSegmentationGgoLib clitkCommon ITKIO)

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

    WRAP_GGO(clitkTestFilter_GGO_C clitkTestFilter.ggo)
    ADD_EXECUTABLE(clitkTestFilter clitkTestFilter.cxx ${clitkTestFilter_GGO_C})
    TARGET_LINK_LIBRARIES(clitkTestFilter clitkSegmentationGgoLib clitkCommon ITKIO)

ENDIF(CLITK_BUILD_SEGMENTATION)

# ADD_EXECUTABLE(ScalarImageMarkovRandomField1 ScalarImageMarkovRandomField1.cxx)
# TARGET_LINK_LIBRARIES(ScalarImageMarkovRandomField1 clitkCommon ITKIO ITKStatistics)
# ADD_EXECUTABLE(ScalarImageKmeansClassifier ScalarImageKmeansClassifier.cxx)
# TARGET_LINK_LIBRARIES(ScalarImageKmeansClassifier clitkCommon ITKIO ITKStatistics)


