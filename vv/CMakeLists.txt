cmake_minimum_required(VERSION 2.4)
#=========================================================
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
#=========================================================
# Find ITK (required)
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)
#=========================================================

#=========================================================
# Find QT
FIND_PACKAGE(Qt4 REQUIRED)

#=========================================================
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${VTK_INCLUDE_DIR}
  ${QT_INCLUDE_DIR}
  ${QT_QTGUI_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ../common
  ../tools
  )

#=========================================================
# To avoid warning with some version
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
#SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/../lib)

#=========================================================
LINK_LIBRARIES (
  #ITKIO
  clitkCommon
  clitkDicomRTStruct
  clitkFilters
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  vtkCommon
  vtkRendering
  vtkIO
  vtkFiltering
  vtkGraphics
  vtkWidgets
  vtkImaging
  QVTK
  )

LINK_DIRECTORIES(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} )

#=========================================================

OPTION(CLITK_VV_USE_BDCM "Build vv with Dicom selector bdcm" OFF)

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
#  vvToolFoo.cxx
#  vvToolFooWithWidgetBase.cxx
  vvToolCropImage.cxx
  vvToolBinarize.cxx
  vvToolSimpleInputSelectorWidget.cxx
  vvToolInputSelectorWidget.cxx
  vvImageContour.cxx
  vvToolImageArithm.cxx
  vvToolConvert.cxx
  # vvToolStructureSetManager.cxx
#   vvStructureSetActor.cxx
#   vvROIActor.cxx
  vvToolResample.cxx
  )

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
  vvToolBinarize.h
  vvToolSimpleInputSelectorWidget.h
  vvToolInputSelectorWidget.h
  vvToolWidgetBase.h
  vvToolCropImage.h
  vvToolImageArithm.h
  vvToolConvert.h
  # vvToolStructureSetManager.h
 #  vvStructureSetActor.h
#   vvROIActor.h
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
#  qt_ui/vvToolFoo.ui
  qt_ui/vvToolCropImage.ui
  qt_ui/vvToolBinarize.ui
  qt_ui/vvToolImageArithm.ui
  # qt_ui/vvToolStructureSetManager.ui
  qt_ui/vvToolResample.ui
  )

SET(vvUI_RCCS vvIcons.qrc)
QT4_ADD_RESOURCES(vv_SRCS ${vvUI_RCCS})

#=========================================================
#support for parallel deformable registration with OpenMP
IF(NOT __APPLE__)
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -march=native")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(NOT __APPLE__)
#=========================================================

IF (CLITK_VV_USE_BDCM)
  FIND_PACKAGE(bdcm)
  IF(bdcm_FOUND)
    INCLUDE(${bdcm_USE_FILE})
  ELSE(bdcm_FOUND)
    MESSAGE(FATAL_ERROR
      "Cannot build without BDCM.  Please set BDCM_DIR.")
  ENDIF(bdcm_FOUND)
  #LINK_DIRECTORIES(/home/dsarrut/src/bdcm/build/)
  #INCLUDE_DIRECTORIES(/home/dsarrut/src/bdcm/src2/)

  FIND_PACKAGE(GDCM)
  IF(GDCM_FOUND)
    INCLUDE(${GDCM_USE_FILE})
  ELSE(GDCM_FOUND)
    MESSAGE(FATAL_ERROR
      "Cannot build without GDCM.  Please set GDCM_DIR.")
  ENDIF(GDCM_FOUND)

ENDIF (CLITK_VV_USE_BDCM)

IF(WIN32)
  SET(EXE_ICON vvIcon.rc)
ENDIF(WIN32)

ADD_DEFINITIONS(-DQT_DLL)

ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)

ADD_EXECUTABLE(vv ${vv_SRCS} vv.cxx ${vv_UI_CXX} ${EXE_ICON})
TARGET_LINK_LIBRARIES(vv ${QT_LIBRARIES} ${ITK_LIBRARIES} ${VTK_LIBRARIES} )

