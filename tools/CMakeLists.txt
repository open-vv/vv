#=========================================================
# Add Libraries used in vv and clitk to avoid recompilation
WRAP_GGO(clitkCropImage_GGO_C clitkCropImage.ggo)
add_library(clitkCropImageLib clitkCropImageGenericFilter.cxx ${clitkCropImage_GGO_C})
target_link_libraries(clitkCropImageLib clitkCommon)

WRAP_GGO(clitkImageConvert_GGO_C clitkImageConvert.ggo)
add_library(clitkImageConvertLib clitkImageConvertGenericFilter.cxx ${clitkImageConvert_GGO_C})
target_link_libraries(clitkImageConvertLib clitkCommon)

WRAP_GGO(clitkBinarizeImage_GGO_C clitkBinarizeImage.ggo)
add_library(clitkBinarizeImageLib clitkBinarizeImageGenericFilter.cxx ${clitkBinarizeImage_GGO_C})
target_link_libraries(clitkBinarizeImageLib clitkCommon)

WRAP_GGO(clitkProfileImage_GGO_C clitkProfileImage.ggo)
add_library(clitkProfileImageLib clitkProfileImageGenericFilter.cxx ${clitkProfileImage_GGO_C})
target_link_libraries(clitkProfileImageLib clitkCommon)

WRAP_GGO(clitkHistogramImage_GGO_C clitkHistogramImage.ggo)
add_library(clitkHistogramImageLib clitkHistogramImageGenericFilter.cxx ${clitkHistogramImage_GGO_C})
target_link_libraries(clitkHistogramImageLib clitkCommon)

WRAP_GGO(clitkImageArithm_GGO_C clitkImageArithm.ggo)
add_library(clitkImageArithmImageLib clitkImageArithmGenericFilter.cxx ${clitkImageArithm_GGO_C})
target_link_libraries(clitkImageArithmImageLib clitkCommon)

WRAP_GGO(clitkVectorArithm_GGO_C clitkVectorArithm.ggo)
add_library(clitkVectorArithmLib clitkVectorArithmGenericFilter.cxx ${clitkVectorArithm_GGO_C})
target_link_libraries(clitkVectorArithmLib clitkCommon)

WRAP_GGO(clitkMIP_GGO_C clitkMIP.ggo)
add_library(clitkMIPLib clitkMIPGenericFilter.cxx ${clitkMIP_GGO_C})
target_link_libraries(clitkMIPLib clitkCommon)

WRAP_GGO(clitkMedianImageFilter_GGO_C clitkMedianImageFilter.ggo)
add_library(clitkMedianImageFilterLib clitkMedianImageGenericFilter.cxx ${clitkMedianImageFilter_GGO_C})
target_link_libraries(clitkMedianImageFilterLib clitkCommon)

