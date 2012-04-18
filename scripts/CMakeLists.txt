#=========================================================
# Install scripts when running make install
SET(SCRIPTS 
  ${CLITK_SOURCE_DIR}/scripts/calculate_motion_amplitude.sh
  ${CLITK_SOURCE_DIR}/scripts/calculate_contour_stats.sh
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
