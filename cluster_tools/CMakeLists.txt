if(CLITK_BUILD_TOOLS)
  #=========================================================
  # Install scripts when running make install
  set(SCRIPTS
    gate_job_cluster.job
    gate_make_merge_release.sh
    gate_make_release.sh
    gate_power_merge.sh
    gate_run_submit_cluster.sh
    gate_upload_release.sh
    mergeDosePerEnergyFile.sh
    mergeStatFile.py
    mergeStatFile.sh
    )

  install(FILES ${SCRIPTS} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

endif(CLITK_BUILD_TOOLS)
