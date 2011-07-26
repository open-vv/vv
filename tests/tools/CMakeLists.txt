include_directories(
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/tools
  ${GTEST_DIR}/include
)

SET (CUSTOM_TEST_SRC
  clitkImageInfoTest.cxx
  vvMainTest.cxx
)

SET(BUILDNAME ${BUILDNAME}_tools CACHE INTERNAL DOCSTRING)

SET (srcs
  toolTestRunner.cxx
)
ADD_EXECUTABLE(toolTestRunner ${srcs})
TARGET_LINK_LIBRARIES(toolTestRunner vvLib ${vvExternalLibs})
SET(exe ${EXECUTABLE_OUTPUT_PATH}/toolTestRunner) 
SET(p ${CLITK_TEST_DATA_PATH})
#=========================================================
# clitkImageInfo
ADD_TEST(clitkImageInfo_1    ${exe} clitkImageInfo ${p}Deformation4D.mhd ${p}Deformation4D_ref.info)
ADD_TEST(clitkImageInfo_2    ${exe} clitkImageInfo ${p}Lung3D.mhd 	 ${p}Lung3D_ref.info)
#=========================================================
# clitkGetSpacing
ADD_TEST(clitkGetSpacing_1   ${exe} clitkGetSpacing -i ${p}Deformation4D.mhd ${p}Deformation4D_ref.spacing)
ADD_TEST(clitkGetSpacing_2   ${exe} clitkGetSpacing -i ${p}Lung3D.mhd 	     ${p}Lung3D_ref.spacing)
#=========================================================
# clitkGetOrigin
ADD_TEST(clitkGetOrigin_1    ${exe} clitkGetOrigin -i ${p}Deformation4D.mhd ${p}Deformation4D_ref.origin)
ADD_TEST(clitkGetOrigin_2    ${exe} clitkGetOrigin -i ${p}Lung3D.mhd 	    ${p}Lung3D_ref.origin)
#=========================================================
# clitkGetSize
ADD_TEST(clitkGetSize_1      ${exe} clitkGetSize -i ${p}Deformation4D.mhd   ${p}Deformation4D_ref.size)
ADD_TEST(clitkGetSize_2      ${exe} clitkGetSize -i ${p}Lung3D.mhd 	    ${p}Lung3D_ref.size)
#=========================================================
# clitkGetDirection
ADD_TEST(clitkGetDirection_1 ${exe} clitkGetDirection -i ${p}Deformation4D.mhd  ${p}Deformation4D_ref.direction)
ADD_TEST(clitkGetDirection_2 ${exe} clitkGetDirection -i ${p}Lung3D.mhd 	${p}Lung3D_ref.direction)





UNSET(exe)
UNSET(p)