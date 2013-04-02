#=========================================================

# Add Libraries used in vv and clitk to avoid recompilation
FOREACH(clitkTool clitkExtractLung clitkExtractPatient clitkConnectedComponentLabeling clitkRegionGrowing)
    WRAP_GGO(${clitkTool}_GGO_C ${clitkTool}.ggo)
    SET(GGO_C_FILES ${GGO_C_FILES} ${${clitkTool}_GGO_C})
ENDFOREACH(clitkTool)
ADD_LIBRARY(clitkSegmentationGgoLib
            clitkFilterWithAnatomicalFeatureDatabaseManagement.cxx
            clitkAnatomicalFeatureDatabase.cxx
            clitkSliceBySliceRelativePositionFilter_uchar.cxx
            ${GGO_C_FILES})

#=========================================================

IF(CLITK_BUILD_SEGMENTATION)
    WRAP_GGO(clitkConnectedComponentLabeling_GGO_C clitkConnectedComponentLabeling.ggo)
    ADD_EXECUTABLE(clitkConnectedComponentLabeling clitkConnectedComponentLabeling.cxx ${clitkConnectedComponentLabeling_GGO_C})
    TARGET_LINK_LIBRARIES(clitkConnectedComponentLabeling clitkCommon ${ITK_LIBRARIES})
    SET(SEGMENTATION_INSTALL clitkConnectedComponentLabeling)

    ADD_EXECUTABLE(clitkExtractPatient clitkExtractPatient.cxx ${clitkExtractPatient_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractPatient clitkCommon ${ITK_LIBRARIES} clitkSegmentationGgoLib)
    SET(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkExtractPatient)

    ADD_EXECUTABLE(clitkExtractLung clitkExtractLung.cxx  ${clitkExtractLung_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractLung clitkSegmentationGgoLib clitkCommon ${ITK_LIBRARIES})
    SET(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkExtractLung)

    WRAP_GGO(clitkExtractBones_GGO_C clitkExtractBones.ggo)
    ADD_EXECUTABLE(clitkExtractBones clitkExtractBones.cxx ${clitkExtractBones_GGO_C})
    TARGET_LINK_LIBRARIES(clitkExtractBones clitkCommon ${ITK_LIBRARIES} clitkSegmentationGgoLib)
    SET(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkExtractBones)

    WRAP_GGO(clitkMorphoMath_GGO_C clitkMorphoMath.ggo)
    ADD_EXECUTABLE(clitkMorphoMath clitkMorphoMath.cxx ${clitkMorphoMath_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMorphoMath clitkCommon ${ITK_LIBRARIES})
    SET(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkMorphoMath)

    WRAP_GGO(clitkDecomposeAndReconstruct_GGO_C clitkDecomposeAndReconstruct.ggo)
    ADD_EXECUTABLE(clitkDecomposeAndReconstruct clitkDecomposeAndReconstruct.cxx clitkDecomposeAndReconstructGenericFilter.cxx ${clitkDecomposeAndReconstruct_GGO_C})
    TARGET_LINK_LIBRARIES(clitkDecomposeAndReconstruct clitkCommon ${ITK_LIBRARIES})
    SET(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkDecomposeAndReconstruct)

    WRAP_GGO(clitkMotionMask_GGO_C clitkMotionMask.ggo)
    ADD_EXECUTABLE(clitkMotionMask clitkMotionMask.cxx clitkMotionMaskGenericFilter.cxx ${clitkMotionMask_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMotionMask clitkCommon ${ITK_LIBRARIES})
    SET(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkMotionMask)
    
    WRAP_GGO(clitkRegionGrowing_GGO_C clitkRegionGrowing.ggo)
    ADD_EXECUTABLE(clitkRegionGrowing clitkRegionGrowing.cxx ${clitkRegionGrowing_GGO_C} ${clitkRelativePosition_GGO_C})
    TARGET_LINK_LIBRARIES(clitkRegionGrowing clitkCommon ${ITK_LIBRARIES})

    INSTALL (TARGETS ${SEGMENTATION_INSTALL} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

ENDIF(CLITK_BUILD_SEGMENTATION)

# ADD_EXECUTABLE(ScalarImageMarkovRandomField1 ScalarImageMarkovRandomField1.cxx)
# TARGET_LINK_LIBRARIES(ScalarImageMarkovRandomField1 clitkCommon ${ITK_LIBRARIES} ITKStatistics)
# ADD_EXECUTABLE(ScalarImageKmeansClassifier ScalarImageKmeansClassifier.cxx)
# TARGET_LINK_LIBRARIES(ScalarImageKmeansClassifier clitkCommon ${ITK_LIBRARIES} ITKStatistics)


