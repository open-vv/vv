#=========================================================
INCLUDE(../cmake/common.cmake)

#=========================================================
# make clitk libraries

SET(clitkFilters_SRC
  clitkGuerreroVentilationGenericFilter.cxx
  #clitkBinarizeImage_ggo.c
  #clitkImageArithmGenericFilter.cxx
  clitkImageConvertGenericFilter.cxx
  clitkImageFillRegionGenericFilter.cxx
  clitkImageResampleGenericFilter.cxx
  clitkSplitImageGenericFilter.cxx
  clitkVFResampleGenericFilter.cxx
  clitkBinarizeImageGenericFilter.cxx
  clitkImageArithmGenericFilter.cxx
  clitkMedianImageGenericFilter.cxx
 # clitkMedianImageFilter_ggo.c
#  clitkBinarizeImageGenericFilter.cxx
  clitkBinarizeImage_ggo.c
#  clitkMedianImageGenericFilter.cxx
  clitkMedianImageFilter_ggo.c
  clitkUnsharpMask_ggo.c
  clitkFooImage_ggo.c
  clitkImageArithm_ggo.c
  )  

ADD_LIBRARY(clitkFilters STATIC ${clitkFilters_SRC})
