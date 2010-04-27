#=========================================================
INCLUDE(../cmake/common.cmake)

#=========================================================
# make clitk libraries

SET(clitkFilters_SRC
  clitkGuerreroVentilationGenericFilter.cxx
  clitkBinarizeImage_ggo.c
  clitkImageConvertGenericFilter.cxx
  clitkImageFillRegionGenericFilter.cxx
  clitkImageResampleGenericFilter.cxx
  clitkSplitImageGenericFilter.cxx
  clitkVFResampleGenericFilter.cxx
  clitkMedianImageFilter_ggo.c
  clitkMedianImageGenericFilter.cxx
  clitkUnsharpMask_ggo.c
  clitkFooImage_ggo.c
  clitkImageArithm_ggo.c
  )  

ADD_LIBRARY(clitkFilters STATIC ${clitkFilters_SRC})
