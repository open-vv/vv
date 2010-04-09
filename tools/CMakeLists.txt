
#=========================================================
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
#=========================================================


ADD_EXECUTABLE(clitkDicomInfo clitkDicomInfo.cxx clitkDicomInfo_ggo.c)
TARGET_LINK_LIBRARIES(clitkDicomInfo clitkCommon ITKIO) 

ADD_EXECUTABLE(clitkImageInfo clitkImageInfo.cxx clitkImageInfo_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageInfo clitkCommon ITKIO)

ADD_EXECUTABLE(clitkImageConvert clitkImageConvert.cxx clitkImageConvert_ggo.c  )
TARGET_LINK_LIBRARIES(clitkImageConvert clitkCommon ITKIO clitkFilters)

ADD_EXECUTABLE(clitkImageResample clitkImageResample.cxx   clitkImageResample_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageResample clitkCommon ITKIO clitkFilters)

ADD_EXECUTABLE(clitkVFResample clitkVFResample.cxx   clitkImageResample_ggo.c)
TARGET_LINK_LIBRARIES(clitkVFResample clitkCommon ITKIO clitkFilters)

ADD_EXECUTABLE(clitkImageCreate clitkImageCreate.cxx clitkImageCreate_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageCreate clitkCommon ITKStatistics ITKIO)

ADD_EXECUTABLE(clitkImageFillRegion clitkImageFillRegion.cxx   clitkImageFillRegion_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageFillRegion clitkCommon
ITKIO ITKStatistics clitkFilters)

ADD_EXECUTABLE(clitkZeroVF clitkZeroVF.cxx   clitkZeroVF_ggo.c
clitkZeroVFGenericFilter.cxx)
TARGET_LINK_LIBRARIES(clitkZeroVF clitkCommon ITKStatistics ITKIO )

ADD_EXECUTABLE(clitkImageExtractLine clitkImageExtractLine.cxx clitkImageExtractLine_ggo.c)
TARGET_LINK_LIBRARIES(clitkImageExtractLine clitkCommon ITKStatistics ITKIO)

ADD_EXECUTABLE(clitkSplitImage clitkSplitImage.cxx clitkSplitImage_ggo.c
 )
TARGET_LINK_LIBRARIES(clitkSplitImage ITKBasicFilters clitkCommon ITKIO
clitkFilters)

ADD_EXECUTABLE(clitkVFMerge clitkVFMerge.cxx clitkVFMerge_ggo.c)
TARGET_LINK_LIBRARIES(clitkVFMerge ITKBasicFilters clitkCommon ITKIO )

ADD_EXECUTABLE(clitkWriteDicomSeries clitkWriteDicomSeries.cxx  clitkWriteDicomSeries_ggo.c)
TARGET_LINK_LIBRARIES(clitkWriteDicomSeries clitkCommon ITKIO clitkFilters)

ADD_EXECUTABLE(clitkAverageTemporalDimension clitkAverageTemporalDimension.cxx  clitkAverageTemporalDimension_ggo.c)
TARGET_LINK_LIBRARIES(clitkAverageTemporalDimension clitkCommon ITKIO
clitkFilters)

ADD_EXECUTABLE(clitkWarpImage  clitkWarpImage.cxx clitkWarpImage_ggo.c
clitkWarpImageGenericFilter.cxx)
TARGET_LINK_LIBRARIES(clitkWarpImage ITKBasicFilters clitkCommon ITKIO
)

ADD_EXECUTABLE(clitkInvertVF clitkInvertVF.cxx  clitkInvertVF_ggo.c)
TARGET_LINK_LIBRARIES(clitkInvertVF clitkCommon ITKIO clitkFilters)

ADD_EXECUTABLE(clitkAffineTransform clitkAffineTransform.cxx  clitkAffineTransform_ggo.c)
TARGET_LINK_LIBRARIES(clitkAffineTransform clitkCommon ITKIO  clitkFilters)

ADD_EXECUTABLE(clitkSetBackground clitkSetBackground.cxx
clitkSetBackgroundGenericFilter.cxx clitkSetBackground_ggo.c)
TARGET_LINK_LIBRARIES(clitkSetBackground clitkCommon ITKIO )

ADD_EXECUTABLE(clitkGuerreroVentilation clitkGuerreroVentilation.cxx
 clitkGuerreroVentilation_ggo.c
)
TARGET_LINK_LIBRARIES(clitkGuerreroVentilation clitkCommon ITKIO ITKBasicFilters
clitkFilters)

ADD_EXECUTABLE(clitkImageArithm clitkImageArithm.cxx)
TARGET_LINK_LIBRARIES(clitkImageArithm clitkCommon ITKIO ITKStatistics clitkFilters)

ADD_EXECUTABLE(clitkFooImage clitkFooImage.cxx)
TARGET_LINK_LIBRARIES(clitkFooImage clitkCommon ITKIO clitkFilters) 

ADD_EXECUTABLE(clitkBinarizeImage clitkBinarizeImage.cxx)
TARGET_LINK_LIBRARIES(clitkBinarizeImage clitkCommon ITKIO clitkFilters) 

ADD_EXECUTABLE(clitkMedianImageFilter clitkMedianImageFilter.cxx)
TARGET_LINK_LIBRARIES(clitkMedianImageFilter clitkCommon ITKIO clitkFilters ITKBasicFilters)
