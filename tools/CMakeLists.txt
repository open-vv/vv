
#=========================================================
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
#=========================================================

SET(clitkGGO_SRC
  clitkImageResample_ggo.c
  clitkBinarizeImage_ggo.c
  )  

ADD_LIBRARY(clitkGGO STATIC ${clitkGGO_SRC})

ADD_EXECUTABLE(clitkDicomInfo clitkDicomInfo.cxx clitkDicomInfo_ggo.c)
TARGET_LINK_LIBRARIES(clitkDicomInfo ITKIO clitkCommon) 

ADD_EXECUTABLE(clitkImageInfo clitkImageInfo.cxx clitkImageInfo_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageInfo ITKIO clitkCommon)

ADD_EXECUTABLE(clitkImageConvert clitkImageConvert.cxx clitkImageConvert_ggo.c  )
TARGET_LINK_LIBRARIES(clitkImageConvert ITKIO clitkCommon clitkFilters)

ADD_EXECUTABLE(clitkImageResample clitkImageResample.cxx   clitkImageResample_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageResample ITKIO clitkCommon clitkFilters)

ADD_EXECUTABLE(clitkVFResample clitkVFResample.cxx   clitkImageResample_ggo.c)
TARGET_LINK_LIBRARIES(clitkVFResample ITKIO clitkCommon clitkFilters)

ADD_EXECUTABLE(clitkImageArithm clitkImageArithm.cxx   clitkImageArithm_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageArithm ITKIO ITKStatistics clitkCommon clitkFilters)

ADD_EXECUTABLE(clitkImageCreate clitkImageCreate.cxx clitkImageCreate_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageCreate ITKIO ITKStatistics clitkCommon)

ADD_EXECUTABLE(clitkImageFillRegion clitkImageFillRegion.cxx   clitkImageFillRegion_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageFillRegion ITKIO ITKStatistics clitkCommon
clitkFilters)

ADD_EXECUTABLE(clitkZeroVF clitkZeroVF.cxx   clitkZeroVF_ggo.c
clitkZeroVFGenericFilter.cxx)
TARGET_LINK_LIBRARIES(clitkZeroVF ITKIO ITKStatistics clitkCommon )

ADD_EXECUTABLE(clitkImageExtractLine clitkImageExtractLine.cxx clitkImageExtractLine_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageExtractLine ITKIO ITKStatistics clitkCommon)

ADD_EXECUTABLE(clitkSplitImage clitkSplitImage.cxx clitkSplitImage_ggo.c
 )
TARGET_LINK_LIBRARIES(clitkSplitImage ITKBasicFilters ITKIO clitkCommon
clitkFilters)

ADD_EXECUTABLE(clitkVFMerge clitkVFMerge.cxx clitkVFMerge_ggo.c)
TARGET_LINK_LIBRARIES(clitkVFMerge ITKBasicFilters ITKIO clitkCommon )

ADD_EXECUTABLE(clitkWriteDicomSeries clitkWriteDicomSeries.cxx  clitkWriteDicomSeries_ggo.c)
TARGET_LINK_LIBRARIES(clitkWriteDicomSeries ITKIO clitkCommon clitkFilters)

ADD_EXECUTABLE(clitkAverageTemporalDimension clitkAverageTemporalDimension.cxx  clitkAverageTemporalDimension_ggo.c)
TARGET_LINK_LIBRARIES(clitkAverageTemporalDimension ITKIO clitkCommon
clitkFilters)

ADD_EXECUTABLE(clitkWarpImage  clitkWarpImage.cxx clitkWarpImage_ggo.c
clitkWarpImageGenericFilter.cxx)
TARGET_LINK_LIBRARIES(clitkWarpImage ITKBasicFilters ITKIO clitkCommon
)

ADD_EXECUTABLE(clitkInvertVF clitkInvertVF.cxx  clitkInvertVF_ggo.c)
TARGET_LINK_LIBRARIES(clitkInvertVF ITKIO clitkCommon clitkFilters)

ADD_EXECUTABLE(clitkAffineTransform clitkAffineTransform.cxx  clitkAffineTransform_ggo.c)
TARGET_LINK_LIBRARIES(clitkAffineTransform clitkCommon ITKIO  clitkFilters)

ADD_EXECUTABLE(clitkSignalMeanPositionTracking clitkSignalMeanPositionTracking.cxx clitkSignalMeanPositionFilter.cxx clitkEllipse.cxx clitkSignalMeanPositionTracking_ggo.c)
TARGET_LINK_LIBRARIES(clitkSignalMeanPositionTracking clitkCommon ITKIO fftw3)

ADD_EXECUTABLE(clitkSignalFilter clitkSignalFilter.cxx clitkSignalFilter_ggo.c)
TARGET_LINK_LIBRARIES(clitkSignalFilter ITKIO clitkCommon fftw3) 

ADD_EXECUTABLE(clitkSetBackground clitkSetBackground.cxx
clitkSetBackgroundGenericFilter.cxx clitkSetBackground_ggo.c)
TARGET_LINK_LIBRARIES(clitkSetBackground clitkCommon ITKIO )

ADD_EXECUTABLE(clitkGuerreroVentilation clitkGuerreroVentilation.cxx
 clitkGuerreroVentilation_ggo.c
)
TARGET_LINK_LIBRARIES(clitkGuerreroVentilation clitkCommon ITKIO ITKBasicFilters
clitkFilters)

#ADD_EXECUTABLE(clitkBinarizeImage clitkBinarizeImage.cxx clitkBinarizeImage_ggo.c)
#TARGET_LINK_LIBRARIES(clitkBinarizeImage ITKIO clitkCommon) 
