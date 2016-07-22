#=========================================================

# Add Libraries used in vv and clitk to avoid recompilation
foreach(clitkTool clitkExtractLung clitkExtractPatient clitkConnectedComponentLabeling clitkRegionGrowing)
    WRAP_GGO(${clitkTool}_GGO_C ${clitkTool}.ggo)
    set(GGO_C_FILES ${GGO_C_FILES} ${${clitkTool}_GGO_C})
endforeach(clitkTool)
add_library(clitkSegmentationGgoLib
            clitkFilterWithAnatomicalFeatureDatabaseManagement.cxx
            clitkAnatomicalFeatureDatabase.cxx
            clitkSliceBySliceRelativePositionFilter_uchar.cxx
            ${GGO_C_FILES})

#=========================================================

if(CLITK_BUILD_SEGMENTATION)
    WRAP_GGO(clitkConnectedComponentLabeling_GGO_C clitkConnectedComponentLabeling.ggo)
    add_executable(clitkConnectedComponentLabeling clitkConnectedComponentLabeling.cxx ${clitkConnectedComponentLabeling_GGO_C})
    target_link_libraries(clitkConnectedComponentLabeling clitkCommon)
    set(SEGMENTATION_INSTALL clitkConnectedComponentLabeling)

    add_executable(clitkExtractPatient clitkExtractPatient.cxx ${clitkExtractPatient_GGO_C})
    target_link_libraries(clitkExtractPatient clitkCommon clitkSegmentationGgoLib)
    set(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkExtractPatient)

    add_executable(clitkExtractLung clitkExtractLung.cxx  ${clitkExtractLung_GGO_C})
    target_link_libraries(clitkExtractLung clitkSegmentationGgoLib clitkCommon)
    set(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkExtractLung)

    WRAP_GGO(clitkExtractBones_GGO_C clitkExtractBones.ggo)
    add_executable(clitkExtractBones clitkExtractBones.cxx ${clitkExtractBones_GGO_C})
    target_link_libraries(clitkExtractBones clitkCommon clitkSegmentationGgoLib)
    set(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkExtractBones)

    WRAP_GGO(clitkMorphoMath_GGO_C clitkMorphoMath.ggo)
    add_executable(clitkMorphoMath clitkMorphoMath.cxx ${clitkMorphoMath_GGO_C})
    target_link_libraries(clitkMorphoMath clitkCommon)
    set(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkMorphoMath)

    WRAP_GGO(clitkDecomposeAndReconstruct_GGO_C clitkDecomposeAndReconstruct.ggo)
    add_executable(clitkDecomposeAndReconstruct clitkDecomposeAndReconstruct.cxx clitkDecomposeAndReconstructGenericFilter.cxx ${clitkDecomposeAndReconstruct_GGO_C})
    target_link_libraries(clitkDecomposeAndReconstruct clitkCommon)
    set(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkDecomposeAndReconstruct)

    WRAP_GGO(clitkMotionMask_GGO_C clitkMotionMask.ggo)
    add_executable(clitkMotionMask clitkMotionMask.cxx clitkMotionMaskGenericFilter.cxx ${clitkMotionMask_GGO_C})
    target_link_libraries(clitkMotionMask clitkCommon)
    set(SEGMENTATION_INSTALL ${SEGMENTATION_INSTALL} clitkMotionMask)

    WRAP_GGO(clitkRegionGrowing_GGO_C clitkRegionGrowing.ggo)
    add_executable(clitkRegionGrowing clitkRegionGrowing.cxx ${clitkRegionGrowing_GGO_C} ${clitkRelativePosition_GGO_C})
    target_link_libraries(clitkRegionGrowing clitkCommon)

    install(TARGETS ${SEGMENTATION_INSTALL} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

endif(CLITK_BUILD_SEGMENTATION)

# add_executable(ScalarImageMarkovRandomField1 ScalarImageMarkovRandomField1.cxx)
# target_link_libraries(ScalarImageMarkovRandomField1 clitkCommon ITKStatistics)
# add_executable(ScalarImageKmeansClassifier ScalarImageKmeansClassifier.cxx)
# target_link_libraries(ScalarImageKmeansClassifier clitkCommon ITKStatistics)


