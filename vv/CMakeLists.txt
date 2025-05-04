cmake_minimum_required(VERSION 2.4)

#=========================================================
# To avoid warning with some version
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0007 NEW)
endif(COMMAND cmake_policy)
#=========================================================
OPTION(CLITK_USE_PACS_CONNECTION "USE PACS CONNECTION"       OFF)
#=========================================================
# Look for VTK OpenGL version
if(VTK_MAJOR_VERSION VERSION_LESS "6")
  if(CMAKE_VERSION VERSION_LESS "3.12.0")
    add_definitions(-DVTK_OPENGL=1)
  else()
    add_compile_definitions(VTK_OPENGL=1)
  endif()
elseif(VTK_MAJOR_VERSION VERSION_EQUAL "6")
  # Look for OpenGL version
  if(VTK_RENDERING_BACKEND STREQUAL "OpenGL")
    if(CMAKE_VERSION VERSION_LESS "3.12.0")
      add_definitions(-DVTK_OPENGL=1)
    else()
      add_compile_definitions(VTK_OPENGL=1)
    endif()
  # Look for OpenGL2 version
  else()
    if(CMAKE_VERSION VERSION_LESS "3.12.0")
      add_definitions(-DVTK_OPENGL=2)
    else()
      add_compile_definitions(VTK_OPENGL=2)
    endif()
  endif()
else()
  if(CMAKE_VERSION VERSION_LESS "3.12.0")
    add_definitions(-DVTK_OPENGL=2)
  else()
    add_compile_definitions(VTK_OPENGL=2)
  endif()
endif()

#List of vv tools to compile
set(vv_TOOLS
  vvToolMedianFilter
  vvToolRigidReg
  vvToolCropImage
  vvToolBinarize
  vvToolImageArithm
  vvToolMIP
  vvToolConvert ## with dummy vvToolConvert.ui
  vvToolROIManager
  vvToolSegmentation
  ## these ones are for tests (not working)
  # vvToolTest
  # vvToolFoo
  # vvToolFooWithWidgetBase
  # vvToolPlastimatch
  # vvToolConnectedComponentLabeling
  ## these ones are old ones
  # vvToolExtractPatient
  # vvToolExtractLung
  # vvToolStructureSetManager
)
#List of vv tools to compile
#Ensure compatibility with QVTKOpenGL for VTK < v8.1 and QVTKOpenGLNativeWidget for others
set(vv_TOOLS_VTK7
  vvToolProfile
  vvToolHistogram
)
# dependencies of the tools
# Future issues : when tool depend on other tools ... how to manage that ?
#  >> add the relevant $_LIBS variable to the tool that has dependencies?
set(vvToolBinarize_LIBS clitkBinarizeImageLib)
set(vvToolProfile_LIBS clitkProfileImageLib)
set(vvToolHistogram_LIBS clitkHistogramImageLib)
set(vvToolConvert_LIBS clitkImageConvertLib)
set(vvToolExtractPatient_LIBS clitkSegmentationGgoLib)
set(vvToolExtractLung_LIBS clitkSegmentationGgoLib)
set(vvToolCropImage_LIBS clitkCropImageLib)
set(vvToolMIP_LIBS clitkMIPLib)
set(vvToolMedianFilter_LIBS clitkMedianImageFilterLib)
set(vvToolImageArithm_LIBS clitkImageArithmImageLib)
set(vvToolROIManager_LIBS clitkDicomRTStruct)

#=========================================================
# List of vv source files

