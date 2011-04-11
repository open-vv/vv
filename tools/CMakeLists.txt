#=========================================================
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
#=========================================================

# Add Libraries used in vv and clitk to avoid recompilation
WRAP_GGO(clitkCropImage_GGO_C clitkCropImage.ggo)
ADD_LIBRARY(clitkCropImageLib clitkCropImageGenericFilter.cxx ${clitkCropImage_GGO_C})

WRAP_GGO(clitkImageConvert_GGO_C clitkImageConvert.ggo)
ADD_LIBRARY(clitkImageConvertLib clitkImageConvertGenericFilter.cxx ${clitkImageConvert_GGO_C})

WRAP_GGO(clitkBinarizeImage_GGO_C clitkBinarizeImage.ggo)
ADD_LIBRARY(clitkBinarizeImageLib clitkBinarizeImageGenericFilter.cxx ${clitkBinarizeImage_GGO_C})

WRAP_GGO(clitkImageArithm_GGO_C clitkImageArithm.ggo)
ADD_LIBRARY(clitkImageArithmImageLib clitkImageArithmGenericFilter.cxx ${clitkImageArithm_GGO_C})

WRAP_GGO(clitkResampleImage_GGO_C clitkResampleImage.ggo)
ADD_LIBRARY(clitkResampleImageLib clitkResampleImageGenericFilter.cxx ${clitkResampleImage_GGO_C})

WRAP_GGO(clitkMIP_GGO_C clitkMIP.ggo)
ADD_LIBRARY(clitkMIPLib clitkMIPGenericFilter.cxx ${clitkMIP_GGO_C})

