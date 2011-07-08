include_directories(
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/tools
  ${GTEST_DIR}/include
)

SET (CUSTOM_TEST_SRC
  clitkImageInfoTest.cxx
  vvMainTest.cxx
)
ADD_EXECUTABLE(toolsTest ${CUSTOM_TEST_SRC})
ADD_DEFINITIONS(-DCLITK_TEST_TOOLS_PATH=\"${PROJECT_BINARY_DIR}/bin/\")
TARGET_LINK_LIBRARIES(toolsTest vvLib ${vvExternalLibs} gtest)

# Add all tests found in the source code, calling the executable to run them
add_google_tests ( ${EXECUTABLE_OUTPUT_PATH}/toolsTest ${srcs})

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
ADD_TEST(clitkImageInfo_1 ${exe} clitkImageInfo ${p}Deformation4D.mhd ${p}Deformation4D_ref.info)
ADD_TEST(clitkImageInfo_2 ${exe} clitkImageInfo ${p}Lung3D.mhd ${p}Lung3D_ref.info)
#=========================================================
# clitkGetSpacing
ADD_TEST(clitkGetSpacing_1 ${exe} clitkGetSpacing -i ${p}Deformation4D.mhd ${p}Deformation4D_ref.spacing)
message(clitkGetSpacing_1 "${exe} clitkGetSpacing -i ${p}Deformation4D.mhd ${p}Deformation4D_ref.spacing")
ADD_TEST(clitkGetSpacing_2 ${exe} clitkGetSpacing -i ${p}Lung3D.mhd ${p}Lung3D_ref.spacing)
UNSET(exe)
UNSET(p)