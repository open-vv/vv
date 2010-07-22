#=========================================================
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
#=========================================================

WRAP_GGO(clitkDicomInfo_GGO_C clitkDicomInfo.ggo)
ADD_EXECUTABLE(clitkDicomInfo clitkDicomInfo.cxx ${clitkDicomInfo_GGO_C})
TARGET_LINK_LIBRARIES(clitkDicomInfo clitkCommon ITKIO) 

WRAP_GGO(clitkImageInfo_GGO_C clitkImageInfo.ggo)
ADD_EXECUTABLE(clitkImageInfo clitkImageInfo.cxx ${clitkImageInfo_GGO_C})
TARGET_LINK_LIBRARIES(clitkImageInfo clitkCommon ITKIO)

WRAP_GGO(clitkImageConvert_GGO_C clitkImageConvert.ggo)
ADD_EXECUTABLE(clitkImageConvert clitkImageConvert.cxx ${clitkImageConvert_GGO_C})
TARGET_LINK_LIBRARIES(clitkImageConvert clitkCommon ITKIO clitkFilters)

# WRAP_GGO(clitkImageResample_GGO_C clitkImageResample.ggo)
# ADD_EXECUTABLE(clitkImageResample clitkImageResample.cxx ${clitkImageResample_GGO_C})
# TARGET_LINK_LIBRARIES(clitkImageResample clitkCommon ITKIO clitkFilters)

WRAP_GGO(clitkVFResample_GGO_C clitkVFResample.ggo)
ADD_EXECUTABLE(clitkVFResample clitkVFResample.cxx ${clitkImageResample_GGO_C})
TARGET_LINK_LIBRARIES(clitkVFResample clitkCommon ITKIO clitkFilters)

WRAP_GGO(clitkImageCreate_GGO_C clitkImageCreate.ggo)
ADD_EXECUTABLE(clitkImageCreate clitkImageCreate.cxx ${clitkImageCreate_GGO_C})
TARGET_LINK_LIBRARIES(clitkImageCreate clitkCommon ITKStatistics ITKIO)

WRAP_GGO(clitkImageFillRegion_GGO_C clitkImageFillRegion.ggo)
ADD_EXECUTABLE(clitkImageFillRegion clitkImageFillRegion.cxx ${clitkImageFillRegion_GGO_C})
TARGET_LINK_LIBRARIES(clitkImageFillRegion clitkCommon ITKIO ITKStatistics clitkFilters)

WRAP_GGO(clitkZeroVF_GGO_C clitkZeroVF.ggo)
ADD_EXECUTABLE(clitkZeroVF clitkZeroVF.cxx ${clitkZeroVF_GGO_C} clitkZeroVFGenericFilter.cxx)
TARGET_LINK_LIBRARIES(clitkZeroVF clitkCommon ITKStatistics ITKIO)

WRAP_GGO(clitkImageExtractLine_GGO_C clitkImageExtractLine.ggo)
ADD_EXECUTABLE(clitkImageExtractLine clitkImageExtractLine.cxx ${clitkImageExtractLine_GGO_C})
TARGET_LINK_LIBRARIES(clitkImageExtractLine clitkCommon ITKStatistics ITKIO)

WRAP_GGO(clitkSplitImage_GGO_C clitkSplitImage.ggo)
ADD_EXECUTABLE(clitkSplitImage clitkSplitImage.cxx ${clitkSplitImage_GGO_C})
TARGET_LINK_LIBRARIES(clitkSplitImage ITKBasicFilters clitkCommon ITKIO clitkFilters)

WRAP_GGO(clitkVFMerge_GGO_C clitkVFMerge.ggo)
ADD_EXECUTABLE(clitkVFMerge clitkVFMerge.cxx ${clitkVFMerge_GGO_C})
TARGET_LINK_LIBRARIES(clitkVFMerge ITKBasicFilters clitkCommon ITKIO)

WRAP_GGO(clitkWriteDicomSeries_GGO_C clitkWriteDicomSeries.ggo)
ADD_EXECUTABLE(clitkWriteDicomSeries clitkWriteDicomSeries.cxx ${clitkWriteDicomSeries_GGO_C})
TARGET_LINK_LIBRARIES(clitkWriteDicomSeries clitkCommon ITKIO clitkFilters)

WRAP_GGO(clitkAverageTemporalDimension_GGO_C clitkAverageTemporalDimension.ggo)
ADD_EXECUTABLE(clitkAverageTemporalDimension clitkAverageTemporalDimension.cxx ${clitkAverageTemporalDimension_GGO_C})
TARGET_LINK_LIBRARIES(clitkAverageTemporalDimension clitkCommon ITKIO clitkFilters)

