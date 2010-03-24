#=========================================================
INCLUDE(../cmake/common.cmake)

#=========================================================
# make clitk libraries

SET(clitkFilters_SRC
  clitkGuerreroVentilationGenericFilter.cxx
  clitkImageArithm_ggo.c
  clitkImageArithmGenericFilter.cxx
  clitkImageConvertGenericFilter.cxx
  clitkImageFillRegionGenericFilter.cxx
  clitkImageResampleGenericFilter.cxx
  clitkSplitImageGenericFilter.cxx
  clitkVFResampleGenericFilter.cxx
  clitkBinarizeImageGenericFilter.cxx
  clitkBinarizeImage_ggo.c
  )  

ADD_LIBRARY(clitkFilters STATIC ${clitkFilters_SRC})
