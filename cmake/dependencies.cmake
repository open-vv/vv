#=========================================================
# Find ITK (required)
find_package(ITK 6.0 REQUIRED COMPONENTS
  ITKBinaryMathematicalMorphology
  ITKCommon
  ITKDICOMParser
  ITKDistanceMap
  ITKFFT
  ITKIOBMP
  ITKIOBioRad
  ITKIOBruker
  ITKIOGDCM
  ITKIOGE
  ITKIOGIPL
  ITKIOHDF5
  ITKIOImageBase
  ITKIOJPEG
  ITKIOJPEG2000
  ITKIOLSM
  ITKIOMINC
  ITKIOMRC
  ITKIOMeshBYU
  ITKIOMeta
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ITKIOStimulate
  ITKIOTIFF
  ITKIOVTK
  ITKIOXML
  ITKImageFeature
  ITKImageIntensity
  ITKLabelMap
  ITKPDEDeformableRegistration
  ITKRegionGrowing
  ITKStatistics
  ITKTransform
  ITKVTK
  RTK
)
include("${ITK_USE_FILE}")
#=========================================================

#=========================================================
# Find VTK (required)
if(CLITK_BUILD_VV)
  set(VTK_REQUIRED_COMPONENTS
  ChartsCore
  CommonCore
  CommonDataModel
  CommonExecutionModel
  CommonMath
  CommonMisc
  CommonTransforms
  FiltersCore
  FiltersGeneral
  FiltersHybrid
  FiltersModeling
  FiltersSources
  GUISupportQt
  ImagingColor
  ImagingCore
  ImagingMath
  ImagingMorphological
  ImagingStatistics
  ImagingStencil
  InteractionImage
  InteractionStyle
  IOCore
  IOGeometry
  IOExport
  IOImage
  IOLegacy
  IOMovie
  IOXML
  RenderingAnnotation
  RenderingContext2D
  RenderingContextOpenGL2
  RenderingCore
  RenderingLOD
  RenderingOpenGL2
  ViewsContext2D)
else(CLITK_BUILD_VV)
  set(VTK_REQUIRED_COMPONENTS
  ChartsCore
  CommonCore
  CommonDataModel
  CommonExecutionModel
  CommonMath
  CommonMisc
  CommonTransforms
  FiltersCore
  FiltersGeneral
  FiltersHybrid
  FiltersModeling
  FiltersSources
  ImagingColor
  ImagingCore
  ImagingMath
  ImagingMorphological
  ImagingStatistics
  ImagingStencil
  InteractionImage
  InteractionStyle
  IOCore
  IOGeometry
  IOExport
  IOImage
  IOLegacy
  IOMovie
  IOXML
  RenderingAnnotation
  RenderingContext2D
  RenderingContextOpenGL2
  RenderingCore
  RenderingLOD
  RenderingOpenGL2
  ViewsContext2D)
endif(CLITK_BUILD_VV)
find_package(VTK 9.5 REQUIRED COMPONENTS ${VTK_REQUIRED_COMPONENTS})
#========================================================

#=========================================================
# Find gengetopt, will create a target exe if not found
set(CMAKE_MODULE_PATH "${CLITK_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Gengetopt)
#=========================================================

#=========================================================
# Find libstatgrab is installed, add clitkMemoryUsage.cxx in the library
if(NOT DEFINED CLITK_MEMORY_INFO OR CLITK_MEMORY_INFO)
  find_library(LIBSTATGRAB NAMES statgrab PATHS)
  if(${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
#  message("Install libstatgrab (http://www.i-scream.org/libstatgrab/) for memory usage information")
    set(CLITK_MEMORY_INFO OFF)
  else(${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
    set(CLITK_MEMORY_INFO ON)
  endif(${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
endif()
#=========================================================


#=========================================================
### Check if ITK was compiled with SYSTEM_GDCM = ON (Not possible anymore with ITK 4.13)
### Add option to activate external GDCM in clitk
set(CLITK_USE_SYSTEM_GDCM FALSE)
if(CLITK_EXTERNAL_GDCM)
  message("Be sure to use external GDCM with ITK")
  set(CLITK_USE_SYSTEM_GDCM TRUE)
endif()


