#=========================================================
#Support for the CTest dashboard testing system
OPTION(BUILD_TESTING "Build the testing tree" OFF)
IF (BUILD_TESTING)
  OPTION(CLITK_BUILD_TESTING "Test ITK" ON)
  INCLUDE(CTest)
ENDIF(BUILD_TESTING)
#=========================================================


#=========================================================
# If the user choose to build documentation, then search for Doxygen executables.
OPTION(BUILD_DOXYGEN "Build Doxygen Documentation" OFF)
IF(BUILD_DOXYGEN)
  FIND_PACKAGE(Doxygen)
  ADD_SUBDIRECTORY(${CLITK_SOURCE_DIR}/Doxygen ${PROJECT_BINARY_DIR}/Doxygen)
ENDIF(BUILD_DOXYGEN)
#=========================================================

# Compilation options
OPTION(CLITK_EXPERIMENTAL "Enable experimental software and features" OFF)
OPTION(CLITK_BUILD_TOOLS "Build command-line tools" OFF)
OPTION(CLITK_BUILD_SEGMENTATION "Build command-line segmentation tools" OFF)
OPTION(CLITK_BUILD_REGISTRATION "Build command-line registration tools" OFF)

OPTION(CLITK_BUILD_VV "Build vv the 4D visualizer (requires VTK and QT)" ON)
IF (CLITK_BUILD_VV)
  ADD_SUBDIRECTORY(${CLITK_SOURCE_DIR}/vv ${PROJECT_BINARY_DIR}/vv)
ENDIF(CLITK_BUILD_VV)

#=========================================================
# Build test when vv has been compiled
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(${CLITK_SOURCE_DIR}/tests ${PROJECT_BINARY_DIR}/tests)
ENDIF(BUILD_TESTING)

#=========================================================
# Install scripts when running make install
SET(SCRIPTS 
  ${CLITK_SOURCE_DIR}/scripts/calculate_motion_amplitude.sh
  ${CLITK_SOURCE_DIR}/scripts/midp_common.sh
  ${CLITK_SOURCE_DIR}/scripts/registration.sh
  ${CLITK_SOURCE_DIR}/scripts/create_midP.sh
  ${CLITK_SOURCE_DIR}/scripts/create_midP-2.0.sh        
  ${CLITK_SOURCE_DIR}/scripts/create_mhd_4D.sh          
  ${CLITK_SOURCE_DIR}/scripts/create_mhd_4D_pattern.sh  
  ${CLITK_SOURCE_DIR}/scripts/create_midP_masks.sh  
  ${CLITK_SOURCE_DIR}/scripts/create_midP_masks-2.0.sh  
  ${CLITK_SOURCE_DIR}/scripts/pts_to_landmarks.sh
  ${CLITK_SOURCE_DIR}/scripts/create_mhd_3D.sh
  ${CLITK_SOURCE_DIR}/scripts/dicom_info.sh
)

INSTALL (FILES ${SCRIPTS} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
