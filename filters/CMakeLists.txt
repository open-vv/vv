#=========================================================

#=========================================================
# make clitk libraries

SET(    clitkFilters_SRC
clitkGuerreroVentilationGenericFilter.cxx
clitkImageArithmGenericFilter.cxx
clitkImageConvertGenericFilter.cxx
clitkImageFillRegionGenericFilter.cxx
clitkImageResampleGenericFilter.cxx
clitkSplitImageGenericFilter.cxx
clitkVFResampleGenericFilter.cxx
)  

ADD_LIBRARY(clitkFilters STATIC ${clitkFilters_SRC})
