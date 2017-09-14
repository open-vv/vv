if(CLITK_BUILD_TOOLS)
  #=========================================================
  # Install scripts when running make install
  set(SCRIPTS
    gate_job_cluster.job
    gate_job_ccin2p3.job
    gate_make_merge_release.sh
    gate_make_release.sh
    gate_power_merge.sh
    gate_run_submit_cluster.sh
    gate_upload_release.sh
    mergeDosePerEnergyFile.sh
    mergeStatFile.py
    mergeStatFile.sh
    gate_run_submit_cluster_nomove.sh
    gate_job_cluster_nomove.job
    check_tmp_cluster.sh
    )

  install(FILES ${SCRIPTS} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

endif(CLITK_BUILD_TOOLS)
