#=========================================================
INCLUDE(../cmake/common.cmake)

#=========================================================
# make clitk libraries

WRAP_GGO(clitkFilters_GGO_C
  clitkBinarizeImage.ggo
  clitkUnsharpMask.ggo
  clitkFooImage.ggo
  clitkImageArithm.ggo
  clitkMedianImageFilter.ggo
  clitkAutoCrop.ggo
)

SET(clitkFilters_SRC
  ${clitkFilters_GGO_C}
  clitkGuerreroVentilationGenericFilter.cxx
  clitkImageConvertGenericFilter.cxx
  clitkImageFillRegionGenericFilter.cxx
  clitkImageResampleGenericFilter.cxx
  ../tools/clitkResampleImageGenericFilter.cxx
  clitkSplitImageGenericFilter.cxx
  clitkVFResampleGenericFilter.cxx
  clitkMedianImageGenericFilter.cxx
)  

ADD_LIBRARY(clitkFilters STATIC ${clitkFilters_SRC})
