# Set them in a dart file when using ctest to submit on an other dashboard
# Those are set for running a classical make Experimental
SET_IF_NOT_SET(CTEST_DROP_METHOD "http")
SET_IF_NOT_SET(CTEST_DROP_SITE "my.cdash.org")
SET_IF_NOT_SET(CTEST_DROP_LOCATION "/submit.php?project=VV")
SET_IF_NOT_SET(CTEST_DROP_SITE_CDASH TRUE)
SET_IF_NOT_SET(CTEST_PROJECT_NAME "vv_itk4")
SET_IF_NOT_SET(CTEST_NIGHTLY_START_TIME "1:00:00 UTC")

#=========================================================
SET(CLITK_TEST_DATA_PATH ${PROJECT_BINARY_DIR}/tests/data/ CACHE INTERNAL DOCSTRING)