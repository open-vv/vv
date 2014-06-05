macro(SET_IF_NOT_SET var val)
  if(NOT DEFINED "${var}")
    set("${var}" "${val}")
  endif(NOT DEFINED "${var}")
endmacro(SET_IF_NOT_SET)

# Those are set for running a classical make Experimental
SET_IF_NOT_SET(CTEST_DROP_METHOD "http")
SET_IF_NOT_SET(CTEST_DROP_SITE "my.cdash.org")
SET_IF_NOT_SET(CTEST_DROP_LOCATION "/submit.php?project=VV")
SET_IF_NOT_SET(CTEST_DROP_SITE_CDASH TRUE)
SET_IF_NOT_SET(CTEST_PROJECT_NAME "vv_itk4")
