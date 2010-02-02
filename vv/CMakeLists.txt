cmake_minimum_required(VERSION 2.4)
#=========================================================
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/common.cmake)
#=========================================================

#Set a reasonable build mode default if the user hasn't set any
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT CMAKE_BUILD_TYPE)

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
  #clitkGGO
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
  QVTKWidget.cxx
  QTreePushButton.cxx
  vvResamplerDialog.cxx
  vvSegmentationDialog.cxx
  vvSurfaceViewerDialog.cxx
  vvMainWindow.cxx
  vvDeformationDialog.cxx
  vvInit.cxx
  vvImageWarp.cxx
  nkitkXDRImageIOFactory.cxx  
  nkitkXDRImageIOReader.cxx  
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
  vvCropDialog.cxx
  vvMidPosition.cxx
  vvImageMapToWLColors.cxx
  vvIntensityValueSlider.cxx
  vvToolManager.cxx
  vvToolCreatorBase.cxx
  vvToolBinarize.cxx
  vvToolInputSelectorWidget.cxx
  )

QT4_WRAP_CPP(vv_SRCS 
  vvMainWindow.h 
  QVTKWidget.h 
  QTreePushButton.h 
  vvInfoPanel.h 
  vvLinkPanel.h 
  vvOverlayPanel.h 
  vvLandmarksPanel.h 
  vvDocumentation.h  
  vvHelpDialog.h  
  vvProgressDialog.h 
  vvQDicomSeriesSelector.h 
  vvResamplerDialog.h
  vvSegmentationDialog.h
  vvSurfaceViewerDialog.h
  vvDeformationDialog.h
  vvSlicerManager.h
  vvStructSelector.h
  vvCropDialog.h
  vvIntensityValueSlider.h
  vvToolCreatorBase.h
  vvToolBinarize.h
  vvToolInputSelectorWidget.h
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
  qt_ui/vvResamplerDialog.ui
  qt_ui/vvDeformationDialog.ui
  qt_ui/vvStructSelector.ui
  qt_ui/vvCropDialog.ui
  qt_ui/vvDummyWindow.ui #For testing
  qt_ui/vvIntensityValueSlider.ui
  qt_ui/vvToolBinarize.ui
  qt_ui/vvToolInputSelectorWidget.ui
  )

SET(vvUI_RCCS vvIcons.qrc)
QT4_ADD_RESOURCES(vv_SRCS ${vvUI_RCCS})

#=========================================================
#support for parallel deformable registration with OpenMP
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -march=native")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

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
  RUNTIME DESTINATION bin CONFIGURATIONS relwithdebinfo
  LIBRARY DESTINATION lib CONFIGURATIONS relwithdebinfo
  ARCHIVE DESTINATION lib CONFIGURATIONS relwithdebinfo)
INSTALL(FILES ${vv_HDRS} DESTINATION include)

IF(WIN32)
  INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/ducky.png DESTINATION bin)
  INSTALL (FILES ${EXECUTABLE_OUTPUT_PATH}relwithdebinfo/vv.exe DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtCore4.dll DESTINATION bin)
  INSTALL (FILES ${QT_PLUGINS_DIR}/../bin/QtGui4.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkCommon.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtksys.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkRendering.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkGraphics.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkImaging.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkFiltering.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkGenericFiltering.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkIO.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/verdict.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/QVTK.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/QVTKWidgetPlugin.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkDICOMParser.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkexoIIc.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkexpat.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkmetaio.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkftgl.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkfreetype.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkGenericFiltering.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkHybrid.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkInfovis.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkjpeg.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtklibxml2.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkNetCDF.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkpng.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtktiff.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkViews.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkVolumeRendering.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkWidgets.dll DESTINATION bin)
  INSTALL (FILES ${VTK_DIR}/bin/relwithdebinfo/vtkzlib.dll DESTINATION bin)
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
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "vv ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\ducky.png")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin/relwithdebinfo\\\\vv.exe")
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