WRAP_GGO(clitkWarpImage_GGO_C clitkWarpImage.ggo)
ADD_EXECUTABLE(clitkWarpImage clitkWarpImage.cxx ${clitkWarpImage_GGO_C} clitkWarpImageGenericFilter.cxx)
TARGET_LINK_LIBRARIES(clitkWarpImage ITKBasicFilters clitkCommon ITKIO)

WRAP_GGO(clitkInvertVF_GGO_C clitkInvertVF.ggo)
ADD_EXECUTABLE(clitkInvertVF clitkInvertVF.cxx ${clitkInvertVF_GGO_C})
TARGET_LINK_LIBRARIES(clitkInvertVF clitkCommon ITKIO clitkFilters)

WRAP_GGO(clitkAffineTransform_GGO_C clitkAffineTransform.ggo)
ADD_EXECUTABLE(clitkAffineTransform clitkAffineTransform.cxx ${clitkAffineTransform_GGO_C})
TARGET_LINK_LIBRARIES(clitkAffineTransform clitkCommon ITKIO clitkFilters)

WRAP_GGO(clitkSetBackground_GGO_C clitkSetBackground.ggo)
ADD_EXECUTABLE(clitkSetBackground clitkSetBackground.cxx clitkSetBackgroundGenericFilter.cxx ${clitkSetBackground_GGO_C})
TARGET_LINK_LIBRARIES(clitkSetBackground clitkCommon ITKIO)

WRAP_GGO(clitkGuerreroVentilation_GGO_C clitkGuerreroVentilation.ggo)
ADD_EXECUTABLE(clitkGuerreroVentilation clitkGuerreroVentilation.cxx ${clitkGuerreroVentilation_GGO_C})
TARGET_LINK_LIBRARIES(clitkGuerreroVentilation clitkCommon ITKIO ITKBasicFilters clitkFilters)

ADD_EXECUTABLE(clitkImageArithm clitkImageArithm.cxx)
TARGET_LINK_LIBRARIES(clitkImageArithm clitkCommon ITKIO ITKStatistics clitkFilters)

ADD_EXECUTABLE(clitkUnsharpMask clitkUnsharpMask.cxx)
TARGET_LINK_LIBRARIES(clitkUnsharpMask clitkCommon ITKIO clitkFilters) 

ADD_EXECUTABLE(clitkFooImage clitkFooImage.cxx)
TARGET_LINK_LIBRARIES(clitkFooImage clitkCommon ITKIO clitkFilters) 

ADD_EXECUTABLE(clitkBinarizeImage clitkBinarizeImage.cxx)
TARGET_LINK_LIBRARIES(clitkBinarizeImage clitkCommon ITKIO clitkFilters) 

ADD_EXECUTABLE(clitkMedianImageFilter clitkMedianImageFilter.cxx)
TARGET_LINK_LIBRARIES(clitkMedianImageFilter clitkCommon ITKIO ITKBasicFilters clitkFilters)

WRAP_GGO(clitkResampleImage_GGO_C clitkResampleImage.ggo)
ADD_EXECUTABLE(clitkResampleImage clitkResampleImage.cxx clitkResampleImageGenericFilter.cxx ${clitkResampleImage_GGO_C})
TARGET_LINK_LIBRARIES(clitkResampleImage clitkCommon ITKIO)

WRAP_GGO(clitkMinMaxMask_GGO_C clitkMinMaxMask.ggo)
ADD_EXECUTABLE(clitkMinMaxMask clitkMinMaxMask.cxx ${clitkMinMaxMask_GGO_C})
TARGET_LINK_LIBRARIES(clitkMinMaxMask clitkCommon ITKIO ITKStatistics clitkFilters)

ADD_EXECUTABLE(clitkAutoCrop clitkAutoCrop.cxx)
TARGET_LINK_LIBRARIES(clitkAutoCrop clitkCommon ITKIO ITKStatistics clitkFilters)

WRAP_GGO(clitkDicomRTStruct2BinaryImage_GGO_C clitkDicomRTStruct2BinaryImage.ggo)
ADD_EXECUTABLE(clitkDicomRTStruct2BinaryImage clitkDicomRT_ROI_ConvertToImageFilter.cxx clitkDicomRTStruct2BinaryImage.cxx ${clitkDicomRTStruct2BinaryImage_GGO_C})
TARGET_LINK_LIBRARIES(clitkDicomRTStruct2BinaryImage ITKCommon clitkDicomRTStruct clitkCommon ITKIO QVTK)