#=========================================================
if(CLITK_BUILD_TOOLS)

  WRAP_GGO(clitkDicomInfo_GGO_C clitkDicomInfo.ggo)
  add_executable(clitkDicomInfo clitkDicomInfo.cxx ${clitkDicomInfo_GGO_C})
  target_link_libraries(clitkDicomInfo clitkCommon)
  # set_target_properties(clitkDicomInfo
  #   PROPERTIES
  #   LINK_INTERFACE_LIBRARIES ""
  #   )
  set(TOOLS_INSTALL clitkDicomInfo)

  WRAP_GGO(clitkDicom2Image_GGO_C clitkDicom2Image.ggo)
  add_executable(clitkDicom2Image clitkDicom2Image.cxx ${clitkDicom2Image_GGO_C})
  target_link_libraries(clitkDicom2Image clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicom2Image)

  WRAP_GGO(clitkDicomWave2Text_GGO_C clitkDicomWave2Text.ggo)
  add_executable(clitkDicomWave2Text clitkDicomWave2Text.cxx ${clitkDicomWave2Text_GGO_C})
  target_link_libraries(clitkDicomWave2Text clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicomWave2Text)

  WRAP_GGO(clitkImageInfo_GGO_C clitkImageInfo.ggo)
  add_executable(clitkImageInfo clitkImageInfo.cxx ${clitkImageInfo_GGO_C})
  target_link_libraries(clitkImageInfo clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageInfo)

  WRAP_GGO(clitkScintivolStats_GGO_C clitkScintivolStats.ggo)
  add_executable(clitkScintivolStats clitkScintivolStats.cxx ${clitkScintivolStats_GGO_C})
  target_link_libraries(clitkScintivolStats clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkScintivolStats)

  add_executable(clitkImageConvert clitkImageConvert.cxx)
  target_link_libraries(clitkImageConvert clitkImageConvertLib clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageConvert)

  add_executable(clitkBinarizeImage clitkBinarizeImage.cxx)
  target_link_libraries(clitkBinarizeImage clitkBinarizeImageLib clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBinarizeImage)

  add_executable(clitkProfileImage clitkProfileImage.cxx)
  target_link_libraries(clitkProfileImage clitkProfileImageLib clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkProfileImage)

  add_executable(clitkHistogramImage clitkHistogramImage.cxx)
  target_link_libraries(clitkHistogramImage clitkHistogramImageLib clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkHistogramImage)

  WRAP_GGO(clitkVFResample_GGO_C clitkVFResample.ggo)
  add_executable(clitkVFResample clitkVFResample.cxx clitkVFResampleGenericFilter.cxx ${clitkVFResample_GGO_C})
  target_link_libraries(clitkVFResample clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVFResample)

  WRAP_GGO(clitkVFInterpolate_GGO_C clitkVFInterpolate.ggo)
  add_executable(clitkVFInterpolate clitkVFInterpolate.cxx clitkVFInterpolateGenericFilter.cxx ${clitkVFInterpolate_GGO_C})
  target_link_libraries(clitkVFInterpolate clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVFInterpolate)

  WRAP_GGO(clitkImageCreate_GGO_C clitkImageCreate.ggo)
  add_executable(clitkImageCreate clitkImageCreate.cxx ${clitkImageCreate_GGO_C})
  target_link_libraries(clitkImageCreate clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageCreate)

  WRAP_GGO(clitkZeroVF_GGO_C clitkZeroVF.ggo)
  add_executable(clitkZeroVF clitkZeroVF.cxx ${clitkZeroVF_GGO_C} clitkZeroVFGenericFilter.cxx)
  target_link_libraries(clitkZeroVF clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkZeroVF)

  WRAP_GGO(clitkSplitImage_GGO_C clitkSplitImage.ggo)
  add_executable(clitkSplitImage clitkSplitImage.cxx clitkSplitImageGenericFilter.cxx ${clitkSplitImage_GGO_C})
  target_link_libraries(clitkSplitImage clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSplitImage)

  WRAP_GGO(clitkWriteDicomSeries_GGO_C clitkWriteDicomSeries.ggo)
  add_executable(clitkWriteDicomSeries clitkWriteDicomSeries.cxx ${clitkWriteDicomSeries_GGO_C})
  target_link_libraries(clitkWriteDicomSeries clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkWriteDicomSeries)

  WRAP_GGO(clitkImage2Dicom_GGO_C clitkImage2Dicom.ggo)
  add_executable(clitkImage2Dicom clitkImage2Dicom.cxx ${clitkImage2Dicom_GGO_C})
  target_link_libraries(clitkImage2Dicom clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImage2Dicom)

  WRAP_GGO(clitkSpect2Dicom_GGO_C clitkSpect2Dicom.ggo)
  add_executable(clitkSpect2Dicom clitkSpect2Dicom.cxx ${clitkSpect2Dicom_GGO_C})
  target_link_libraries(clitkSpect2Dicom clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSpect2Dicom)

  WRAP_GGO(clitkChangeDicomTag_GGO_C clitkChangeDicomTag.ggo)
  add_executable(clitkChangeDicomTag clitkChangeDicomTag.cxx ${clitkChangeDicomTag_GGO_C})
  target_link_libraries(clitkChangeDicomTag clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkChangeDicomTag)

  WRAP_GGO(clitkGateSimulation2Dicom_GGO_C clitkGateSimulation2Dicom.ggo)
  add_executable(clitkGateSimulation2Dicom clitkGateSimulation2Dicom.cxx ${clitkGateSimulation2Dicom_GGO_C})
  target_link_libraries(clitkGateSimulation2Dicom clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGateSimulation2Dicom)

  WRAP_GGO(clitkPartitionEnergyWindowDicom_GGO_C clitkPartitionEnergyWindowDicom.ggo)
  add_executable(clitkPartitionEnergyWindowDicom clitkPartitionEnergyWindowDicom.cxx ${clitkPartitionEnergyWindowDicom_GGO_C})
  target_link_libraries(clitkPartitionEnergyWindowDicom clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkPartitionEnergyWindowDicom)

  WRAP_GGO(clitkUpdateVRTagDicom_GGO_C clitkUpdateVRTagDicom.ggo)
  add_executable(clitkUpdateVRTagDicom clitkUpdateVRTagDicom.cxx ${clitkUpdateVRTagDicom_GGO_C})
  target_link_libraries(clitkUpdateVRTagDicom clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkUpdateVRTagDicom)

  WRAP_GGO(clitkImage2DicomDose_GGO_C clitkImage2DicomDose.ggo)
  add_executable(clitkImage2DicomDose clitkImage2DicomDose.cxx ${clitkImage2DicomDose_GGO_C})
  target_link_libraries(clitkImage2DicomDose clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImage2DicomDose)

  WRAP_GGO(clitkMedianTemporalDimension_GGO_C clitkMedianTemporalDimension.ggo)
  add_executable(clitkMedianTemporalDimension clitkMedianTemporalDimension.cxx
    ${clitkMedianTemporalDimension_GGO_C})
  target_link_libraries(clitkMedianTemporalDimension clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMedianTemporalDimension)

  WRAP_GGO(clitkWarpImage_GGO_C clitkWarpImage.ggo)
  add_executable(clitkWarpImage clitkWarpImage.cxx ${clitkWarpImage_GGO_C} clitkWarpImageGenericFilter.cxx)
  target_link_libraries(clitkWarpImage clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkWarpImage)

  WRAP_GGO(clitkInvertVF_GGO_C clitkInvertVF.ggo)
  add_executable(clitkInvertVF clitkInvertVF.cxx ${clitkInvertVF_GGO_C})
  target_link_libraries(clitkInvertVF clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkInvertVF)

  WRAP_GGO(clitkAffineTransform_GGO_C clitkAffineTransform.ggo)
  add_library(clitkAffineTransformLib clitkAffineTransform.cxx ${clitkAffineTransform_GGO_C})
  target_link_libraries(clitkAffineTransformLib clitkCommon)
  add_executable(clitkAffineTransform clitkAffineTransform.cxx ${clitkAffineTransform_GGO_C})
  target_link_libraries(clitkAffineTransform clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkAffineTransform)

  WRAP_GGO(clitkElastixTransformToMatrix_GGO_C clitkElastixTransformToMatrix.ggo)
  add_executable(clitkElastixTransformToMatrix clitkElastixTransformToMatrix.cxx ${clitkElastixTransformToMatrix_GGO_C})
  target_link_libraries(clitkElastixTransformToMatrix clitkAffineTransformLib clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkElastixTransformToMatrix)

  WRAP_GGO(clitkMatrixToElastixTransform_GGO_C clitkMatrixToElastixTransform.ggo)
  add_executable(clitkMatrixToElastixTransform clitkMatrixToElastixTransform.cxx ${clitkMatrixToElastixTransform_GGO_C})
  target_link_libraries(clitkMatrixToElastixTransform clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMatrixToElastixTransform)

  WRAP_GGO(clitkMatrixMultiply_GGO_C clitkMatrixMultiply.ggo)
  add_executable(clitkMatrixMultiply clitkMatrixMultiply.cxx ${clitkMatrixMultiply_GGO_C})
  target_link_libraries(clitkMatrixMultiply clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMatrixMultiply)

  WRAP_GGO(clitkMatrixInverse_GGO_C clitkMatrixInverse.ggo)
  add_executable(clitkMatrixInverse clitkMatrixInverse.cxx ${clitkMatrixInverse_GGO_C})
  target_link_libraries(clitkMatrixInverse clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMatrixInverse)

  WRAP_GGO(clitkSetBackground_GGO_C clitkSetBackground.ggo)
  add_executable(clitkSetBackground clitkSetBackground.cxx clitkSetBackgroundGenericFilter.cxx ${clitkSetBackground_GGO_C})
  target_link_libraries(clitkSetBackground clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSetBackground)

  WRAP_GGO(clitkGammaIndex_GGO_C clitkGammaIndex.ggo)
  add_executable(clitkGammaIndex clitkGammaIndex.cxx ${clitkGammaIndex_GGO_C})
  target_link_libraries(clitkGammaIndex clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkGammaIndex)

  add_executable(clitkImageArithm clitkImageArithm.cxx)
  target_link_libraries(clitkImageArithm clitkImageArithmImageLib clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageArithm)

  add_executable(clitkVectorArithm clitkVectorArithm.cxx)
  target_link_libraries(clitkVectorArithm clitkVectorArithmLib clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVectorArithm)

  WRAP_GGO(clitkUnsharpMask_GGO_C clitkUnsharpMask.ggo)
  add_executable(clitkUnsharpMask clitkUnsharpMask.cxx ${clitkUnsharpMask_GGO_C})
  target_link_libraries(clitkUnsharpMask clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkUnsharpMask)

  WRAP_GGO(clitkMedianImageFilter_GGO_C clitkMedianImageFilter.ggo)
  add_executable(clitkMedianImageFilter clitkMedianImageFilter.cxx ${clitkMedianImageFilter_GGO_C})
  target_link_libraries(clitkMedianImageFilter clitkMedianImageFilterLib clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMedianImageFilter)

  WRAP_GGO(clitkMinMaxMask_GGO_C clitkMinMaxMask.ggo)
  add_executable(clitkMinMaxMask clitkMinMaxMask.cxx ${clitkMinMaxMask_GGO_C})
  target_link_libraries(clitkMinMaxMask clitkCommon  )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMinMaxMask)

  WRAP_GGO(clitkAutoCrop_GGO_C clitkAutoCrop.ggo)
  add_executable(clitkAutoCrop clitkAutoCrop.cxx ${clitkAutoCrop_GGO_C})
  target_link_libraries(clitkAutoCrop clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkAutoCrop)

  WRAP_GGO(clitkDicomRTStruct2Image_GGO_C clitkDicomRTStruct2Image.ggo)
  add_executable(clitkDicomRTStruct2Image clitkDicomRTStruct2Image.cxx ${clitkDicomRTStruct2Image_GGO_C})
  target_link_libraries(clitkDicomRTStruct2Image clitkDicomRTStruct)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicomRTStruct2Image)

  if(CLITK_USE_SYSTEM_GDCM)
    WRAP_GGO(clitkImage2DicomRTStruct_GGO_C clitkImage2DicomRTStruct.ggo)
    add_executable(clitkImage2DicomRTStruct clitkImage2DicomRTStruct.cxx ${clitkImage2DicomRTStruct_GGO_C})
    target_link_libraries(clitkImage2DicomRTStruct clitkDicomRTStruct)

    #include pugiXML to parse XML
    WRAP_GGO(clitkXml2DicomRTStruct_GGO_C clitkXml2DicomRTStruct.ggo)
    add_executable(clitkXml2DicomRTStruct ../utilities/pugixml/pugixml.cpp clitkXml2DicomRTStruct.cxx ${clitkXml2DicomRTStruct_GGO_C})
    target_link_libraries(clitkXml2DicomRTStruct clitkDicomRTStruct)
  endif()

  WRAP_GGO(clitkComposeVF_GGO_C clitkComposeVF.ggo)
  add_executable(clitkComposeVF clitkComposeVFGenericFilter.cxx clitkComposeVF.cxx ${clitkComposeVF_GGO_C})
  target_link_libraries(clitkComposeVF clitkCommon  )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkComposeVF)

  WRAP_GGO(clitkMergeSequence_GGO_C clitkMergeSequence.ggo)
  add_executable(clitkMergeSequence clitkMergeSequence.cxx clitkMergeSequenceGenericFilter.cxx ${clitkMergeSequence_GGO_C})
  target_link_libraries(clitkMergeSequence clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMergeSequence)

  WRAP_GGO(clitkBackProjectImage_GGO_C clitkBackProjectImage.ggo)
  add_executable(clitkBackProjectImage clitkBackProjectImage.cxx clitkBackProjectImageGenericFilter.cxx ${clitkBackProjectImage_GGO_C})
  target_link_libraries(clitkBackProjectImage clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBackProjectImage)

  add_executable(clitkCropImage clitkCropImage.cxx ${clitkCropImage_GGO_C})
  target_link_libraries(clitkCropImage clitkCropImageLib clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkCropImage)

  # WRAP_GGO(clitkExtractSlice_GGO_C clitkExtractSlice.ggo)
  # add_executable(clitkExtractSlice clitkExtractSlice.cxx clitkExtractSliceGenericFilter.cxx ${clitkExtractSlice_GGO_C})
  # target_link_libraries(clitkExtractSlice clitkCommon)
  #set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkExtractSlice)

  WRAP_GGO(clitkFlipImage_GGO_C clitkFlipImage.ggo)
  add_executable(clitkFlipImage clitkFlipImage.cxx clitkFlipImageGenericFilter.cxx ${clitkFlipImage_GGO_C})
  target_link_libraries(clitkFlipImage clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkFlipImage)

  WRAP_GGO(clitkMirrorPadImage_GGO_C clitkMirrorPadImage.ggo)
  add_executable(clitkMirrorPadImage clitkMirrorPadImage.cxx clitkMirrorPadImageGenericFilter.cxx ${clitkMirrorPadImage_GGO_C})
  target_link_libraries(clitkMirrorPadImage clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMirrorPadImage)

  WRAP_GGO(clitkImageMoment_GGO_C clitkImageMoment.ggo)
  add_executable(clitkImageMoment clitkImageMoment.cxx clitkImageMomentGenericFilter.cxx ${clitkImageMoment_GGO_C})
  target_link_libraries(clitkImageMoment clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageMoment)

  WRAP_GGO(clitkImageStatistics_GGO_C clitkImageStatistics.ggo)
  add_executable(clitkImageStatistics clitkImageStatistics.cxx clitkImageStatisticsGenericFilter.cxx ${clitkImageStatistics_GGO_C})
  target_link_libraries(clitkImageStatistics clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageStatistics)

  WRAP_GGO(clitkSUVPeak_GGO_C clitkSUVPeak.ggo)
  add_executable(clitkSUVPeak clitkSUVPeak.cxx clitkSUVPeakGenericFilter.cxx ${clitkSUVPeak_GGO_C})
  target_link_libraries(clitkSUVPeak clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSUVPeak)

  WRAP_GGO(clitkRTStructStatistics_GGO_C clitkRTStructStatistics.ggo)
  add_executable(clitkRTStructStatistics clitkRTStructStatistics.cxx clitkRTStructStatisticsGenericFilter.cxx ${clitkRTStructStatistics_GGO_C})
  target_link_libraries(clitkRTStructStatistics clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkRTStructStatistics)

  WRAP_GGO(clitkVFConvert_GGO_C clitkVFConvert.ggo)
  add_executable(clitkVFConvert clitkVFConvert.cxx clitkVFConvertGenericFilter.cxx ${clitkVFConvert_GGO_C})
  target_link_libraries(clitkVFConvert clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVFConvert)

  WRAP_GGO(clitkImageToVectorImage_GGO_C clitkImageToVectorImage.ggo)
  add_executable(clitkImageToVectorImage  clitkImageToVectorImage.cxx clitkImageToVectorImageGenericFilter.cxx ${clitkImageToVectorImage_GGO_C})
  target_link_libraries(clitkImageToVectorImage  clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageToVectorImage)

  WRAP_GGO(clitkVectorImageToImage_GGO_C clitkVectorImageToImage.ggo)
  add_executable(clitkVectorImageToImage  clitkVectorImageToImage.cxx clitkVectorImageToImageGenericFilter.cxx ${clitkVectorImageToImage_GGO_C})
  target_link_libraries(clitkVectorImageToImage  clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkVectorImageToImage)
  
  WRAP_GGO(clitkNVectorImageTo4DImage_GGO_C clitkNVectorImageTo4DImage.ggo)
  add_executable(clitkNVectorImageTo4DImage  clitkNVectorImageTo4DImage.cxx clitkNVectorImageTo4DImageGenericFilter.cxx ${clitkNVectorImageTo4DImage_GGO_C})
  target_link_libraries(clitkNVectorImageTo4DImage  clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkNVectorImageTo4DImage)

  WRAP_GGO(clitk4DImageToNVectorImage_GGO_C clitk4DImageToNVectorImage.ggo)
  add_executable(clitk4DImageToNVectorImage  clitk4DImageToNVectorImage.cxx clitk4DImageToNVectorImageGenericFilter.cxx ${clitk4DImageToNVectorImage_GGO_C})
  target_link_libraries(clitk4DImageToNVectorImage  clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitk4DImageToNVectorImage)

  add_executable(clitkMIP clitkMIP.cxx clitkMIPGenericFilter.cxx)
  target_link_libraries(clitkMIP clitkMIPLib clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMIP)

  WRAP_GGO(clitkSum_GGO_C clitkSum.ggo)
  add_executable(clitkSum clitkSum.cxx ${clitkSum_GGO_C})
  target_link_libraries(clitkSum clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkSum)

  WRAP_GGO(clitkExtrude_GGO_C clitkExtrude.ggo)
  add_executable(clitkExtrude clitkExtrude.cxx ${clitkExtrude_GGO_C})
  target_link_libraries(clitkExtrude clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkExtrude)

  WRAP_GGO(clitkTransformLandmarks_GGO_C clitkTransformLandmarks.ggo)
  add_executable(clitkTransformLandmarks clitkTransformLandmarks.cxx ${clitkTransformLandmarks_GGO_C})
  target_link_libraries(clitkTransformLandmarks clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkTransformLandmarks)

  WRAP_GGO(clitkMaskLandmarks_GGO_C clitkMaskLandmarks.ggo)
  add_executable(clitkMaskLandmarks clitkMaskLandmarks.cxx ${clitkMaskLandmarks_GGO_C})
  target_link_libraries(clitkMaskLandmarks clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMaskLandmarks)

  WRAP_GGO(clitkJacobianImage_GGO_C clitkJacobianImage.ggo)
  add_executable(clitkJacobianImage clitkJacobianImage.cxx clitkJacobianImageGenericFilter.cxx ${clitkJacobianImage_GGO_C})
  target_link_libraries(clitkJacobianImage clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkJacobianImage)

  WRAP_GGO(clitkPadImage_GGO_C clitkPadImage.ggo)
  add_executable(clitkPadImage clitkPadImage.cxx clitkPadImageGenericFilter.cxx ${clitkPadImage_GGO_C})
  target_link_libraries(clitkPadImage clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkPadImage)

  WRAP_GGO(clitkCatImage_GGO_C clitkCatImage.ggo)
  add_executable(clitkCatImage clitkCatImage.cxx ${clitkCatImage_GGO_C})
  target_link_libraries(clitkCatImage clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkCatImage)

  WRAP_GGO(clitkAnisotropicDiffusion_GGO_C clitkAnisotropicDiffusion.ggo)
  add_executable(clitkAnisotropicDiffusion clitkAnisotropicDiffusion.cxx
    clitkAnisotropicDiffusionGenericFilter.cxx
    ${clitkAnisotropicDiffusion_GGO_C})
  target_link_libraries(clitkAnisotropicDiffusion clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkAnisotropicDiffusion)

  WRAP_GGO(clitkChangeImageInfo_GGO_C clitkChangeImageInfo.ggo)
  add_executable(clitkChangeImageInfo clitkChangeImageInfo.cxx
    clitkChangeImageInfoGenericFilter.cxx
    ${clitkChangeImageInfo_GGO_C})
  target_link_libraries(clitkChangeImageInfo clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkChangeImageInfo)

  WRAP_GGO(clitkMergeAsciiDoseActor_GGO_C clitkMergeAsciiDoseActor.ggo)
  add_executable(clitkMergeAsciiDoseActor clitkMergeAsciiDoseActor.cxx ${clitkMergeAsciiDoseActor_GGO_C})
  target_link_libraries(clitkMergeAsciiDoseActor ITKCommon clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMergeAsciiDoseActor)

  WRAP_GGO(clitkImageUncertainty_GGO_C clitkImageUncertainty.ggo)
  add_executable(clitkImageUncertainty clitkImageUncertainty.cxx clitkImageUncertainty_ggo.c)
  target_link_libraries(clitkImageUncertainty clitkCommon ${ITK_LIBRARIES})
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageUncertainty)

  WRAP_GGO(clitkNormalizeImageFilter_GGO_C clitkNormalizeImageFilter.ggo)
  add_executable(clitkNormalizeImageFilter clitkNormalizeImageFilter.cxx ${clitkNormalizeImageFilter_GGO_C})
  target_link_libraries(clitkNormalizeImageFilter clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkNormalizeImageFilter)

  WRAP_GGO(clitkImageGradientMagnitude_GGO_C clitkImageGradientMagnitude.ggo)
  add_executable(clitkImageGradientMagnitude clitkImageGradientMagnitude.cxx ${clitkImageGradientMagnitude_GGO_C})
  target_link_libraries(clitkImageGradientMagnitude clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageGradientMagnitude)

  WRAP_GGO(clitkImageLaplacian_GGO_C clitkImageLaplacian.ggo)
  add_executable(clitkImageLaplacian clitkImageLaplacian.cxx ${clitkImageLaplacian_GGO_C})
  target_link_libraries(clitkImageLaplacian clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageLaplacian)

  WRAP_GGO(clitkImageIntensityWindowing_GGO_C clitkImageIntensityWindowing.ggo)
  add_executable(clitkImageIntensityWindowing clitkImageIntensityWindowing.cxx ${clitkImageIntensityWindowing_GGO_C})
  target_link_libraries(clitkImageIntensityWindowing clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkImageIntensityWindowing)

  WRAP_GGO(clitkMaskOfIntegratedIntensity_GGO_C clitkMaskOfIntegratedIntensity.ggo)
  add_executable(clitkMaskOfIntegratedIntensity clitkMaskOfIntegratedIntensity.cxx ${clitkMaskOfIntegratedIntensity_GGO_C})
  target_link_libraries(clitkMaskOfIntegratedIntensity clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMaskOfIntegratedIntensity)


  WRAP_GGO(clitkBlurImage_GGO_C clitkBlurImage.ggo)
  add_executable(clitkBlurImage clitkBlurImage.cxx ${clitkBlurImage_GGO_C})
  target_link_libraries(clitkBlurImage clitkCommon )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBlurImage)

  #=========================================================
  option(CLITK_USE_ROOT "Build experimental tools using root" OFF)
  if (CLITK_USE_ROOT)
    find_package(ROOT REQUIRED)
    if(ROOT_FOUND)
      message(STATUS "ROOT found : ${ROOT_LIBRARY_DIR} ${ROOT_INCLUDE_DIR} ${ROOT_LIBRARIES}")
    else(ROOT_FOUND)
      message(FATAL_ERROR
        "Cannot build without ROOT.  Please set ROOTSYS environement variable.")
    endif(ROOT_FOUND)
    include_directories(${ROOT_INCLUDE_DIR})
    link_directories(${ROOT_LIBRARY_DIR})
    WRAP_GGO(clitkMergeRootFiles_GGO_C clitkMergeRootFiles.ggo)
    add_executable(clitkMergeRootFiles clitkMergeRootFiles.cxx GateMergeManager.cc ${clitkMergeRootFiles_GGO_C})
    target_link_libraries(clitkMergeRootFiles ${ROOT_LIBRARIES})
    set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMergeRootFiles)
  endif()
  #=========================================================


  #=========================================================
  if(CLITK_EXPERIMENTAL)
    WRAP_GGO(clitkBinaryImageToMesh_GGO_C clitkBinaryImageToMesh.ggo)
    add_executable(clitkBinaryImageToMesh clitkBinaryImageToMesh.cxx ${clitkBinaryImageToMesh_GGO_C})
    target_link_libraries(clitkBinaryImageToMesh clitkCommon)
    set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkBinaryImageToMesh)

    WRAP_GGO(clitkMeshToBinaryImage_GGO_C clitkMeshToBinaryImage.ggo)
    add_executable(clitkMeshToBinaryImage clitkMeshToBinaryImage.cxx ${clitkMeshToBinaryImage_GGO_C})
    target_link_libraries(clitkMeshToBinaryImage clitkCommon)
    set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMeshToBinaryImage)

    WRAP_GGO(clitkMeshViewer_GGO_C clitkMeshViewer.ggo)
    add_executable(clitkMeshViewer clitkMeshViewer.cxx ${clitkMeshViewer_GGO_C})
    target_link_libraries(clitkMeshViewer clitkCommon)
    set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkMeshViewer)
  endif(CLITK_EXPERIMENTAL)
  #=========================================================


  #=========================================================
  WRAP_GGO(clitkDice_GGO_C clitkDice.ggo)
  add_executable(clitkDice clitkDice.cxx ${clitkDice_GGO_C})
  target_link_libraries(clitkDice clitkSegmentationGgoLib clitkCommon ${ITK_LIBRARIES} )
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDice)

  WRAP_GGO(clitkDicomRTPlan2Gate_GGO_C clitkDicomRTPlan2Gate.ggo)
  add_executable(clitkDicomRTPlan2Gate clitkDicomRTPlan2Gate.cxx clitkDicomRTPlan2Gate_ggo.c)
  target_link_libraries(clitkDicomRTPlan2Gate clitkCommon)
  set(TOOLS_INSTALL ${TOOLS_INSTALL} clitkDicomRTPlan2Gate)
  #=========================================================


  install(TARGETS ${TOOLS_INSTALL} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

endif(CLITK_BUILD_TOOLS)