# in COMMON_WITH_UI -> must have .h and .cxx and .ui
set(vv_COMMON_WITH_UI
  vvInfoPanel
  vvMainWindow
  vvLinkPanel
  vvOverlayPanel
  vvLandmarksPanel
  vvProgressDialog
  vvDeformationDialog
  vvStructSelector
  vvIntensityValueSlider
  vvLabelImageLoaderWidget
  vvToolWidgetBase
  vvToolSimpleInputSelectorWidget
  vvToolInputSelectorWidget
  vvRegisterForm
)
# in COMMON_WITH_UI -> must have .h and .cxx and VTK7.ui
#Ensure compatibility with QVTKOpenGL for VTK < v8.1 and QVTKOpenGLNativeWidget for others
set(vv_COMMON_WITH_UI_VTK7
  vvMainWindow
  vvSegmentationDialog
  vvSurfaceViewerDialog
)

# All others sources
set(vv_SRCS
  vvQProgressDialogITKCommand.cxx
  vvQDicomSeriesSelector.cxx
  QTreePushButton.cxx
  vvMainWindowBase.cxx
  vvImageWarp.cxx
  vvDeformableRegistration.cxx
  vtkVOXImageWriter.cxx
  vvInteractorStyleNavigator.cxx
  vvSlicer.cxx
  vvLandmarks.cxx
  vvLandmarksGlyph.cxx
  vvClipPolyData.cxx
  vvGlyphSource.cxx
  vvGlyph2D.cxx
  vvSlicerManager.cxx
  vvSlicerManagerCommand.cxx
  vvToolHistogramCommand.cxx
  vvUtils.cxx
#  vvMaximumIntensityProjection.cxx 
 vvMesh.cxx
  vvMeshActor.cxx
  vvMeshReader.cxx
  vvMidPosition.cxx
  vvImageContour.cxx
  vvBinaryImageOverlayActor.cxx
  vvStructureSetActor.cxx
  vvSaveState.cxx
  vvReadState.cxx
  vvROIActor.cxx
  vvBlendImageActor.cxx
  vvToolManager.cxx
  vvToolCreatorBase.cxx
  vvToolBaseBase.cxx
)

#=========================================================
# Qt related commands
if(vv_QT_VERSION VERSION_GREATER "4")
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Network REQUIRED)
  find_package(Qt5Designer REQUIRED)
else()
  find_package(Qt4 REQUIRED)
endif()

if(vv_QT_VERSION VERSION_GREATER "4")
  link_directories(${QT5Widgets_LIBRARIES})
  link_directories(${QT5Network_LIBRARIES})
  link_directories(${QT5Designer_LIBRARIES})
else()
  link_directories(${QT_LIBRARY_DIR})
endif()

if(vv_QT_VERSION VERSION_GREATER "4")
  qt5_wrap_cpp(vv_SRCS
    vvMainWindowBase.h
    QTreePushButton.h
    vvDocumentation.h
    vvHelpDialog.h
    vvQDicomSeriesSelector.h
    vvSlicerManager.h
    vvSlicer.h
    vvStructureSetActor.h
    vvROIActor.h
    vvToolCreatorBase.h
    )
else()
  QT4_WRAP_CPP(vv_SRCS
    vvMainWindowBase.h
    QTreePushButton.h
    vvDocumentation.h
    vvHelpDialog.h
    vvQDicomSeriesSelector.h
    vvSlicerManager.h
    vvSlicer.h
    vvStructureSetActor.h
    vvROIActor.h
    vvToolCreatorBase.h
    )
endif()

if(vv_QT_VERSION VERSION_GREATER "4")
  qt5_wrap_ui(vv_UI_CXX
    qt_ui/vvHelpDialog.ui
    qt_ui/vvDocumentation.ui
    qt_ui/vvDicomSeriesSelector.ui
    qt_ui/vvDummyWindow.ui #For testing
    )
else()
  QT4_WRAP_UI(vv_UI_CXX
    qt_ui/vvHelpDialog.ui
    qt_ui/vvDocumentation.ui
    qt_ui/vvDicomSeriesSelector.ui
    qt_ui/vvDummyWindow.ui #For testing
    )
endif()

if(vv_QT_VERSION VERSION_GREATER "4")
  QT5_add_resources(vv_SRCS vvIcons.qrc)
