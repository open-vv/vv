cmake_minimum_required(VERSION 2.4)

#=========================================================
# To avoid warning with some version
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
#=========================================================

#=========================================================
# List of vv source files
SET(vv_SRCS
  vvInfoPanel.cxx
  vvLinkPanel.cxx
  vvOverlayPanel.cxx
  vvLandmarksPanel.cxx
  vvQProgressDialogITKCommand.cxx
  vvQDicomSeriesSelector.cxx
  QTreePushButton.cxx
  vvSegmentationDialog.cxx
  vvSurfaceViewerDialog.cxx
  vvMainWindowBase.cxx
  vvMainWindow.cxx
  vvDeformationDialog.cxx
  vvImageWarp.cxx
  vvDeformableRegistration.cxx
  vtkVOXImageWriter.cxx
  vvInteractorStyleNavigator.cxx
  vvSlicer.cxx
  vvImageReader.cxx
  vvImageReader.txx
  vvImageWriter.cxx
  vvImageWriter.txx
  vvLandmarks.cxx
  vvLandmarksGlyph.cxx
  vvGlyphSource.cxx
  vvGlyph2D.cxx
  vvSlicerManager.cxx
  vvSlicerManagerCommand.cxx
  vvUtils.cxx
  vvMaximumIntensityProjection.cxx
  vvMesh.cxx
  vvMeshActor.cxx
  vvMeshReader.cxx
  vvStructSelector.cxx
  vvMidPosition.cxx
  vvImageMapToWLColors.cxx
  vvIntensityValueSlider.cxx
  vvToolManager.cxx
  vvToolCreatorBase.cxx
  vvToolWidgetBase.cxx
  vvToolMedianFilter.cxx
  vvToolRigidReg.cxx
#  vvToolFoo.cxx
#  vvToolFooWithWidgetBase.cxx
  vvToolCropImage.cxx
  vvToolBinarize.cxx
  vvToolSimpleInputSelectorWidget.cxx
  vvToolInputSelectorWidget.cxx
  vvImageContour.cxx
  vvBinaryImageOverlayActor.cxx
  vvToolImageArithm.cxx
  vvToolConvert.cxx
  vvToolStructureSetManager.cxx
  vvStructureSetActor.cxx
  vvROIActor.cxx
  vvToolResample.cxx
  vvBlendImageActor.cxx
  )
#=========================================================

#=========================================================
# Qt related commands
FIND_PACKAGE(Qt4 REQUIRED)

QT4_WRAP_CPP(vv_SRCS 
  vvMainWindowBase.h 
  vvMainWindow.h 
  QTreePushButton.h 
  vvInfoPanel.h 
  vvLinkPanel.h 
  vvOverlayPanel.h 
  vvLandmarksPanel.h 
  vvDocumentation.h  
  vvHelpDialog.h  
  vvProgressDialog.h 
  vvQDicomSeriesSelector.h 
  vvSegmentationDialog.h
  vvSurfaceViewerDialog.h
  vvDeformationDialog.h
  vvSlicerManager.h
  vvStructSelector.h
  vvIntensityValueSlider.h
  vvToolCreatorBase.h
#  vvToolFoo.h
#  vvToolFooWithWidgetBase.h
  vvToolMedianFilter.h
  vvToolRigidReg.h
  vvToolBinarize.h
  vvToolSimpleInputSelectorWidget.h
  vvToolInputSelectorWidget.h
  vvToolWidgetBase.h
  vvToolCropImage.h
  vvToolImageArithm.h
  vvToolConvert.h
  vvToolStructureSetManager.h
  vvStructureSetActor.h
  vvROIActor.h
  vvToolResample.h
  )

QT4_WRAP_UI(vv_UI_CXX 
  qt_ui/vvMainWindow.ui 
  qt_ui/vvInfoPanel.ui 
  qt_ui/vvLinkPanel.ui 
  qt_ui/vvOverlayPanel.ui 
  qt_ui/vvLandmarksPanel.ui 
  qt_ui/vvHelpDialog.ui 
  qt_ui/vvDocumentation.ui 
  qt_ui/vvProgressDialog.ui 
  qt_ui/vvDicomSeriesSelector.ui 
  qt_ui/vvSegmentationDialog.ui
  qt_ui/vvSurfaceViewerDialog.ui
  qt_ui/vvDeformationDialog.ui
  qt_ui/vvStructSelector.ui
  qt_ui/vvDummyWindow.ui #For testing
  qt_ui/vvIntensityValueSlider.ui
  qt_ui/vvToolSimpleInputSelectorWidget.ui
  qt_ui/vvToolInputSelectorWidget.ui
  qt_ui/vvToolWidgetBase.ui
  qt_ui/vvToolMedianFilter.ui
  qt_ui/vvToolRigidReg.ui
#  qt_ui/vvToolFoo.ui
  qt_ui/vvToolCropImage.ui
  qt_ui/vvToolBinarize.ui
  qt_ui/vvToolImageArithm.ui
  qt_ui/vvToolStructureSetManager.ui
  qt_ui/vvToolResample.ui
  )

