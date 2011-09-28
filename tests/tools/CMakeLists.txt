include_directories(
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/tools
  ${GTEST_DIR}/include
)

SET(BUILDNAME ${BUILDNAME}_tools CACHE INTERNAL DOCSTRING)
ADD_DEFINITIONS(-DCLITK_TEST_TOOLS_PATH=\"${PROJECT_BINARY_DIR}/bin/\")
SET (srcs
  toolTestRunner.cxx
)

ADD_EXECUTABLE(toolTestRunner ${srcs})
TARGET_LINK_LIBRARIES(toolTestRunner ${ITK_LIBRARIES})
SET(exe ${EXECUTABLE_OUTPUT_PATH}/toolTestRunner) 
SET(p ${CLITK_TEST_DATA_PATH})
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
ADD_TEST(clitkBinarizeBGl0.1356_4d ${exe} clitkBinarizeImage -i ${p}Deformation4D.mhd --mode BG -l 0.1356 -o Deformation4D_ref.binarizeBGl0.1356.mhd ${p}Deformation4D_ref.binarizeBGl0.1356.mhd)
ADD_TEST(clitkBinarizeFGl0.1556_3d ${exe} clitkBinarizeImage -i ${p}Lung3D.mhd        --mode FG -l 0.1556 -o Lung3D_ref.binarizeFGl0.1556.mhd ${p}Lung3D_ref.binarizeFGl0.1556.mhd)
#=========================================================
# clitkImageArithm
ADD_TEST(clitkImageArithm0_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 0 -o Lung3D_ref.arithm0.mhd ${p}Lung3D_ref.arithm0.mhd)
ADD_TEST(clitkImageArithm1_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 1 -o Lung3D_ref.arithm1.mhd ${p}Lung3D_ref.arithm1.mhd)
ADD_TEST(clitkImageArithm2_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 2 -o Lung3D_ref.arithm2.mhd ${p}Lung3D_ref.arithm2.mhd)
ADD_TEST(clitkImageArithm3_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 3 -o Lung3D_ref.arithm3.mhd ${p}Lung3D_ref.arithm3.mhd)
ADD_TEST(clitkImageArithm4_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 4 -o Lung3D_ref.arithm4.mhd ${p}Lung3D_ref.arithm4.mhd)
ADD_TEST(clitkImageArithm5_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 5 -o Lung3D_ref.arithm5.mhd ${p}Lung3D_ref.arithm5.mhd)
ADD_TEST(clitkImageArithm6_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 6 -o Lung3D_ref.arithm6.mhd ${p}Lung3D_ref.arithm6.mhd)

ADD_TEST(clitkImageArithm0_4d ${exe} clitkImageArithm -i ${p}Deformation4D.mhd -j ${p}Deformation4D.mhd -t 0 -o Deformation4D_ref.arithm0.mhd ${p}Deformation4D_ref.arithm0.mhd)
ADD_TEST(clitkImageArithm2_4d ${exe} clitkImageArithm -i ${p}Deformation4D.mhd -j ${p}Deformation4D.mhd -t 2 -o Deformation4D_ref.arithm2.mhd ${p}Deformation4D_ref.arithm2.mhd)
#=========================================================
# clitkCropImage
ADD_TEST(clitkCropImage_3d ${exe} clitkCropImage -i ${p}Lung3D.mhd -b 1,4,0,2,2,3 -o Lung3D_ref.cropImage.1.4.0.2.2.3.mhd ${p}Lung3D_ref.cropImage.1.4.0.2.2.3.mhd)
#=========================================================
#clitkMorphoMath
ADD_TEST(clitkMorphoMath0_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t0.mhd ${p}Lung3D_ref.morphoMath.t0.mhd)
ADD_TEST(clitkMorphoMath1_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t1.mhd ${p}Lung3D_ref.morphoMath.t1.mhd)
ADD_TEST(clitkMorphoMath2_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t2.mhd ${p}Lung3D_ref.morphoMath.t2.mhd)
ADD_TEST(clitkMorphoMath3_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t3.mhd ${p}Lung3D_ref.morphoMath.t3.mhd)
ADD_TEST(clitkMorphoMath4_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t4.mhd ${p}Lung3D_ref.morphoMath.t4.mhd)
ADD_TEST(clitkMorphoMath5_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t5.mhd ${p}Lung3D_ref.morphoMath.t5.mhd)

ADD_TEST(clitkMorphoMath0_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t0.mhd ${p}Deformation4D_ref.morphoMath.t0.mhd)
ADD_TEST(clitkMorphoMath1_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t1.mhd ${p}Deformation4D_ref.morphoMath.t1.mhd)
ADD_TEST(clitkMorphoMath2_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t2.mhd ${p}Deformation4D_ref.morphoMath.t2.mhd)
ADD_TEST(clitkMorphoMath3_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t3.mhd ${p}Deformation4D_ref.morphoMath.t3.mhd)
ADD_TEST(clitkMorphoMath4_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t4.mhd ${p}Deformation4D_ref.morphoMath.t4.mhd)
ADD_TEST(clitkMorphoMath5_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t5.mhd ${p}Deformation4D_ref.morphoMath.t5.mhd)
#=========================================================
#clitkSetBackground
ADD_TEST(clitkSetBackground_4d ${exe} clitkSetBackground -i ${p}Deformation4D.mhd -m ${p}Deformation4D.mhd -o Deformation4D_ref.setBackground.mhd ${p}Deformation4D_ref.setBackground.mhd)
ADD_TEST(clitkSetBackground_3d ${exe} clitkSetBackground -i ${p}Lung3D.mhd -m ${p}Lung3D.mhd -o Lung3D_ref.setBackground.mhd ${p}Lung3D_ref.setBackground.mhd)
#clitkWarpImage
ADD_TEST(clitkWarpImage_3d ${exe} clitkWarpImage -i ${p}00-P.mhd --vf ${p}vf50-P_1.mhd -o 00-P_ref.warpImage.mhd ${p}00-P_ref.warpImage.mhd)









UNSET(exe)
UNSET(p)

