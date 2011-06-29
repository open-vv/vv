set(CTEST_PROJECT_NAME "vv_itk4")
set(CTEST_NIGHTLY_START_TIME "1:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=VV")
set(CTEST_DROP_SITE_CDASH TRUE)


#=========================================================
SET(CLITK_TEST_DATA_PATH ${PROJECT_BINARY_DIR}/tests/data CACHE INTERNAL DOCSTRING)
# Get the data tests
IF(WIN32)
  SET(scriptExt bat)
ELSE(UNIX)
  SET(scriptExt sh)
ENDIF()
EXEC_PROGRAM(${PROJECT_SOURCE_DIR}/tests/fetch_data_test.${scriptExt} ARGS ${CLITK_TEST_DATA_PATH} http://russule/data/.git)