QT4_ADD_RESOURCES(vv_SRCS vvIcons.qrc)

ADD_DEFINITIONS(-DQT_DLL)
ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)

INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${QT_INCLUDE_DIR}
  ${QT_QTGUI_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)
#=========================================================

#=========================================================
#support for parallel deformable registration with OpenMP
IF(NOT __APPLE__)
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -march=native")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(NOT __APPLE__)
#=========================================================

#=========================================================
INCLUDE_DIRECTORIES(
  ../common
  ../tools
  )

LINK_DIRECTORIES(${QT_INCLUDES})
ADD_EXECUTABLE(vv ${vv_SRCS} vv.cxx ${vv_UI_CXX} ${EXE_ICON})
TARGET_LINK_LIBRARIES(vv clitkCommon clitkDicomRTStruct clitkFilters ${ITK_LIBRARIES} QVTK vtkHybrid)

IF (CLITK_VV_USE_BDCM)
  TARGET_LINK_LIBRARIES(vv bdcm)
ENDIF (CLITK_VV_USE_BDCM)
#=========================================================

#=========================================================
# Install options (also used by CPack)
IF(UNIX AND NOT APPLE)
  INSTALL(TARGETS vv
          DESTINATION .)
  #QT
  FOREACH(LIB QtCore QtGui QtOpenGL QtWebKit QtXmlPatterns QtNetwork QtSql)
    INSTALL(FILES "${QT_LIBRARY_DIR}/lib${LIB}.so.${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
            RENAME "lib${LIB}.so.${QT_VERSION_MAJOR}"
            DESTINATION .)
  ENDFOREACH(LIB)

  #VTK
  FOREACH(LIB QVTK vtkQtChart vtkViews vtkInfovis vtkWidgets vtkHybrid vtkRendering vtkImaging vtkGraphics vtkverdict
              vtkIO vtkFiltering vtkCommon vtkDICOMParser vtkmetaio vtksqlite vtkpng vtktiff vtkjpeg vtkexpat vtksys
              vtkftgl vtkfreetype vtkexoIIc vtkNetCDF vtklibxml2 vtkzlib vtkalglib)
    INSTALL(FILES "${VTK_DIR}/bin/lib${LIB}.so.${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR}.0"
            RENAME "lib${LIB}.so.${VTK_VERSION_MAJOR}.${VTK_VERSION_MINOR}"
            DESTINATION .)
  ENDFOREACH(LIB)

  #ITK
  FOREACH(LIB ITKAlgorithms ITKStatistics ITKFEM itkNetlibSlatec ITKNumerics ITKBasicFilters ITKIO ITKNrrdIO itkgdcm
              itkjpeg12 itkjpeg16 itkopenjpeg itkpng itktiff itkjpeg8 ITKSpatialObject ITKCommon itkvnl_inst
              itkvnl_algo itkv3p_netlib itkvnl itkvcl ITKMetaIO itksys ITKDICOMParser ITKEXPAT ITKniftiio ITKznz itkzlib)
    INSTALL(FILES "${ITK_DIR}/bin/lib${LIB}.so.${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.0"
            RENAME "lib${LIB}.so.${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}"
            DESTINATION .)
    ENDFOREACH(LIB)
ENDIF(UNIX AND NOT APPLE)

IF(WIN32)
  SET(EXE_ICON vvIcon.rc)
ENDIF(WIN32)

#=========================================================
# CPack options
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vv, the 4D slicer : let's jump into a new dimension !")
SET(CPACK_PACKAGE_VENDOR "Creatis-CLB")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "vv")
#IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
#  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\ducky.png")
#  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin/${CMAKE_BUILD_TYPE}\\\\vv.exe")
#  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} The 4D slicer")
#  SET(CPACK_NSIS_HELP_LINK "http://www.creatis.insa-lyon.fr/rio")
#  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.creatis.insa-lyon.fr/rio")
#  SET(CPACK_NSIS_MODIFY_PATH ON)
#ELSE(WIN32 AND NOT UNIX)
#  SET(CPACK_STRIP_FILES "bin/vv")
#  SET(CPACK_SOURCE_STRIP_FILES "")
#ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "vv" "vv")
INCLUDE(CPack)
