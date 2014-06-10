#=========================================================

# Add Libraries used in vv and clitk to avoid recompilation
WRAP_GGO(clitkAffineRegistration_GGO_C clitkAffineRegistration.ggo)
add_library(clitkAffineRegistrationLib
            clitkAffineRegistrationGenericFilter.cxx
            ${clitkAffineRegistration_GGO_C}
            clitkLBFGSBOptimizer.cxx
            clitkGenericAffineTransform.cxx)

#=========================================================

if(CLITK_BUILD_REGISTRATION)
    ############################## registration algorithms
    add_executable(clitkAffineRegistration clitkAffineRegistration.cxx)
    target_link_libraries(clitkAffineRegistration  clitkAffineRegistrationLib clitkCommon)
    set(REGISTRATION_INSTALL clitkAffineRegistration)

    WRAP_GGO(clitkDemonsDeformableRegistration_GGO_C clitkDemonsDeformableRegistration.ggo)
    add_executable(clitkDemonsDeformableRegistration clitkDemonsDeformableRegistration.cxx ${clitkDemonsDeformableRegistration_GGO_C} clitkDemonsDeformableRegistrationGenericFilter.cxx)
    target_link_libraries(clitkDemonsDeformableRegistration clitkCommon)
    set(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkDemonsDeformableRegistration)

    WRAP_GGO(clitkBLUTDIR_GGO_C clitkBLUTDIR.ggo)
    add_executable(clitkBLUTDIR  clitkBLUTDIR.cxx clitkBLUTDIRGenericFilter.cxx ${clitkBLUTDIR_GGO_C} clitkLBFGSBOptimizer.cxx )
    target_link_libraries(clitkBLUTDIR clitkCommon)
    set(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkBLUTDIR)


    ############################## registration-related tools to process points, bsplines, vf and image pyramids
    WRAP_GGO(clitkCalculateTRE_GGO_C clitkCalculateTRE.ggo)
    add_executable(clitkCalculateTRE clitkCalculateTRE.cxx clitkCalculateTREGenericFilter.cxx ${clitkCalculateTRE_GGO_C})
    target_link_libraries(clitkCalculateTRE clitkCommon)
    set(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkCalculateTRE)

    WRAP_GGO(clitkMatrixTransformToVF_GGO_C clitkMatrixTransformToVF.ggo)
    add_executable(clitkMatrixTransformToVF clitkMatrixTransformToVF.cxx clitkMatrixTransformToVFGenericFilter.cxx  ${clitkMatrixTransformToVF_GGO_C})
    target_link_libraries(clitkMatrixTransformToVF clitkCommon)
    set(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkMatrixTransformToVF)

    WRAP_GGO(clitkConvertBSplineDeformableTransformToVF_GGO_C clitkConvertBSplineDeformableTransformToVF.ggo)
    add_executable(clitkConvertBSplineDeformableTransformToVF  clitkConvertBSplineDeformableTransformToVF.cxx clitkConvertBSplineDeformableTransformToVFGenericFilter.cxx  ${clitkConvertBSplineDeformableTransformToVF_GGO_C})
    target_link_libraries(clitkConvertBSplineDeformableTransformToVF clitkCommon)
    set(REGISTRATION_INSTALL ${REGISTRATION_INSTALL} clitkConvertBSplineDeformableTransformToVF)

    install(TARGETS ${REGISTRATION_INSTALL} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

endif(CLITK_BUILD_REGISTRATION)

