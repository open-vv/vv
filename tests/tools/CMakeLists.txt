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
ADD_DEFINITIONS(-DCLITK_TEST_TOOLS_PATH=\"${PROJECT_BINARY_DIR}/bin/\")
SET (srcs
  toolTestRunner.cxx
)

ADD_EXECUTABLE(toolTestRunner ${srcs})
TARGET_LINK_LIBRARIES(toolTestRunner vvLib ${vvExternalLibs})
SET(exe ${EXECUTABLE_OUTPUT_PATH}/toolTestRunner) 
SET(p ${CLITK_TEST_DATA_PATH})
SET(tmpFile "feve51zd")
#=========================================================
# clitkImageInfo
ADD_TEST(clitkImageInfo_4d    ${exe} clitkImageInfo ${p}Deformation4D.mhd ${p}Deformation4D_ref.info)
ADD_TEST(clitkImageInfo_3d    ${exe} clitkImageInfo ${p}Lung3D.mhd 	 ${p}Lung3D_ref.info)
#=========================================================
# clitkGetSpacing
ADD_TEST(clitkGetSpacing_4d   ${exe} clitkGetSpacing -i ${p}Deformation4D.mhd ${p}Deformation4D_ref.spacing)
ADD_TEST(clitkGetSpacing_3d   ${exe} clitkGetSpacing -i ${p}Lung3D.mhd 	     ${p}Lung3D_ref.spacing)
#=========================================================
# clitkGetOrigin
ADD_TEST(clitkGetOrigin_4d    ${exe} clitkGetOrigin -i ${p}Deformation4D.mhd ${p}Deformation4D_ref.origin)
ADD_TEST(clitkGetOrigin_3d    ${exe} clitkGetOrigin -i ${p}Lung3D.mhd 	    ${p}Lung3D_ref.origin)
#=========================================================
# clitkGetSize
ADD_TEST(clitkGetSize_4d      ${exe} clitkGetSize -i ${p}Deformation4D.mhd   ${p}Deformation4D_ref.size)
ADD_TEST(clitkGetSize_3d     ${exe} clitkGetSize -i ${p}Lung3D.mhd 	    ${p}Lung3D_ref.size)
#=========================================================
# clitkGetDirection
ADD_TEST(clitkGetDirection_4d ${exe} clitkGetDirection -i ${p}Deformation4D.mhd  ${p}Deformation4D_ref.direction)
ADD_TEST(clitkGetDirection_3d ${exe} clitkGetDirection -i ${p}Lung3D.mhd 	${p}Lung3D_ref.direction)
#=========================================================
# clitkBinarize
ADD_TEST(clitkBinarizeBGl0.1356_4d ${exe} clitkBinarizeImage -i ${p}Deformation4D.mhd --mode BG -l 0.1356 -o Deformation4D_ref.binarizeBGl0.1356.mhd ${p}Deformation4D_ref.binarizeBGl0.1356)
ADD_TEST(clitkBinarizeFGl0.1556_3d ${exe} clitkBinarizeImage -i ${p}Lung3D.mhd        --mode FG -l 0.1556 -o Lung3D_ref.binarizeFGl0.1556.mhd ${p}Lung3D_ref.binarizeFGl0.1556)

UNSET(tmpFile)
UNSET(exe)
UNSET(p)