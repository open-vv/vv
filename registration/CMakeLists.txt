#ADD_LIBRARY(clitkLBGSB STATIC clitkLBGSBOptimizer.cxx)
#=========================================================

# Add Libraries used in vv and clitk to avoid recompilation
WRAP_GGO(clitkAffineRegistration_GGO_C clitkAffineRegistration.ggo)
ADD_LIBRARY(clitkAffineRegistrationLib 
            clitkAffineRegistrationGenericFilter.cxx
            ${clitkAffineRegistration_GGO_C}
            clitkLBFGSBOptimizer.cxx
            clitkGenericAffineTransform.cxx)

#=========================================================

IF(CLITK_BUILD_REGISTRATION)
    ############################## registration algorithms
    ADD_EXECUTABLE(clitkAffineRegistration clitkAffineRegistration.cxx)
    TARGET_LINK_LIBRARIES(clitkAffineRegistration  clitkAffineRegistrationLib clitkCommon ${ITK_LIBRARIES})
    SET(REGISTRATION_INSTALL clitkAffineRegistration)

    WRAP_GGO(clitkDemonsDeformableRegistration_GGO_C clitkDemonsDeformableRegistration.ggo)
    ADD_EXECUTABLE(clitkDemonsDeformableRegistration clitkDemonsDeformableRegistration.cxx ${clitkDemonsDeformableRegistration_GGO_C} clitkDemonsDeformableRegistrationGenericFilter.cxx)
    TARGET_LINK_LIBRARIES(clitkDemonsDeformableRegistration ${ITK_LIBRARIES} clitkCommon)
    SET(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkDemonsDeformableRegistration)

    WRAP_GGO(clitkBLUTDIR_GGO_C clitkBLUTDIR.ggo)
    ADD_EXECUTABLE(clitkBLUTDIR  clitkBLUTDIR.cxx clitkBLUTDIRGenericFilter.cxx ${clitkBLUTDIR_GGO_C} clitkLBFGSBOptimizer.cxx )
    TARGET_LINK_LIBRARIES(clitkBLUTDIR clitkCommon ${ITK_LIBRARIES} )
    SET(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkBLUTDIR)


    ############################## registration-related tools to process points, bsplines, vf and image pyramids
    WRAP_GGO(clitkCalculateTRE_GGO_C clitkCalculateTRE.ggo)
    ADD_EXECUTABLE(clitkCalculateTRE clitkCalculateTRE.cxx clitkCalculateTREGenericFilter.cxx ${clitkCalculateTRE_GGO_C})
    TARGET_LINK_LIBRARIES(clitkCalculateTRE clitkCommon ${ITK_LIBRARIES} )
    SET(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkCalculateTRE)

    WRAP_GGO(clitkMatrixTransformToVF_GGO_C clitkMatrixTransformToVF.ggo)
    ADD_EXECUTABLE(clitkMatrixTransformToVF clitkMatrixTransformToVF.cxx clitkMatrixTransformToVFGenericFilter.cxx  ${clitkMatrixTransformToVF_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMatrixTransformToVF ${ITK_LIBRARIES} clitkCommon)
    SET(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkMatrixTransformToVF)

    WRAP_GGO(clitkConvertBSplineDeformableTransformToVF_GGO_C clitkConvertBSplineDeformableTransformToVF.ggo)
    ADD_EXECUTABLE(clitkConvertBSplineDeformableTransformToVF  clitkConvertBSplineDeformableTransformToVF.cxx clitkConvertBSplineDeformableTransformToVFGenericFilter.cxx  ${clitkConvertBSplineDeformableTransformToVF_GGO_C})
    TARGET_LINK_LIBRARIES(clitkConvertBSplineDeformableTransformToVF clitkCommon  ${ITK_LIBRARIES})
    SET(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkConvertBSplineDeformableTransformToVF)

#    SET_TARGET_PROPERTIES(${REGISTRATION_INSTALL} PROPERTIES INSTALL_RPATH "${VTK_DIR}:${ITK_DIR}" )  
    INSTALL (TARGETS ${REGISTRATION_INSTALL} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

ENDIF(CLITK_BUILD_REGISTRATION)

