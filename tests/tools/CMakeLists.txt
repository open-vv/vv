include_directories(
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/tools
)

set(BUILDNAME ${BUILDNAME}_tools CACHE INTERNAL DOCSTRING)
add_definitions(-DCLITK_TEST_TOOLS_PATH=\"${PROJECT_BINARY_DIR}/bin/\")
set(srcs
  toolTestRunner.cxx
)

add_executable(toolTestRunner ${srcs})
target_link_libraries(toolTestRunner ${ITK_LIBRARIES})
set(exe ${EXECUTABLE_OUTPUT_PATH}/toolTestRunner)
set(p ${CLITK_TEST_DATA_PATH})
#=========================================================
# clitkImageInfo
add_test(clitkImageInfo_4d    ${exe} clitkImageInfo ${p}Deformation4D.mhd ${p}Deformation4D_ref.info)
add_test(clitkImageInfo_3d    ${exe} clitkImageInfo ${p}Lung3D.mhd 	 ${p}Lung3D_ref.info)
#=========================================================
# clitkGetSpacing
add_test(clitkGetSpacing_4d   ${exe} clitkGetSpacing -i ${p}Deformation4D.mhd ${p}Deformation4D_ref.spacing)
add_test(clitkGetSpacing_3d   ${exe} clitkGetSpacing -i ${p}Lung3D.mhd 	     ${p}Lung3D_ref.spacing)
#=========================================================
# clitkGetOrigin
add_test(clitkGetOrigin_4d    ${exe} clitkGetOrigin -i ${p}Deformation4D.mhd ${p}Deformation4D_ref.origin)
add_test(clitkGetOrigin_3d    ${exe} clitkGetOrigin -i ${p}Lung3D.mhd 	    ${p}Lung3D_ref.origin)
#=========================================================
# clitkGetSize
add_test(clitkGetSize_4d      ${exe} clitkGetSize -i ${p}Deformation4D.mhd   ${p}Deformation4D_ref.size)
add_test(clitkGetSize_3d     ${exe} clitkGetSize -i ${p}Lung3D.mhd 	    ${p}Lung3D_ref.size)
#=========================================================
# clitkGetDirection
add_test(clitkGetDirection_4d ${exe} clitkGetDirection -i ${p}Deformation4D.mhd  ${p}Deformation4D_ref.direction)
add_test(clitkGetDirection_3d ${exe} clitkGetDirection -i ${p}Lung3D.mhd 	${p}Lung3D_ref.direction)
#=========================================================
# clitkBinarize
add_test(clitkBinarizeBGl0.1356_4d ${exe} clitkBinarizeImage -i ${p}Deformation4D.mhd --mode BG -l 0.1356 -o Deformation4D_ref.binarizeBGl0.1356.mhd ${p}Deformation4D_ref.binarizeBGl0.1356.mhd)
add_test(clitkBinarizeFGl0.1556_3d ${exe} clitkBinarizeImage -i ${p}Lung3D.mhd        --mode FG -l 0.1556 -o Lung3D_ref.binarizeFGl0.1556.mhd ${p}Lung3D_ref.binarizeFGl0.1556.mhd)
#=========================================================
# clitkImageArithm
add_test(clitkImageArithm0_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 0 -o Lung3D_ref.arithm0.mhd ${p}Lung3D_ref.arithm0.mhd)
add_test(clitkImageArithm1_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 1 -o Lung3D_ref.arithm1.mhd ${p}Lung3D_ref.arithm1.mhd)
add_test(clitkImageArithm2_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 2 -o Lung3D_ref.arithm2.mhd ${p}Lung3D_ref.arithm2.mhd)
add_test(clitkImageArithm3_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 3 -o Lung3D_ref.arithm3.mhd ${p}Lung3D_ref.arithm3.mhd)
add_test(clitkImageArithm4_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 4 -o Lung3D_ref.arithm4.mhd ${p}Lung3D_ref.arithm4.mhd)
add_test(clitkImageArithm5_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 5 -o Lung3D_ref.arithm5.mhd ${p}Lung3D_ref.arithm5.mhd)
add_test(clitkImageArithm6_3d ${exe} clitkImageArithm -i ${p}Lung3D.mhd -j ${p}Lung3D.mhd -t 6 -o Lung3D_ref.arithm6.mhd ${p}Lung3D_ref.arithm6.mhd)

add_test(clitkImageArithm0_4d ${exe} clitkImageArithm -i ${p}Deformation4D.mhd -j ${p}Deformation4D.mhd -t 0 -o Deformation4D_ref.arithm0.mhd ${p}Deformation4D_ref.arithm0.mhd)
add_test(clitkImageArithm2_4d ${exe} clitkImageArithm -i ${p}Deformation4D.mhd -j ${p}Deformation4D.mhd -t 2 -o Deformation4D_ref.arithm2.mhd ${p}Deformation4D_ref.arithm2.mhd)
#=========================================================
# clitkCropImage
add_test(clitkCropImage_3d ${exe} clitkCropImage -i ${p}Lung3D.mhd -b 1,4,0,2,2,3 -o Lung3D_ref.cropImage.1.4.0.2.2.3.mhd ${p}Lung3D_ref.cropImage.1.4.0.2.2.3.mhd)
#=========================================================
#clitkMorphoMath
add_test(clitkMorphoMath0_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t0.mhd ${p}Lung3D_ref.morphoMath.t0.mhd)
add_test(clitkMorphoMath1_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t1.mhd ${p}Lung3D_ref.morphoMath.t1.mhd)
add_test(clitkMorphoMath2_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t2.mhd ${p}Lung3D_ref.morphoMath.t2.mhd)
add_test(clitkMorphoMath3_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t3.mhd ${p}Lung3D_ref.morphoMath.t3.mhd)
add_test(clitkMorphoMath4_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t4.mhd ${p}Lung3D_ref.morphoMath.t4.mhd)
add_test(clitkMorphoMath5_3d ${exe} clitkMorphoMath -i ${p}Lung3D.mhd -o Lung3D_ref.morphoMath.t5.mhd ${p}Lung3D_ref.morphoMath.t5.mhd)

add_test(clitkMorphoMath0_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t0.mhd ${p}Deformation4D_ref.morphoMath.t0.mhd)
add_test(clitkMorphoMath1_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t1.mhd ${p}Deformation4D_ref.morphoMath.t1.mhd)
add_test(clitkMorphoMath2_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t2.mhd ${p}Deformation4D_ref.morphoMath.t2.mhd)
add_test(clitkMorphoMath3_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t3.mhd ${p}Deformation4D_ref.morphoMath.t3.mhd)
add_test(clitkMorphoMath4_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t4.mhd ${p}Deformation4D_ref.morphoMath.t4.mhd)
add_test(clitkMorphoMath5_4d ${exe} clitkMorphoMath -i ${p}Deformation4D.mhd -o Deformation4D_ref.morphoMath.t5.mhd ${p}Deformation4D_ref.morphoMath.t5.mhd)
#=========================================================
#clitkSetBackground
add_test(clitkSetBackground_4d ${exe} clitkSetBackground -i ${p}Deformation4D.mhd -m ${p}Deformation4D.mhd -o Deformation4D_ref.setBackground.mhd ${p}Deformation4D_ref.setBackground.mhd)
add_test(clitkSetBackground_3d ${exe} clitkSetBackground -i ${p}Lung3D.mhd -m ${p}Lung3D.mhd -o Lung3D_ref.setBackground.mhd ${p}Lung3D_ref.setBackground.mhd)
#clitkWarpImage
add_test(clitkWarpImage_3d ${exe} clitkWarpImage -i ${p}00-P.mhd --vf ${p}vf50-P_1.mhd -o 00-P_ref.warpImage.mhd ${p}00-P_ref.warpImage.mhd)









unset(exe)
unset(p)