else()
  QT4_ADD_RESOURCES(vv_SRCS vvIcons.qrc)
endif()

# Add DICOM SERVER gui selector if the adequate GDCM is available
if(CLITK_USE_PACS_CONNECTION)
  SET(vv_SRCS ${vv_SRCS}
    vvQPacsConnection.cxx
	vvPacsSettingsDialog.cxx
	vvDicomServerQueryFactory.cxx
	)
  if(vv_QT_VERSION VERSION_GREATER "4")
    qt5_wrap_cpp(vv_SRCS vvQPacsConnection.h
      vvPacsSettingsDialog.h)
    qt5_wrap_ui(vv_UI_CXX 
	  qt_ui/vvPacsConnection.ui
      qt_ui/vvPacsSettingsDialog.ui)
  else()
    QT4_WRAP_CPP(vv_SRCS vvQPacsConnection.h
      vvPacsSettingsDialog.h)
    QT4_WRAP_UI(vv_UI_CXX 
	  qt_ui/vvPacsConnection.ui
      qt_ui/vvPacsSettingsDialog.ui)
  endif()
endif(CLITK_USE_PACS_CONNECTION)
  
  # Add the autotools in the header vvToolsList.h for initialization of the dummy
  # variables in vv.cxx for the tools contained in vvLib
  if(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt IS_NEWER_THAN ${CMAKE_CURRENT_BINARY_DIR}/vvToolsList.h)
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/vvToolsList.h)
    foreach(tool ${vv_TOOLS} ${vv_TOOLS_VTK7})
      file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/vvToolsList.h "#include \"${tool}.h\"\n")
      file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/vvToolsList.h "extern const vvToolCreator<${tool}> *dummy${tool};\n")
      file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/vvToolsList.h "const vvToolCreator<${tool}> *dummy${tool}2 = dummy${tool};\n\n")
    endforeach(tool)
  endif(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt IS_NEWER_THAN ${CMAKE_CURRENT_BINARY_DIR}/vvToolsList.h)
  
  # Add the autotools and the common files ui
  foreach(tool ${vv_TOOLS} ${vv_COMMON_WITH_UI})
      set(vv_SRCS ${vv_SRCS} ${tool}.cxx)
      if(vv_QT_VERSION VERSION_GREATER "4")
        QT5_WRAP_CPP(vv_SRCS ${tool}.h)
        QT5_WRAP_UI(vv_UI_CXX qt_ui/${tool}.ui)
      else()
        QT4_WRAP_CPP(vv_SRCS ${tool}.h)
        QT4_WRAP_UI(vv_UI_CXX qt_ui/${tool}.ui)
      endif()
  endforeach(tool)

  #Special case for Main Window with QVTKWidget according VTK version
  foreach(tool ${vv_TOOLS_VTK7} ${vv_COMMON_WITH_UI_VTK7})
      set(vv_SRCS ${vv_SRCS} ${tool}.cxx)
      if(VTK_VERSION VERSION_LESS "8.2.0")
        if(vv_QT_VERSION VERSION_GREATER "4")
          QT5_WRAP_CPP(vv_SRCS ${tool}.h)
          QT5_WRAP_UI(vv_UI_CXX qt_ui/${tool}VTK7.ui)
        else()
          QT4_WRAP_CPP(vv_SRCS ${tool}.h)
          QT4_WRAP_UI(vv_UI_CXX qt_ui/${tool}VTK7.ui)
        endif()
      else()
        if(vv_QT_VERSION VERSION_GREATER "4")
          QT5_WRAP_CPP(vv_SRCS ${tool}.h)
          QT5_WRAP_UI(vv_UI_CXX qt_ui/${tool}.ui)
        else()
          QT4_WRAP_CPP(vv_SRCS ${tool}.h)
          QT4_WRAP_UI(vv_UI_CXX qt_ui/${tool}.ui)
        endif()
      endif()
  endforeach(tool)
  
  # Add the common source files
  foreach(tool ${vv_COMMON})
      set(vv_SRCS ${vv_SRCS} ${tool}.cxx)
      if(vv_QT_VERSION VERSION_GREATER "4")
        QT5_WRAP_CPP(vv_SRCS ${tool}.h)
      else()
        QT4_WRAP_CPP(vv_SRCS ${tool}.h)
      endif()
  endforeach(tool)
  
  # if Windows and Qt was built as a static lib then don't set QT_DLL flag
  set(QT_STATIC 0)
  if(WIN32)
    if(vv_QT_VERSION VERSION_GREATER "4")
      if(EXISTS ${Qt5Widgets_LIBRARIES}/QtCore.prl)
        file(READ ${Qt5Widgets_LIBRARIES}/QtCore.prl QT_CORE_PRL)
        if(${QT_CORE_PRL} MATCHES "static")
          set(QT_STATIC 1)
        endif(${QT_CORE_PRL} MATCHES "static")
      endif(EXISTS ${Qt5Widgets_LIBRARIES}/QtCore.prl)
    else()
      if(EXISTS ${QT_LIBRARY_DIR}/QtCore.prl)
        file(READ ${QT_LIBRARY_DIR}/QtCore.prl QT_CORE_PRL)
        if(${QT_CORE_PRL} MATCHES "static")
          set(QT_STATIC 1)
        endif(${QT_CORE_PRL} MATCHES "static")
      endif(EXISTS ${QT_LIBRARY_DIR}/QtCore.prl)
    endif()
  endif(WIN32)
  if(NOT QT_STATIC)
    add_definitions(-DQT_DLL)
  endif(NOT QT_STATIC)

  add_definitions(-DQT_THREAD_SUPPORT)
  if(vv_QT_VERSION VERSION_GREATER "4")
    include_directories(
      ${Qt5Widgets_INCLUDE_DIRS}
      ${Qt5Gui_INCLUDE_DIRS}
      ${Qt5Core_INCLUDE_DIRS}
      ${Qt5Network_INCLUDE_DIRS}
      ${Qt5Designer_INCLUDE_DIRS}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
    )
  else()
    include_directories(
      ${QT_INCLUDES}
      ${QT_INCLUDE_DIR}
      ${QT_QTGUI_INCLUDE_DIR}
      ${QT_QTCORE_INCLUDE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
    )
  endif()
#=========================================================

#=========================================================
#support for parallel deformable registration with OpenMP
if(NOT APPLE)
if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif(CMAKE_COMPILER_IS_GNUCC)
endif(NOT APPLE)
#=========================================================

#=========================================================
include_directories(
  ../common
  ../tools
  ../segmentation
  )

#=========================================================
#Add each tool's dependencies
foreach(tool ${vv_TOOLS} ${vv_TOOLS_VTK7})
  set(toolLibs ${toolLibs} ${${tool}_LIBS})
endforeach(tool)

#=========================================================
#Add Foundation Libraries (this should be after our libraries, since we depend
#on them)
if(VTK_VERSION VERSION_GREATER "6.2.0")
set(foundationLibraries clitkCommon ${ITK_LIBRARIES} ${VTK_LIBRARIES})
else()
set(foundationLibraries clitkCommon ${ITK_LIBRARIES} QVTK vtkHybrid)
endif()

#=========================================================
# Use CxImage to create animated gifs
set(vvCxImage clitkCxImage)
set(vv_SRCS ${vv_SRCS} vvAnimatedGIFWriter.cxx)
include_directories(${CLITK_SOURCE_DIR}/utilities/CxImage)
add_subdirectory(${CLITK_SOURCE_DIR}/utilities/CxImage ${PROJECT_BINARY_DIR}/utilities/CxImage)
#=========================================================

#-----------------------------------------------------------------------------
# Avoid linker bug in Mac OS 10.5
# See http://wiki.finkproject.org/index.php/Fink:Packaging:Preparing_for_10.5#OpenGL_Bug
#
if(APPLE)
  find_library(CoreFoundation_LIBRARY CoreFoundation REQUIRED)
  find_library(ApplicationServices_LIBRARY ApplicationServices REQUIRED)
  find_library(SystemConfiguration_LIBRARY SystemConfiguration REQUIRED)
  find_library(Security_LIBRARY Security REQUIRED)
  set(APPLE_FRAMEWORKS ${CoreFoundation_LIBRARY}
    ${ApplicationServices_LIBRARY}
    ${SystemConfiguration_LIBRARY}
    ${Security_LIBRARY}
    )
  message(STATUS "===== ${APPLE_FRAMEWORKS}")
endif()
#=========================================================
#Create binary and libs for tests
if(vv_QT_VERSION VERSION_GREATER "4")
set(vvExternalLibs clitkSegmentationGgoLib ${toolLibs} ${foundationLibraries} ${vvCxImage} Qt5::Widgets Qt5::Network Qt5::Designer ${APPLE_FRAMEWORKS})
else()
set(vvExternalLibs clitkSegmentationGgoLib ${toolLibs} ${foundationLibraries} ${vvCxImage} ${QT_LIBRARIES} ${APPLE_FRAMEWORKS})
endif()
# QtNetwork is required by vvRegisterForm ${QT_QTNETWORK_LIBRARY}

#-----------------------------------------------------------------------------
# For retina displays, see
# http://public.kitware.com/pipermail/vtkusers/2015-February/090117.html
if(APPLE)
  set(vv_SRCS ${vv_SRCS} vvOSXHelper.mm)
endif()

add_library(vvLib ${vv_SRCS} ${vv_UI_CXX})
target_link_libraries(vvLib ${vvExternalLibs})

if(WIN32)
  add_executable(vv WIN32 vv.cxx vvIcon.rc)
elseif(APPLE)
  add_executable(vv MACOSX_BUNDLE vv.cxx)
else(WIN32)
  add_executable(vv vv.cxx)
endif(WIN32)

target_link_libraries(vv vvLib)

#=========================================================
# Install options (also used by CPack)
install(TARGETS vv DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

#=========================================================

#=========================================================
# CPack options
set(CPACK_PACKAGE_NAME "vv")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "vv-src")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vv, the 4D slicer : let's jump into a new dimension !")
set(CPACK_PACKAGE_VENDOR "Creatis-CLB")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "0")
if(NIGHTLY)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "vv-nightly")
else(NIGHTLY)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "vv")
endif(NIGHTLY)
set(CPACK_STRIP_FILES TRUE)

if(WIN32)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\icons\\\\ducky.ico")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "vv.exe")
if(NIGHTLY)
  set(CPACK_NSIS_DISPLAY_NAME "vv - The 4D slicer (nightly)")
else(NIGHTLY)
  set(CPACK_NSIS_DISPLAY_NAME "vv - The 4D slicer")
endif(NIGHTLY)
  set(CPACK_NSIS_HELP_LINK "http://www.creatis.insa-lyon.fr/rio/vv")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://www.creatis.insa-lyon.fr/rio")
  set(CPACK_NSIS_CONTACT "vv@creatis.insa-lyon.fr")
  set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\vv.lnk\\\" \\\"$INSTDIR\\\\bin\\\\vv.exe\\\" ")
  set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\vv.lnk\\\" ")
  #set(CPACK_NSIS_MODIFY_PATH ON) SR: buggy, wait for cmake updates for a fix
  #set(CPACK_BINARY_ZIP ON)
else(WIN32)
  set(CPACK_GENERATOR "STGZ")
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif(WIN32)
include(CPack)
#=========================================================
#=========================================================
configure_file(vvConfiguration.h.in vvConfiguration.h)
#=========================================================
