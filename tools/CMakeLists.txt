# Add Libraries used in vv and clitk to avoid recompilation
WRAP_GGO(clitkCropImage_GGO_C clitkCropImage.ggo)
ADD_LIBRARY(clitkCropImageLib clitkCropImageGenericFilter.cxx ${clitkCropImage_GGO_C})

WRAP_GGO(clitkImageConvert_GGO_C clitkImageConvert.ggo)
ADD_LIBRARY(clitkImageConvertLib clitkImageConvertGenericFilter.cxx ${clitkImageConvert_GGO_C})

WRAP_GGO(clitkBinarizeImage_GGO_C clitkBinarizeImage.ggo)
ADD_LIBRARY(clitkBinarizeImageLib clitkBinarizeImageGenericFilter.cxx ${clitkBinarizeImage_GGO_C})

WRAP_GGO(clitkImageArithm_GGO_C clitkImageArithm.ggo)
ADD_LIBRARY(clitkImageArithmImageLib clitkImageArithmGenericFilter.cxx ${clitkImageArithm_GGO_C})

WRAP_GGO(clitkVectorArithm_GGO_C clitkVectorArithm.ggo)
ADD_LIBRARY(clitkVectorArithmLib clitkVectorArithmGenericFilter.cxx ${clitkVectorArithm_GGO_C})

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

  WRAP_GGO(clitkVFInterpolate_GGO_C clitkVFInterpolate.ggo)
  ADD_EXECUTABLE(clitkVFInterpolate clitkVFInterpolate.cxx clitkVFInterpolateGenericFilter.cxx ${clitkVFInterpolate_GGO_C})
  TARGET_LINK_LIBRARIES(clitkVFInterpolate clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVFInterpolate)

  WRAP_GGO(clitkImageCreate_GGO_C clitkImageCreate.ggo)
  ADD_EXECUTABLE(clitkImageCreate clitkImageCreate.cxx ${clitkImageCreate_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageCreate clitkCommon  ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageCreate)

  WRAP_GGO(clitkZeroVF_GGO_C clitkZeroVF.ggo)
  ADD_EXECUTABLE(clitkZeroVF clitkZeroVF.cxx ${clitkZeroVF_GGO_C} clitkZeroVFGenericFilter.cxx)
  TARGET_LINK_LIBRARIES(clitkZeroVF clitkCommon  ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkZeroVF)

  WRAP_GGO(clitkSplitImage_GGO_C clitkSplitImage.ggo)
  ADD_EXECUTABLE(clitkSplitImage clitkSplitImage.cxx clitkSplitImageGenericFilter.cxx ${clitkSplitImage_GGO_C})
  TARGET_LINK_LIBRARIES(clitkSplitImage clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSplitImage)

  WRAP_GGO(clitkWriteDicomSeries_GGO_C clitkWriteDicomSeries.ggo)
  ADD_EXECUTABLE(clitkWriteDicomSeries clitkWriteDicomSeries.cxx ${clitkWriteDicomSeries_GGO_C})
  TARGET_LINK_LIBRARIES(clitkWriteDicomSeries clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkWriteDicomSeries)

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
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkAffineTransform)

  WRAP_GGO(clitkSetBackground_GGO_C clitkSetBackground.ggo)
  ADD_EXECUTABLE(clitkSetBackground clitkSetBackground.cxx clitkSetBackgroundGenericFilter.cxx ${clitkSetBackground_GGO_C})
  TARGET_LINK_LIBRARIES(clitkSetBackground clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSetBackground)

 WRAP_GGO(clitkGammaIndex_GGO_C clitkGammaIndex.ggo)
  ADD_EXECUTABLE(clitkGammaIndex clitkGammaIndex.cxx ${clitkGammaIndex_GGO_C})
  TARGET_LINK_LIBRARIES(clitkGammaIndex clitkCommon ${VTK_LIBRARIES} ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGammaIndex)

  ADD_EXECUTABLE(clitkImageArithm clitkImageArithm.cxx)
  TARGET_LINK_LIBRARIES(clitkImageArithm clitkImageArithmImageLib clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageArithm)

  ADD_EXECUTABLE(clitkVectorArithm clitkVectorArithm.cxx)
  TARGET_LINK_LIBRARIES(clitkVectorArithm clitkVectorArithmLib clitkCommon ${ITK_LIBRARIES} )
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVectorArithm)

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
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageMoment)

  WRAP_GGO(clitkImageStatistics_GGO_C clitkImageStatistics.ggo)
  ADD_EXECUTABLE(clitkImageStatistics clitkImageStatistics.cxx clitkImageStatisticsGenericFilter.cxx ${clitkImageStatistics_GGO_C})
  TARGET_LINK_LIBRARIES(clitkImageStatistics clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageStatistics)

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

  ADD_EXECUTABLE(clitkMIP clitkMIP.cxx clitkMIPGenericFilter.cxx)
  TARGET_LINK_LIBRARIES(clitkMIP clitkMIPLib clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMIP)

  WRAP_GGO(clitkTransformLandmarks_GGO_C clitkTransformLandmarks.ggo)
  ADD_EXECUTABLE(clitkTransformLandmarks clitkTransformLandmarks.cxx ${clitkTransformLandmarks_GGO_C})
  TARGET_LINK_LIBRARIES(clitkTransformLandmarks clitkCommon ${ITK_LIBRARIES})
  SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkTransformLandmarks)

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
    TARGET_LINK_LIBRARIES(clitkBinaryImageToMesh itksys ${ITK_LIBRARIES} ${VTK_LIBRARIES})
    SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBinaryImageToMesh)

    WRAP_GGO(clitkMeshToBinaryImage_GGO_C clitkMeshToBinaryImage.ggo)
    ADD_EXECUTABLE(clitkMeshToBinaryImage clitkMeshToBinaryImage.cxx ${clitkMeshToBinaryImage_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMeshToBinaryImage clitkCommon ${ITK_LIBRARIES} ${VTK_LIBRARIES})
    SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMeshToBinaryImage)

    WRAP_GGO(clitkMeshViewer_GGO_C clitkMeshViewer.ggo)
    ADD_EXECUTABLE(clitkMeshViewer clitkMeshViewer.cxx ${clitkMeshViewer_GGO_C})
    TARGET_LINK_LIBRARIES(clitkMeshViewer ${ITK_LIBRARIES} ${VTK_LIBRARIES})
    SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMeshViewer)
  ENDIF(CLITK_EXPERIMENTAL)

  IF(ITK_VERSION_MAJOR VERSION_LESS 4)
    MESSAGE("clitkDicomRTPlan2Gate is not compatible with GDCM<2 (ITK<4). It will not be built.")
  ELSE(ITK_VERSION_MAJOR VERSION_LESS 4)
    WRAP_GGO(clitkDicomRTPlan2Gate_GGO_C clitkDicomRTPlan2Gate.ggo)
    ADD_EXECUTABLE(clitkDicomRTPlan2Gate clitkDicomRTPlan2Gate.cxx clitkDicomRTPlan2Gate_ggo.c)
    TARGET_LINK_LIBRARIES(clitkDicomRTPlan2Gate clitkCommon ${ITK_LIBRARIES})
    SET(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicomRTPlan2Gate)
  ENDIF(ITK_VERSION_MAJOR VERSION_LESS 4)

  INSTALL (TARGETS ${TOOLS_INSTALL} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

ENDIF(CLITK_BUILD_TOOLS)

