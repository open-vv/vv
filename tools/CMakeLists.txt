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

WRAP_GGO(clitkMedianImageFilter_GGO_C clitkMedianImageFilter.ggo)
ADD_LIBRARY(clitkMedianImageFilterLib clitkMedianImageGenericFilter.cxx ${clitkMedianImageFilter_GGO_C})

IF (CLITK_BUILD_TOOLS)
  WRAP_GGO(clitkDicomInfo_GGO_C clitkDicomInfo.ggo)
  ADD_EXECUTABLE(clitkDicomInfo clitkDicomInfo.cxx ${clitkDicomInfo_GGO_C})
  TARGET_LINK_LIBRARIES(clitkDicomInfo clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL clitkDicomInfo)

  WRAP_GGO(clitkDicom2Image_GGO_C clitkDicom2Image.ggo)
  ADD_EXECUTABLE(clitkDicom2Image clitkDicom2Image.cxx ${clitkDicom2Image_GGO_C})
  TARGET_LINK_LIBRARIES(clitkDicom2Image clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicom2Image)

  WRAP_GGO(clitkDicomWave2Text_GGO_C clitkDicomWave2Text.ggo)
  ADD_EXECUTABLE(clitkDicomWave2Text clitkDicomWave2Text.cxx ${clitkDicomWave2Text_GGO_C})
  TARGET_LINK_LIBRARIES(clitkDicomWave2Text clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicomWave2Text)

  WRAP_GGO(clitkImageInfo_GGO_C clitkImageInfo.ggo)
  ADD_EXECUTABLE(clitkImageInfo clitkImageInfo.cxx ${clitkImageInfo_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageInfo clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageInfo)

  ADD_EXECUTABLE(clitkImageConvert clitkImageConvert.cxx)
  TARGET_LINK_LIBRARIES(clitkImageConvert clitkImageConvertLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageConvert)

  ADD_EXECUTABLE(clitkBinarizeImage clitkBinarizeImage.cxx)
  TARGET_LINK_LIBRARIES(clitkBinarizeImage clitkBinarizeImageLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBinarizeImage)

  WRAP_GGO(clitkVFResample_GGO_C clitkVFResample.ggo)
  ADD_EXECUTABLE(clitkVFResample clitkVFResample.cxx clitkVFResampleGenericFilter.cxx ${clitkVFResample_GGO_C})
  TARGET_LINK_LIBRARIES(clitkVFResample clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVFResample)

  WRAP_GGO(clitkImageCreate_GGO_C clitkImageCreate.ggo)
  ADD_EXECUTABLE(clitkImageCreate clitkImageCreate.cxx ${clitkImageCreate_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageCreate clitkCommon  ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageCreate)

  WRAP_GGO(clitkZeroVF_GGO_C clitkZeroVF.ggo)
  ADD_EXECUTABLE(clitkZeroVF clitkZeroVF.cxx ${clitkZeroVF_GGO_C} clitkZeroVFGenericFilter.cxx)
  TARGET_LINK_LIBRARIES(clitkZeroVF clitkCommon  ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkZeroVF)

  WRAP_GGO(clitkImageExtractLine_GGO_C clitkImageExtractLine.ggo)
  ADD_EXECUTABLE(clitkImageExtractLine clitkImageExtractLine.cxx ${clitkImageExtractLine_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageExtractLine clitkCommon  ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageExtractLine)

  WRAP_GGO(clitkSplitImage_GGO_C clitkSplitImage.ggo)
  ADD_EXECUTABLE(clitkSplitImage clitkSplitImage.cxx clitkSplitImageGenericFilter.cxx ${clitkSplitImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkSplitImage clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSplitImage)

  WRAP_GGO(clitkVFMerge_GGO_C clitkVFMerge.ggo)
  ADD_EXECUTABLE(clitkVFMerge clitkVFMerge.cxx ${clitkVFMerge_GGO_C})
  TARGET_LINK_LIBRARIES(clitkVFMerge clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVFMerge)

  WRAP_GGO(clitkWriteDicomSeries_GGO_C clitkWriteDicomSeries.ggo)
  ADD_EXECUTABLE(clitkWriteDicomSeries clitkWriteDicomSeries.cxx ${clitkWriteDicomSeries_GGO_C})
  TARGET_LINK_LIBRARIES(clitkWriteDicomSeries clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkWriteDicomSeries)

  WRAP_GGO(clitkAverageTemporalDimension_GGO_C clitkAverageTemporalDimension.ggo)
  ADD_EXECUTABLE(clitkAverageTemporalDimension clitkAverageTemporalDimension.cxx ${clitkAverageTemporalDimension_GGO_C})
  TARGET_LINK_LIBRARIES(clitkAverageTemporalDimension clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkAverageTemporalDimension)

  WRAP_GGO(clitkMedianTemporalDimension_GGO_C clitkMedianTemporalDimension.ggo)
  ADD_EXECUTABLE(clitkMedianTemporalDimension clitkMedianTemporalDimension.cxx
      ${clitkMedianTemporalDimension_GGO_C})
  TARGET_LINK_LIBRARIES(clitkMedianTemporalDimension clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMedianTemporalDimension)

  WRAP_GGO(clitkWarpImage_GGO_C clitkWarpImage.ggo)
  ADD_EXECUTABLE(clitkWarpImage clitkWarpImage.cxx ${clitkWarpImage_GGO_C} clitkWarpImageGenericFilter.cxx)
  TARGET_LINK_LIBRARIES(clitkWarpImage clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkWarpImage)

  WRAP_GGO(clitkInvertVF_GGO_C clitkInvertVF.ggo)
  ADD_EXECUTABLE(clitkInvertVF clitkInvertVF.cxx ${clitkInvertVF_GGO_C})
  TARGET_LINK_LIBRARIES(clitkInvertVF clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkInvertVF)

  WRAP_GGO(clitkAffineTransform_GGO_C clitkAffineTransform.ggo)
  ADD_EXECUTABLE(clitkAffineTransform clitkAffineTransform.cxx ${clitkAffineTransform_GGO_C})
  TARGET_LINK_LIBRARIES(clitkAffineTransform clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicom2Image)

  WRAP_GGO(clitkSetBackground_GGO_C clitkSetBackground.ggo)
  ADD_EXECUTABLE(clitkSetBackground clitkSetBackground.cxx clitkSetBackgroundGenericFilter.cxx ${clitkSetBackground_GGO_C})
  TARGET_LINK_LIBRARIES(clitkSetBackground clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSetBackground)

  WRAP_GGO(clitkGuerreroVentilation_GGO_C clitkGuerreroVentilation.ggo)
  ADD_EXECUTABLE(clitkGuerreroVentilation clitkGuerreroVentilation.cxx clitkGuerreroVentilationGenericFilter.cxx ${clitkGuerreroVentilation_GGO_C})
  TARGET_LINK_LIBRARIES(clitkGuerreroVentilation clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGuerreroVentilation)

  WRAP_GGO(clitkGammaIndex_GGO_C clitkGammaIndex.ggo)
  ADD_EXECUTABLE(clitkGammaIndex clitkGammaIndex.cxx ${clitkGammaIndex_GGO_C})
  TARGET_LINK_LIBRARIES(clitkGammaIndex clitkCommon ${VTK_LIBRARIES} ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGammaIndex)

  ADD_EXECUTABLE(clitkImageArithm clitkImageArithm.cxx)
  TARGET_LINK_LIBRARIES(clitkImageArithm clitkImageArithmImageLib clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageArithm)

  WRAP_GGO(clitkUnsharpMask_GGO_C clitkUnsharpMask.ggo)
  ADD_EXECUTABLE(clitkUnsharpMask clitkUnsharpMask.cxx ${clitkUnsharpMask_GGO_C})
  TARGET_LINK_LIBRARIES(clitkUnsharpMask clitkCommon ${ITK_LIBRARIES} ) 
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkUnsharpMask)

  WRAP_GGO(clitkFooImage_GGO_C clitkFooImage.ggo)
  ADD_EXECUTABLE(clitkFooImage clitkFooImage.cxx ${clitkFooImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkFooImage clitkCommon ${ITK_LIBRARIES} ) 
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkFooImage)

  #WRAP_GGO(clitkMedianImageFilter_GGO_C clitkMedianImageFilter.ggo)
  #ADD_EXECUTABLE(clitkMedianImageFilter clitkMedianImageFilter.cxx ${clitkMedianImageFilter_GGO_C})
  #TARGET_LINK_LIBRARIES(clitkMedianImageFilter clitkMedianImageFilterLib clitkCommon ${ITK_LIBRARIES})
  #SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMedianImageFilter)

  ADD_EXECUTABLE(clitkResampleImage clitkResampleImage.cxx)
  TARGET_LINK_LIBRARIES(clitkResampleImage clitkResampleImageLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkResampleImage)

  WRAP_GGO(clitkMinMaxMask_GGO_C clitkMinMaxMask.ggo)
  ADD_EXECUTABLE(clitkMinMaxMask clitkMinMaxMask.cxx ${clitkMinMaxMask_GGO_C})
  TARGET_LINK_LIBRARIES(clitkMinMaxMask clitkCommon ${ITK_LIBRARIES}  )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMinMaxMask)

  WRAP_GGO(clitkAutoCrop_GGO_C clitkAutoCrop.ggo)
  ADD_EXECUTABLE(clitkAutoCrop clitkAutoCrop.cxx ${clitkAutoCrop_GGO_C})
  TARGET_LINK_LIBRARIES(clitkAutoCrop clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkAutoCrop)

  WRAP_GGO(clitkDicomRTStruct2Image_GGO_C clitkDicomRTStruct2Image.ggo)
  ADD_EXECUTABLE(clitkDicomRTStruct2Image clitkDicomRTStruct2Image.cxx ${clitkDicomRTStruct2Image_GGO_C})
  TARGET_LINK_LIBRARIES(clitkDicomRTStruct2Image clitkDicomRTStruct clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicomRTStruct2Image)

  # WRAP_GGO(clitkImage2DicomRTStruct_GGO_C clitkImage2DicomRTStruct.ggo)
  # ADD_EXECUTABLE(clitkImage2DicomRTStruct clitkImage2DicomRTStruct.cxx ${clitkImage2DicomRTStruct_GGO_C})
  # TARGET_LINK_LIBRARIES(clitkImage2DicomRTStruct clitkDicomRTStruct clitkCommon ${ITK_LIBRARIES} )
  # SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImage2DicomRTStruct)

  WRAP_GGO(clitkImageLog_GGO_C clitkImageLog.ggo)
  ADD_EXECUTABLE(clitkImageLog clitkImageLog.cxx ${clitkImageLog_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageLog  ${ITK_LIBRARIES} clitkCommon)
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageLog)

  WRAP_GGO(clitkFilter_GGO_C clitkFilter.ggo)
  ADD_EXECUTABLE(clitkFilter clitkFilter.cxx clitkFilterGenericFilter.cxx ${clitkFilter_GGO_C})
  TARGET_LINK_LIBRARIES(clitkFilter ${ITK_LIBRARIES} clitkCommon)
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkFilter)

  WRAP_GGO(clitkConeBeamProjectImage_GGO_C clitkConeBeamProjectImage.ggo)
  ADD_EXECUTABLE(clitkConeBeamProjectImage clitkConeBeamProjectImage.cxx clitkConeBeamProjectImageGenericFilter.cxx ${clitkConeBeamProjectImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkConeBeamProjectImage  ${ITK_LIBRARIES} clitkCommon )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkConeBeamProjectImage)

  WRAP_GGO(clitkComposeVF_GGO_C clitkComposeVF.ggo)
  ADD_EXECUTABLE(clitkComposeVF clitkComposeVFGenericFilter.cxx clitkComposeVF.cxx ${clitkComposeVF_GGO_C})
  TARGET_LINK_LIBRARIES(clitkComposeVF ${ITK_LIBRARIES} clitkCommon)
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkComposeVF)

  WRAP_GGO(clitkMergeSequence_GGO_C clitkMergeSequence.ggo)
  ADD_EXECUTABLE(clitkMergeSequence clitkMergeSequence.cxx clitkMergeSequenceGenericFilter.cxx ${clitkMergeSequence_GGO_C})
  TARGET_LINK_LIBRARIES(clitkMergeSequence ${ITK_LIBRARIES} clitkCommon)
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMergeSequence)

  WRAP_GGO(clitkBackProjectImage_GGO_C clitkBackProjectImage.ggo)
  ADD_EXECUTABLE(clitkBackProjectImage clitkBackProjectImage.cxx clitkBackProjectImageGenericFilter.cxx ${clitkBackProjectImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkBackProjectImage ${ITK_LIBRARIES} clitkCommon )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBackProjectImage)

  ADD_EXECUTABLE(clitkCropImage clitkCropImage.cxx ${clitkCropImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkCropImage clitkCropImageLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkCropImage)

  # WRAP_GGO(clitkExtractSlice_GGO_C clitkExtractSlice.ggo)
  # ADD_EXECUTABLE(clitkExtractSlice clitkExtractSlice.cxx clitkExtractSliceGenericFilter.cxx ${clitkExtractSlice_GGO_C})
  # TARGET_LINK_LIBRARIES(clitkExtractSlice clitkCommon ${ITK_LIBRARIES})
  #SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkExtractSlice)
  
  WRAP_GGO(clitkFlipImage_GGO_C clitkFlipImage.ggo)
  ADD_EXECUTABLE(clitkFlipImage clitkFlipImage.cxx clitkFlipImageGenericFilter.cxx ${clitkFlipImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkFlipImage clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkFlipImage)

  WRAP_GGO(clitkMirrorPadImage_GGO_C clitkMirrorPadImage.ggo)
  ADD_EXECUTABLE(clitkMirrorPadImage clitkMirrorPadImage.cxx clitkMirrorPadImageGenericFilter.cxx ${clitkMirrorPadImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkMirrorPadImage clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMirrorPadImage)

  WRAP_GGO(clitkImageMoment_GGO_C clitkImageMoment.ggo)
  ADD_EXECUTABLE(clitkImageMoment clitkImageMoment.cxx clitkImageMomentGenericFilter.cxx ${clitkImageMoment_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageMoment clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicom2Image)

  WRAP_GGO(clitkImageStatistics_GGO_C clitkImageStatistics.ggo)
  ADD_EXECUTABLE(clitkImageStatistics clitkImageStatistics.cxx clitkImageStatisticsGenericFilter.cxx ${clitkImageStatistics_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageStatistics clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageStatistics)

  WRAP_GGO(clitkSetOrigin_GGO_C clitkSetOrigin.ggo)
  ADD_EXECUTABLE(clitkSetOrigin clitkSetOrigin.cxx clitkSetOriginGenericFilter.cxx ${clitkSetOrigin_GGO_C})
  TARGET_LINK_LIBRARIES(clitkSetOrigin clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSetOrigin)
    
  WRAP_GGO(clitkGetOrigin_GGO_C clitkGetOrigin.ggo)
  ADD_EXECUTABLE(clitkGetOrigin clitkGetOrigin.cxx clitkGetOriginGenericFilter.cxx ${clitkGetOrigin_GGO_C})
  TARGET_LINK_LIBRARIES(clitkGetOrigin clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGetOrigin)

  WRAP_GGO(clitkGetDirection_GGO_C clitkGetDirection.ggo)
  ADD_EXECUTABLE(clitkGetDirection clitkGetDirection.cxx clitkGetDirectionGenericFilter.cxx ${clitkGetDirection_GGO_C})
  TARGET_LINK_LIBRARIES(clitkGetDirection clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGetDirection)

  WRAP_GGO(clitkSetDirection_GGO_C clitkSetDirection.ggo)
  ADD_EXECUTABLE(clitkSetDirection clitkSetDirection.cxx clitkSetDirectionGenericFilter.cxx ${clitkSetDirection_GGO_C})
  TARGET_LINK_LIBRARIES(clitkSetDirection clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSetDirection)

  WRAP_GGO(clitkGetSize_GGO_C clitkGetSize.ggo)
  ADD_EXECUTABLE(clitkGetSize  clitkGetSize.cxx clitkGetSizeGenericFilter.cxx ${clitkGetSize_GGO_C})
  TARGET_LINK_LIBRARIES(clitkGetSize  clitkCommon   ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGetSize)

  WRAP_GGO(clitkGetSpacing_GGO_C clitkGetSpacing.ggo)
  ADD_EXECUTABLE(clitkGetSpacing  clitkGetSpacing.cxx clitkGetSpacingGenericFilter.cxx ${clitkGetSpacing_GGO_C})
  TARGET_LINK_LIBRARIES(clitkGetSpacing  clitkCommon   ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGetSpacing)

  WRAP_GGO(clitkSetSpacing_GGO_C clitkSetSpacing.ggo)
  ADD_EXECUTABLE(clitkSetSpacing  clitkSetSpacing.cxx clitkSetSpacingGenericFilter.cxx ${clitkSetSpacing_GGO_C})
  TARGET_LINK_LIBRARIES(clitkSetSpacing  clitkCommon   ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicom2Image)

  WRAP_GGO(clitkCombineImage_GGO_C clitkCombineImage.ggo)
  ADD_EXECUTABLE(clitkCombineImage clitkCombineImage.cxx clitkCombineImageGenericFilter.cxx ${clitkCombineImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkCombineImage clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkCombineImage)

  WRAP_GGO(clitkPermuteAxes_GGO_C clitkPermuteAxes.ggo)
  ADD_EXECUTABLE(clitkPermuteAxes clitkPermuteAxes.cxx clitkPermuteAxesGenericFilter.cxx ${clitkPermuteAxes_GGO_C})
  TARGET_LINK_LIBRARIES(clitkPermuteAxes clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkPermuteAxes)

  WRAP_GGO(clitkChangeImageOrientation_GGO_C clitkChangeImageOrientation.ggo)
  ADD_EXECUTABLE(clitkChangeImageOrientation clitkChangeImageOrientation.cxx clitkChangeImageOrientationGenericFilter.cxx ${clitkChangeImageOrientation_GGO_C})
  TARGET_LINK_LIBRARIES(clitkChangeImageOrientation clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkChangeImageOrientation)

  WRAP_GGO(clitkVFConvert_GGO_C clitkVFConvert.ggo)
  ADD_EXECUTABLE(clitkVFConvert clitkVFConvert.cxx clitkVFConvertGenericFilter.cxx ${clitkVFConvert_GGO_C})
  TARGET_LINK_LIBRARIES(clitkVFConvert ${ITK_LIBRARIES} clitkCommon )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVFConvert)

  WRAP_GGO(clitkImageToVectorImage_GGO_C clitkImageToVectorImage.ggo)
  ADD_EXECUTABLE(clitkImageToVectorImage  clitkImageToVectorImage.cxx clitkImageToVectorImageGenericFilter.cxx ${clitkImageToVectorImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageToVectorImage  clitkCommon   ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageToVectorImage)

  WRAP_GGO(clitkVectorImageToImage_GGO_C clitkVectorImageToImage.ggo)
  ADD_EXECUTABLE(clitkVectorImageToImage  clitkVectorImageToImage.cxx clitkVectorImageToImageGenericFilter.cxx ${clitkVectorImageToImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkVectorImageToImage  clitkCommon   ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVectorImageToImage)

  WRAP_GGO(clitkBSplineCoefficientsToValues_GGO_C clitkBSplineCoefficientsToValues.ggo)
  ADD_EXECUTABLE(clitkBSplineCoefficientsToValues  clitkBSplineCoefficientsToValues.cxx clitkBSplineCoefficientsToValuesGenericFilter.cxx ${clitkBSplineCoefficientsToValues_GGO_C})
  TARGET_LINK_LIBRARIES(clitkBSplineCoefficientsToValues clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBSplineCoefficientsToValues)

  WRAP_GGO(clitkValuesToBSplineCoefficients_GGO_C clitkValuesToBSplineCoefficients.ggo)
  ADD_EXECUTABLE(clitkValuesToBSplineCoefficients  clitkValuesToBSplineCoefficients.cxx clitkValuesToBSplineCoefficientsGenericFilter.cxx ${clitkValuesToBSplineCoefficients_GGO_C})
  TARGET_LINK_LIBRARIES(clitkValuesToBSplineCoefficients clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkValuesToBSplineCoefficients)

  ADD_EXECUTABLE(clitkMIP clitkMIP.cxx clitkMIPGenericFilter.cxx)
  TARGET_LINK_LIBRARIES(clitkMIP clitkMIPLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMIP)

  WRAP_GGO(clitkRelativePosition_GGO_C clitkRelativePosition.ggo)
  ADD_EXECUTABLE(clitkRelativePosition  clitkRelativePosition.cxx ${clitkRelativePosition_GGO_C})
  TARGET_LINK_LIBRARIES(clitkRelativePosition clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkRelativePosition)

  WRAP_GGO(clitkLabelImageOverlapMeasure_GGO_C clitkLabelImageOverlapMeasure.ggo)
  ADD_EXECUTABLE(clitkLabelImageOverlapMeasure clitkLabelImageOverlapMeasure.cxx ${clitkLabelImageOverlapMeasure_GGO_C})
  TARGET_LINK_LIBRARIES(clitkLabelImageOverlapMeasure clitkSegmentationGgoLib clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkLabelImageOverlapMeasure)

  WRAP_GGO(clitkRelativePositionAnalyzer_GGO_C clitkRelativePositionAnalyzer.ggo)
  ADD_EXECUTABLE(clitkRelativePositionAnalyzer ../itk/clitkRelativePositionDataBase.cxx clitkRelativePositionAnalyzer.cxx ${clitkRelativePositionAnalyzer_GGO_C})
  TARGET_LINK_LIBRARIES(clitkRelativePositionAnalyzer clitkSegmentationGgoLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkRelativePositionAnalyzer)

  WRAP_GGO(clitkRelativePositionDataBaseAnalyzer_GGO_C clitkRelativePositionDataBaseAnalyzer.ggo)
  ADD_EXECUTABLE(clitkRelativePositionDataBaseAnalyzer ../itk/clitkRelativePositionDataBase.cxx clitkRelativePositionDataBaseAnalyzer.cxx ${clitkRelativePositionDataBaseAnalyzer_GGO_C})
  TARGET_LINK_LIBRARIES(clitkRelativePositionDataBaseAnalyzer clitkSegmentationGgoLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkRelativePositionDataBaseAnalyzer)

  WRAP_GGO(clitkRelativePositionDataBaseBuilder_GGO_C clitkRelativePositionDataBaseBuilder.ggo)
  ADD_EXECUTABLE(clitkRelativePositionDataBaseBuilder ../itk/clitkRelativePositionDataBase.cxx clitkRelativePositionDataBaseBuilder.cxx ${clitkRelativePositionDataBaseBuilder_GGO_C})
  TARGET_LINK_LIBRARIES(clitkRelativePositionDataBaseBuilder clitkSegmentationGgoLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkRelativePositionDataBaseBuilder)

  WRAP_GGO(clitkTransformLandmarks_GGO_C clitkTransformLandmarks.ggo)
  ADD_EXECUTABLE(clitkTransformLandmarks clitkTransformLandmarks.cxx ${clitkTransformLandmarks_GGO_C})
  TARGET_LINK_LIBRARIES(clitkTransformLandmarks clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkTransformLandmarks)

  WRAP_GGO(clitkLineProfile_GGO_C clitkLineProfile.ggo)
  ADD_EXECUTABLE(clitkLineProfile clitkLineProfile.cxx clitkLineProfileGenericFilter.cxx ${clitkLineProfile_GGO_C})
  TARGET_LINK_LIBRARIES(clitkLineProfile clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkLineProfile)

  ADD_EXECUTABLE(clitkMakeSphereImage clitkMakeSphereImage.cxx) # clitkLineProfileGenericFilter.cxx ${clitkLineProfile_GGO_C})
  TARGET_LINK_LIBRARIES(clitkMakeSphereImage clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMakeSphereImage)

  WRAP_GGO(clitkJacobianImage_GGO_C clitkJacobianImage.ggo)
  ADD_EXECUTABLE(clitkJacobianImage clitkJacobianImage.cxx clitkJacobianImageGenericFilter.cxx ${clitkJacobianImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkJacobianImage clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkJacobianImage)
  
  WRAP_GGO(clitkPadImage_GGO_C clitkPadImage.ggo)
  ADD_EXECUTABLE(clitkPadImage clitkPadImage.cxx clitkPadImageGenericFilter.cxx ${clitkPadImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkPadImage clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkPadImage)

  WRAP_GGO(clitkCatImage_GGO_C clitkCatImage.ggo)
  ADD_EXECUTABLE(clitkCatImage clitkCatImage.cxx ${clitkCatImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkCatImage clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkCatImage)

  IF(CLITK_EXPERIMENTAL)
    WRAP_GGO(clitkBinaryImageToMesh_GGO_C clitkBinaryImageToMesh.ggo)
    ADD_EXECUTABLE(clitkBinaryImageToMesh clitkBinaryImageToMesh.cxx ${clitkBinaryImageToMesh_GGO_C})
    TARGET_LINK_LIBRARIES(clitkBinaryImageToMesh ${ITK_LIBRARIES} ${VTK_LIBRARIES})
    SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBinaryImageToMesh)

    WRAP_GGO(clitkMeshToBinaryImage_GGO_C clitkMeshToBinaryImage.ggo)
    ADD_EXECUTABLE(clitkMeshToBinaryImage clitkMeshToBinaryImage.cxx ${clitkMeshToBinaryImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMeshToBinaryImage clitkCommon ${ITK_LIBRARIES} ${VTK_LIBRARIES})
    SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMeshToBinaryImage)

    WRAP_GGO(clitkMeshViewer_GGO_C clitkMeshViewer.ggo)
    ADD_EXECUTABLE(clitkMeshViewer clitkMeshViewer.cxx ${clitkMeshViewer_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMeshViewer ${VTK_LIBRARIES})
    SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMeshViewer)
  ENDIF(CLITK_EXPERIMENTAL)

  SET_TARGET_PROPERTIES(${TOOLS_INSTALL} PROPERTIES INSTALL_RPATH "${VTK_DIR}:${ITK_DIR}" )  
  INSTALL (TARGETS ${TOOLS_INSTALL} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

ENDIF(CLITK_BUILD_TOOLS)