#test executable
#QT4_WRAP_CPP(VVS_MOC_OUTFILE vvs.h)
#QT4_WRAP_UI(VVS_UI_OUTFILE qt_ui/vvDummyWindow.ui)
#ADD_EXECUTABLE(vvs ${vv_SRCS} vvs.cxx ${vv_UI_CXX} ${VVS_UI_OUTFILE}
#${VVS_MOC_OUTFILE} ${EXE_ICON})
#TARGET_LINK_LIBRARIES(vvs ${QT_LIBRARIES} ${ITK_LIBRARIES} ${VTK_LIBRARIES} )

IF (CLITK_VV_USE_BDCM)
  TARGET_LINK_LIBRARIES(vv ${QT_LIBRARIES} ${ITK_LIBRARIES} ${VTK_LIBRARIES} bdcm)
ELSE (CLITK_VV_USE_BDCM)
  #TARGET_LINK_LIBRARIES(vv ${QT_LIBRARIES} ${ITK_LIBRARIES} ${VTK_LIBRARIES} )
ENDIF (CLITK_VV_USE_BDCM)

#IF( MINGW )
#    # resource compilation for MinGW
#    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vvIcons.o
#                        COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/vvIcons.qrc -o ${CMAKE_CURRENT_BINARY_DIR}/vvIcons.o )
#    SET(vv_SRCS ${vv_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/vvIcons.o)
#ENDIF (MINGW)

#IF( MINGW )
#  ADD_EXECUTABLE(vv WIN32 ${vv_SRCS} vv.cxx ${vv_UI_CXX} ${EXE_ICON})
#ELSE (MINGW)
#  ADD_EXECUTABLE(vv ${vv_SRCS} vv.cxx ${vv_UI_CXX} ${EXE_ICON})
#ENDIF (MINGW)
#TARGET_LINK_LIBRARIES(vv ${QT_LIBRARIES} ${ITK_LIBRARIES} ${VTK_LIBRARIES} QVTKWidgetPlugin )

#=========================================================
# Installation file
INSTALL(TARGETS vv vv
  RUNTIME DESTINATION bin CONFIGURATIONS ${CMAKE_BUILD_TYPE}
  LIBRARY DESTINATION lib CONFIGURATIONS ${CMAKE_BUILD_TYPE}
  ARCHIVE DESTINATION lib CONFIGURATIONS ${CMAKE_BUILD_TYPE})
INSTALL(FILES ${vv_HDRS} DESTINATION include)

IF(WIN32)
  INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/ducky.png DESTINATION bin)
  INSTALL (FILES ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/vv.exe DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtCore4.dll DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtGui4.dll DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtWebKit4.dll DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtOpenGL4.dll DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/phonon4.dll DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtXmlPatterns4.dll DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtNetwork4.dll DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtSql4.dll DESTINATION bin)
  INSTALL (FILES ${ITK_DIR}/bin/${CMAKE_BUILD_TYPE}/itkCommon.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkCommon.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtksys.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkRendering.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkGraphics.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkImaging.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkFiltering.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkGenericFiltering.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkIO.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkverdict.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/QVTK.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkDICOMParser.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkexoIIc.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkexpat.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkmetaio.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkftgl.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkfreetype.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkHybrid.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkInfovis.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkjpeg.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtklibxml2.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkNetCDF.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkpng.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtktiff.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkViews.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkWidgets.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkzlib.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkQtChart.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE}/vtkalglib.dll DESTINATION bin)
ENDIF(WIN32)

IF(UNIX)
  INSTALL (FILES ${EXECUTABLE_OUTPUT_PATH}/vv DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE  OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
  INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/ducky.png DESTINATION bin)
  INSTALL (FILES ${EXECUTABLE_OUTPUT_PATH}vv DESTINATION bin)
ENDIF(UNIX)

#=========================================================
# CPack options
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vv, the 4D slicer : let's jump into a new dimension !")
SET(CPACK_PACKAGE_VENDOR "Creatis, CLB/RIO Team")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "vv ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
#  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\ducky.png")
#  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin/${CMAKE_BUILD_TYPE}\\\\vv.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} The 4D slicer")
  SET(CPACK_NSIS_HELP_LINK "http://www.creatis.insa-lyon.fr/rio")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.creatis.insa-lyon.fr/rio")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/vv")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "vv" "vv")
INCLUDE(CPack)
