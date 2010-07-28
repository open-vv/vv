#ADD_LIBRARY(clitkLBGSB STATIC clitkLBGSBOptimizer.cxx)
#=========================================================

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)

#=========================================================

#Add your ggo files to this list if they are used by vv
FOREACH(clitkTool clitkAffineRegistration)
    WRAP_GGO(${clitkTool}_GGO_C ${clitkTool}.ggo)
    SET(GGO_C_FILES ${GGO_C_FILES} ${${clitkTool}_GGO_C})
ENDFOREACH(clitkTool)
ADD_LIBRARY(clitkRegistrationGgoLib ${GGO_C_FILES} clitkAffineRegistrationGenericFilter.cxx clitkLBFGSBOptimizer.cxx clitkGenericAffineTransform.cxx)

#=========================================================

IF(CLITK_BUILD_REGISTRATION)
    ############################## registration algorithms
    ADD_EXECUTABLE(clitkAffineRegistration clitkAffineRegistration.cxx)
    TARGET_LINK_LIBRARIES(clitkAffineRegistration  clitkRegistrationGgoLib clitkCommon ITKIO ITKNumerics ITKStatistics)

    WRAP_GGO(clitkDemonsDeformableRegistration_GGO_C clitkDemonsDeformableRegistration.ggo)
    ADD_EXECUTABLE(clitkDemonsDeformableRegistration clitkDemonsDeformableRegistration.cxx ${clitkDemonsDeformableRegistration_GGO_C} clitkDemonsDeformableRegistrationGenericFilter.cxx)
    TARGET_LINK_LIBRARIES(clitkDemonsDeformableRegistration ITKNumerics ITKIO clitkCommon)

    WRAP_GGO(clitkBLUTDIR_GGO_C clitkBLUTDIR.ggo)
    ADD_EXECUTABLE(clitkBLUTDIR  clitkBLUTDIR.cxx clitkBLUTDIRGenericFilter.cxx ${clitkBLUTDIR_GGO_C} clitkLBFGSBOptimizer.cxx )
    TARGET_LINK_LIBRARIES(clitkBLUTDIR clitkCommon ITKIO ITKNumerics ITKStatistics)

    WRAP_GGO(clitkShapedBLUTSpatioTemporalDIR_GGO_C clitkShapedBLUTSpatioTemporalDIR.ggo)
    ADD_EXECUTABLE(clitkShapedBLUTSpatioTemporalDIR  clitkShapedBLUTSpatioTemporalDIR.cxx clitkShapedBLUTSpatioTemporalDIRGenericFilter.cxx  ${clitkShapedBLUTSpatioTemporalDIR_GGO_C} clitkLBFGSBOptimizer.cxx)
    TARGET_LINK_LIBRARIES(clitkShapedBLUTSpatioTemporalDIR clitkCommon ITKNumerics ITKStatistics ITKIO)

    WRAP_GGO(clitkBSplineDeformableRegistration_GGO_C clitkBSplineDeformableRegistration.ggo)
    ADD_EXECUTABLE(clitkBSplineDeformableRegistration clitkBSplineDeformableRegistration.cxx clitkBSplineDeformableRegistrationGenericFilter.cxx  ${clitkBSplineDeformableRegistration_GGO_C} clitkLBFGSBOptimizer.cxx)
    TARGET_LINK_LIBRARIES(clitkBSplineDeformableRegistration  clitkCommon ITKNumerics ITKStatistics ITKIO)


    ############################## registration-related tools to process points, bsplines, vf and image pyramids
    WRAP_GGO(clitkConvertPointList_GGO_C clitkConvertPointList.ggo)
    ADD_EXECUTABLE(clitkConvertPointList clitkConvertPointList.cxx  ${clitkConvertPointList_GGO_C})
    TARGET_LINK_LIBRARIES(clitkConvertPointList ITKIO clitkCommon )

    WRAP_GGO(clitkSelectPoints_GGO_C clitkSelectPoints.ggo)
    ADD_EXECUTABLE(clitkSelectPoints clitkSelectPoints.cxx clitkSelectPointsGenericFilter.cxx  ${clitkSelectPoints_GGO_C})
    TARGET_LINK_LIBRARIES(clitkSelectPoints clitkCommon ITKIO)

    WRAP_GGO(clitkPointTrajectory_GGO_C clitkPointTrajectory.ggo)
    ADD_EXECUTABLE(clitkPointTrajectory  clitkPointTrajectory.cxx clitkPointTrajectoryGenericFilter.cxx  ${clitkPointTrajectory_GGO_C})
    TARGET_LINK_LIBRARIES(clitkPointTrajectory clitkCommon ITKIO)

    WRAP_GGO(clitkCalculateTRE_GGO_C clitkCalculateTRE.ggo)
    ADD_EXECUTABLE(clitkCalculateTRE clitkCalculateTRE.cxx clitkCalculateTREGenericFilter.cxx ${clitkCalculateTRE_GGO_C})
    TARGET_LINK_LIBRARIES(clitkCalculateTRE clitkCommon ITKIO )

    WRAP_GGO(clitkMatrixTransformToVF_GGO_C clitkMatrixTransformToVF.ggo)
    ADD_EXECUTABLE(clitkMatrixTransformToVF clitkMatrixTransformToVF.cxx clitkMatrixTransformToVFGenericFilter.cxx  ${clitkMatrixTransformToVF_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMatrixTransformToVF ITKIO clitkCommon)

    WRAP_GGO(clitkConvertBSplineDeformableTransformToVF_GGO_C clitkConvertBSplineDeformableTransformToVF.ggo)
    ADD_EXECUTABLE(clitkConvertBSplineDeformableTransformToVF  clitkConvertBSplineDeformableTransformToVF.cxx clitkConvertBSplineDeformableTransformToVFGenericFilter.cxx  ${clitkConvertBSplineDeformableTransformToVF_GGO_C})
    TARGET_LINK_LIBRARIES(clitkConvertBSplineDeformableTransformToVF clitkCommon  ITKIO)

    WRAP_GGO(clitkMultiResolutionPyramid_GGO_C clitkMultiResolutionPyramid.ggo)
    ADD_EXECUTABLE(clitkMultiResolutionPyramid  clitkMultiResolutionPyramid.cxx clitkMultiResolutionPyramidGenericFilter.cxx ${clitkMultiResolutionPyramid_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMultiResolutionPyramid clitkCommon  ITKIO)

    WRAP_GGO(clitkBSplinePyramid_GGO_C clitkBSplinePyramid.ggo)
    ADD_EXECUTABLE(clitkBSplinePyramid  clitkBSplinePyramid.cxx clitkBSplinePyramidGenericFilter.cxx ${clitkBSplinePyramid_GGO_C})
    TARGET_LINK_LIBRARIES(clitkBSplinePyramid  clitkCommon  ITKIO)

    WRAP_GGO(clitkResampleBSplineDeformableTransform_GGO_C clitkResampleBSplineDeformableTransform.ggo)
    ADD_EXECUTABLE(clitkResampleBSplineDeformableTransform  clitkResampleBSplineDeformableTransform.cxx clitkResampleBSplineDeformableTransformGenericFilter.cxx ${clitkResampleBSplineDeformableTransform_GGO_C})
    TARGET_LINK_LIBRARIES(clitkResampleBSplineDeformableTransform clitkCommon ITKIO)
ENDIF(CLITK_BUILD_REGISTRATION)