IF (CLITK_BUILD_TOOLS)
    WRAP_GGO(clitkDicomInfo_GGO_C clitkDicomInfo.ggo)
    ADD_EXECUTABLE(clitkDicomInfo clitkDicomInfo.cxx ${clitkDicomInfo_GGO_C})
    TARGET_LINK_LIBRARIES(clitkDicomInfo clitkCommon ITKIO)

    WRAP_GGO(clitkDicom2Image_GGO_C clitkDicom2Image.ggo)
    ADD_EXECUTABLE(clitkDicom2Image clitkDicom2Image.cxx ${clitkDicom2Image_GGO_C})
    TARGET_LINK_LIBRARIES(clitkDicom2Image clitkCommon ITKIO)

    WRAP_GGO(clitkImageInfo_GGO_C clitkImageInfo.ggo)
    ADD_EXECUTABLE(clitkImageInfo clitkImageInfo.cxx ${clitkImageInfo_GGO_C})
    TARGET_LINK_LIBRARIES(clitkImageInfo clitkCommon ITKIO)

    ADD_EXECUTABLE(clitkImageConvert clitkImageConvert.cxx)
    TARGET_LINK_LIBRARIES(clitkImageConvert clitkImageConvertLib clitkCommon ITKIO)

    ADD_EXECUTABLE(clitkBinarizeImage clitkBinarizeImage.cxx)
    TARGET_LINK_LIBRARIES(clitkBinarizeImage clitkBinarizeImageLib clitkCommon ITKIO)

    WRAP_GGO(clitkVFResample_GGO_C clitkVFResample.ggo)
    ADD_EXECUTABLE(clitkVFResample clitkVFResample.cxx clitkVFResampleGenericFilter.cxx ${clitkVFResample_GGO_C})
    TARGET_LINK_LIBRARIES(clitkVFResample clitkCommon ITKIO)

    WRAP_GGO(clitkImageCreate_GGO_C clitkImageCreate.ggo)
    ADD_EXECUTABLE(clitkImageCreate clitkImageCreate.cxx ${clitkImageCreate_GGO_C})
    TARGET_LINK_LIBRARIES(clitkImageCreate clitkCommon  ITKIO)

    WRAP_GGO(clitkZeroVF_GGO_C clitkZeroVF.ggo)
    ADD_EXECUTABLE(clitkZeroVF clitkZeroVF.cxx ${clitkZeroVF_GGO_C} clitkZeroVFGenericFilter.cxx)
    TARGET_LINK_LIBRARIES(clitkZeroVF clitkCommon  ITKIO)

    WRAP_GGO(clitkImageExtractLine_GGO_C clitkImageExtractLine.ggo)
    ADD_EXECUTABLE(clitkImageExtractLine clitkImageExtractLine.cxx ${clitkImageExtractLine_GGO_C})
    TARGET_LINK_LIBRARIES(clitkImageExtractLine clitkCommon  ITKIO)

    WRAP_GGO(clitkSplitImage_GGO_C clitkSplitImage.ggo)
    ADD_EXECUTABLE(clitkSplitImage clitkSplitImage.cxx clitkSplitImageGenericFilter.cxx ${clitkSplitImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkSplitImage clitkCommon ITKIO )

    WRAP_GGO(clitkVFMerge_GGO_C clitkVFMerge.ggo)
    ADD_EXECUTABLE(clitkVFMerge clitkVFMerge.cxx ${clitkVFMerge_GGO_C})
    TARGET_LINK_LIBRARIES(clitkVFMerge ITKBasicFilters clitkCommon ITKIO)

    WRAP_GGO(clitkWriteDicomSeries_GGO_C clitkWriteDicomSeries.ggo)
    ADD_EXECUTABLE(clitkWriteDicomSeries clitkWriteDicomSeries.cxx ${clitkWriteDicomSeries_GGO_C})
    TARGET_LINK_LIBRARIES(clitkWriteDicomSeries clitkCommon ITKIO )

    WRAP_GGO(clitkAverageTemporalDimension_GGO_C clitkAverageTemporalDimension.ggo)
    ADD_EXECUTABLE(clitkAverageTemporalDimension clitkAverageTemporalDimension.cxx ${clitkAverageTemporalDimension_GGO_C})
    TARGET_LINK_LIBRARIES(clitkAverageTemporalDimension clitkCommon ITKIO )

    WRAP_GGO(clitkMedianTemporalDimension_GGO_C clitkMedianTemporalDimension.ggo)
    ADD_EXECUTABLE(clitkMedianTemporalDimension clitkMedianTemporalDimension.cxx
        ${clitkMedianTemporalDimension_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMedianTemporalDimension clitkCommon ITKIO )

    WRAP_GGO(clitkWarpImage_GGO_C clitkWarpImage.ggo)
    ADD_EXECUTABLE(clitkWarpImage clitkWarpImage.cxx ${clitkWarpImage_GGO_C} clitkWarpImageGenericFilter.cxx)
    TARGET_LINK_LIBRARIES(clitkWarpImage ITKBasicFilters clitkCommon ITKIO)

    WRAP_GGO(clitkInvertVF_GGO_C clitkInvertVF.ggo)
    ADD_EXECUTABLE(clitkInvertVF clitkInvertVF.cxx ${clitkInvertVF_GGO_C})
    TARGET_LINK_LIBRARIES(clitkInvertVF clitkCommon ITKIO )

    WRAP_GGO(clitkAffineTransform_GGO_C clitkAffineTransform.ggo)
    ADD_EXECUTABLE(clitkAffineTransform clitkAffineTransform.cxx ${clitkAffineTransform_GGO_C})
    TARGET_LINK_LIBRARIES(clitkAffineTransform clitkCommon ITKIO )

    WRAP_GGO(clitkSetBackground_GGO_C clitkSetBackground.ggo)
    ADD_EXECUTABLE(clitkSetBackground clitkSetBackground.cxx clitkSetBackgroundGenericFilter.cxx ${clitkSetBackground_GGO_C})
    TARGET_LINK_LIBRARIES(clitkSetBackground clitkCommon ITKIO)

    WRAP_GGO(clitkGuerreroVentilation_GGO_C clitkGuerreroVentilation.ggo)
    ADD_EXECUTABLE(clitkGuerreroVentilation clitkGuerreroVentilation.cxx clitkGuerreroVentilationGenericFilter.cxx ${clitkGuerreroVentilation_GGO_C})
    TARGET_LINK_LIBRARIES(clitkGuerreroVentilation clitkCommon ITKIO)

    WRAP_GGO(clitkGammaIndex_GGO_C clitkGammaIndex.ggo)
    ADD_EXECUTABLE(clitkGammaIndex clitkGammaIndex.cxx ${clitkGammaIndex_GGO_C})
    TARGET_LINK_LIBRARIES(clitkGammaIndex vtkCommon vtkFiltering vtkGraphics vtkIO vtkImaging)

    ADD_EXECUTABLE(clitkImageArithm clitkImageArithm.cxx)
    TARGET_LINK_LIBRARIES(clitkImageArithm clitkImageArithmImageLib clitkCommon ITKIO )

    WRAP_GGO(clitkUnsharpMask_GGO_C clitkUnsharpMask.ggo)
    ADD_EXECUTABLE(clitkUnsharpMask clitkUnsharpMask.cxx ${clitkUnsharpMask_GGO_C})
    TARGET_LINK_LIBRARIES(clitkUnsharpMask clitkCommon ITKIO ) 

    WRAP_GGO(clitkFooImage_GGO_C clitkFooImage.ggo)
    ADD_EXECUTABLE(clitkFooImage clitkFooImage.cxx ${clitkFooImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkFooImage clitkCommon ITKIO ) 

    WRAP_GGO(clitkMedianImageFilter_GGO_C clitkMedianImageFilter.ggo)
    ADD_EXECUTABLE(clitkMedianImageFilter clitkMedianImageFilter.cxx ${clitkMedianImageFilter_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMedianImageFilter clitkCommon ITKIO ITKBasicFilters )

    ADD_EXECUTABLE(clitkResampleImage clitkResampleImage.cxx)
    TARGET_LINK_LIBRARIES(clitkResampleImage clitkResampleImageLib clitkCommon ITKIO)

    WRAP_GGO(clitkMinMaxMask_GGO_C clitkMinMaxMask.ggo)
    ADD_EXECUTABLE(clitkMinMaxMask clitkMinMaxMask.cxx ${clitkMinMaxMask_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMinMaxMask clitkCommon ITKIO  )

    # WRAP_GGO(clitkAutoCrop_GGO_C clitkAutoCrop.ggo)
    # ADD_EXECUTABLE(clitkAutoCrop clitkAutoCrop.cxx ${clitkAutoCrop_GGO_C})
    # TARGET_LINK_LIBRARIES(clitkAutoCrop clitkCommon ITKIO )

    WRAP_GGO(clitkDicomRTStruct2BinaryImage_GGO_C clitkDicomRTStruct2BinaryImage.ggo)
    ADD_EXECUTABLE(clitkDicomRTStruct2BinaryImage clitkDicomRTStruct2BinaryImage.cxx ${clitkDicomRTStruct2BinaryImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkDicomRTStruct2BinaryImage ITKCommon clitkDicomRTStruct clitkCommon ITKIO )

    WRAP_GGO(clitkImageLog_GGO_C clitkImageLog.ggo)
    ADD_EXECUTABLE(clitkImageLog clitkImageLog.cxx ${clitkImageLog_GGO_C})
    TARGET_LINK_LIBRARIES(clitkImageLog  ITKIO clitkCommon)

    WRAP_GGO(clitkFilter_GGO_C clitkFilter.ggo)
    ADD_EXECUTABLE(clitkFilter clitkFilter.cxx clitkFilterGenericFilter.cxx ${clitkFilter_GGO_C})
    TARGET_LINK_LIBRARIES(clitkFilter ITKIO clitkCommon)

    WRAP_GGO(clitkConeBeamProjectImage_GGO_C clitkConeBeamProjectImage.ggo)
    ADD_EXECUTABLE(clitkConeBeamProjectImage clitkConeBeamProjectImage.cxx clitkConeBeamProjectImageGenericFilter.cxx ${clitkConeBeamProjectImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkConeBeamProjectImage  ITKIO clitkCommon )

    WRAP_GGO(clitkComposeVF_GGO_C clitkComposeVF.ggo)
    ADD_EXECUTABLE(clitkComposeVF clitkComposeVFGenericFilter.cxx clitkComposeVF.cxx ${clitkComposeVF_GGO_C})
    TARGET_LINK_LIBRARIES(clitkComposeVF ITKBasicFilters ITKIO clitkCommon)

    WRAP_GGO(clitkMergeSequence_GGO_C clitkMergeSequence.ggo)
    ADD_EXECUTABLE(clitkMergeSequence clitkMergeSequence.cxx clitkMergeSequenceGenericFilter.cxx ${clitkMergeSequence_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMergeSequence ITKIO clitkCommon)

    WRAP_GGO(clitkBackProjectImage_GGO_C clitkBackProjectImage.ggo)
    ADD_EXECUTABLE(clitkBackProjectImage clitkBackProjectImage.cxx clitkBackProjectImageGenericFilter.cxx ${clitkBackProjectImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkBackProjectImage ITKIO clitkCommon )

    ADD_EXECUTABLE(clitkCropImage clitkCropImage.cxx ${clitkCropImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkCropImage clitkCropImageLib clitkCommon ITKIO)

    # WRAP_GGO(clitkExtractSlice_GGO_C clitkExtractSlice.ggo)
    # ADD_EXECUTABLE(clitkExtractSlice clitkExtractSlice.cxx clitkExtractSliceGenericFilter.cxx ${clitkExtractSlice_GGO_C})
    # TARGET_LINK_LIBRARIES(clitkExtractSlice clitkCommon ITKIO)
    
    WRAP_GGO(clitkFlipImage_GGO_C clitkFlipImage.ggo)
    ADD_EXECUTABLE(clitkFlipImage clitkFlipImage.cxx clitkFlipImageGenericFilter.cxx ${clitkFlipImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkFlipImage clitkCommon ITKIO)

    WRAP_GGO(clitkMirrorPadImage_GGO_C clitkMirrorPadImage.ggo)
    ADD_EXECUTABLE(clitkMirrorPadImage clitkMirrorPadImage.cxx clitkMirrorPadImageGenericFilter.cxx ${clitkMirrorPadImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMirrorPadImage clitkCommon ITKIO)

    WRAP_GGO(clitkImageMoment_GGO_C clitkImageMoment.ggo)
    ADD_EXECUTABLE(clitkImageMoment clitkImageMoment.cxx clitkImageMomentGenericFilter.cxx ${clitkImageMoment_GGO_C})
    TARGET_LINK_LIBRARIES(clitkImageMoment clitkCommon ITKIO)

    WRAP_GGO(clitkImageStatistics_GGO_C clitkImageStatistics.ggo)
    ADD_EXECUTABLE(clitkImageStatistics clitkImageStatistics.cxx clitkImageStatisticsGenericFilter.cxx ${clitkImageStatistics_GGO_C})
    TARGET_LINK_LIBRARIES(clitkImageStatistics clitkCommon ITKIO ITKStatistics)

    WRAP_GGO(clitkSetOrigin_GGO_C clitkSetOrigin.ggo)
    ADD_EXECUTABLE(clitkSetOrigin clitkSetOrigin.cxx clitkSetOriginGenericFilter.cxx ${clitkSetOrigin_GGO_C})
    TARGET_LINK_LIBRARIES(clitkSetOrigin clitkCommon ITKIO )
     
    WRAP_GGO(clitkGetOrigin_GGO_C clitkGetOrigin.ggo)
    ADD_EXECUTABLE(clitkGetOrigin clitkGetOrigin.cxx clitkGetOriginGenericFilter.cxx ${clitkGetOrigin_GGO_C})
    TARGET_LINK_LIBRARIES(clitkGetOrigin clitkCommon ITKIO )

    WRAP_GGO(clitkGetDirection_GGO_C clitkGetDirection.ggo)
    ADD_EXECUTABLE(clitkGetDirection clitkGetDirection.cxx clitkGetDirectionGenericFilter.cxx ${clitkGetDirection_GGO_C})
    TARGET_LINK_LIBRARIES(clitkGetDirection clitkCommon ITKIO)

    WRAP_GGO(clitkSetDirection_GGO_C clitkSetDirection.ggo)
    ADD_EXECUTABLE(clitkSetDirection clitkSetDirection.cxx clitkSetDirectionGenericFilter.cxx ${clitkSetDirection_GGO_C})
    TARGET_LINK_LIBRARIES(clitkSetDirection clitkCommon ITKIO)

    WRAP_GGO(clitkGetSize_GGO_C clitkGetSize.ggo)
    ADD_EXECUTABLE(clitkGetSize  clitkGetSize.cxx clitkGetSizeGenericFilter.cxx ${clitkGetSize_GGO_C})
    TARGET_LINK_LIBRARIES(clitkGetSize  clitkCommon   ITKIO)

    WRAP_GGO(clitkGetSpacing_GGO_C clitkGetSpacing.ggo)
    ADD_EXECUTABLE(clitkGetSpacing  clitkGetSpacing.cxx clitkGetSpacingGenericFilter.cxx ${clitkGetSpacing_GGO_C})
    TARGET_LINK_LIBRARIES(clitkGetSpacing  clitkCommon   ITKIO)

    WRAP_GGO(clitkSetSpacing_GGO_C clitkSetSpacing.ggo)
    ADD_EXECUTABLE(clitkSetSpacing  clitkSetSpacing.cxx clitkSetSpacingGenericFilter.cxx ${clitkSetSpacing_GGO_C})
    TARGET_LINK_LIBRARIES(clitkSetSpacing  clitkCommon   ITKIO)

    WRAP_GGO(clitkCombineImage_GGO_C clitkCombineImage.ggo)
    ADD_EXECUTABLE(clitkCombineImage clitkCombineImage.cxx clitkCombineImageGenericFilter.cxx ${clitkCombineImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkCombineImage clitkCommon ITKIO)

    WRAP_GGO(clitkPermuteAxes_GGO_C clitkPermuteAxes.ggo)
    ADD_EXECUTABLE(clitkPermuteAxes clitkPermuteAxes.cxx clitkPermuteAxesGenericFilter.cxx ${clitkPermuteAxes_GGO_C})
    TARGET_LINK_LIBRARIES(clitkPermuteAxes clitkCommon ITKIO)

    WRAP_GGO(clitkChangeImageOrientation_GGO_C clitkChangeImageOrientation.ggo)
    ADD_EXECUTABLE(clitkChangeImageOrientation clitkChangeImageOrientation.cxx clitkChangeImageOrientationGenericFilter.cxx ${clitkChangeImageOrientation_GGO_C})
    TARGET_LINK_LIBRARIES(clitkChangeImageOrientation clitkCommon ITKIO)

    WRAP_GGO(clitkVFConvert_GGO_C clitkVFConvert.ggo)
    ADD_EXECUTABLE(clitkVFConvert clitkVFConvert.cxx clitkVFConvertGenericFilter.cxx ${clitkVFConvert_GGO_C})
    TARGET_LINK_LIBRARIES(clitkVFConvert ITKBasicFilters ITKIO clitkCommon )

    WRAP_GGO(clitkImageToVectorImage_GGO_C clitkImageToVectorImage.ggo)
    ADD_EXECUTABLE(clitkImageToVectorImage  clitkImageToVectorImage.cxx clitkImageToVectorImageGenericFilter.cxx ${clitkImageToVectorImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkImageToVectorImage  clitkCommon   ITKIO)

    WRAP_GGO(clitkVectorImageToImage_GGO_C clitkVectorImageToImage.ggo)
    ADD_EXECUTABLE(clitkVectorImageToImage  clitkVectorImageToImage.cxx clitkVectorImageToImageGenericFilter.cxx ${clitkVectorImageToImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkVectorImageToImage  clitkCommon   ITKIO)

    WRAP_GGO(clitkBSplineCoefficientsToValues_GGO_C clitkBSplineCoefficientsToValues.ggo)
    ADD_EXECUTABLE(clitkBSplineCoefficientsToValues  clitkBSplineCoefficientsToValues.cxx clitkBSplineCoefficientsToValuesGenericFilter.cxx ${clitkBSplineCoefficientsToValues_GGO_C})
    TARGET_LINK_LIBRARIES(clitkBSplineCoefficientsToValues  clitkCommon   ITKIO)

    WRAP_GGO(clitkValuesToBSplineCoefficients_GGO_C clitkValuesToBSplineCoefficients.ggo)
    ADD_EXECUTABLE(clitkValuesToBSplineCoefficients  clitkValuesToBSplineCoefficients.cxx clitkValuesToBSplineCoefficientsGenericFilter.cxx ${clitkValuesToBSplineCoefficients_GGO_C})
    TARGET_LINK_LIBRARIES(clitkValuesToBSplineCoefficients clitkCommon ITKIO)

    ADD_EXECUTABLE(clitkMIP clitkMIP.cxx clitkMIPGenericFilter.cxx)
    TARGET_LINK_LIBRARIES(clitkMIP clitkMIPLib clitkCommon ITKIO)

    WRAP_GGO(clitkRelativePosition_GGO_C clitkRelativePosition.ggo)
    ADD_EXECUTABLE(clitkRelativePosition  clitkRelativePosition.cxx ${clitkRelativePosition_GGO_C})
    TARGET_LINK_LIBRARIES(clitkRelativePosition clitkCommon ITKIO)

    WRAP_GGO(clitkTransformLandmarks_GGO_C clitkTransformLandmarks.ggo)
    ADD_EXECUTABLE(clitkTransformLandmarks clitkTransformLandmarks.cxx ${clitkTransformLandmarks_GGO_C})
    TARGET_LINK_LIBRARIES(clitkTransformLandmarks clitkCommon ITKIO)

    WRAP_GGO(clitkLineProfile_GGO_C clitkLineProfile.ggo)
    ADD_EXECUTABLE(clitkLineProfile clitkLineProfile.cxx clitkLineProfileGenericFilter.cxx ${clitkLineProfile_GGO_C})
    TARGET_LINK_LIBRARIES(clitkLineProfile clitkCommon ITKIO)

ENDIF(CLITK_BUILD_TOOLS)

