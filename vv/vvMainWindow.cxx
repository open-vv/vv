/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/

#include <algorithm>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include "QTreePushButton.h"
#include <QUrl>
#include <QSettings>

// VV include
#include "vvMainWindow.h"
#include "vvHelpDialog.h"
#include "vvRegisterForm.h"
#include "vvDocumentation.h"
#include "vvProgressDialog.h"
#include "vvQDicomSeriesSelector.h"
#include "vvSlicerManager.h"
#include "clitkImageCommon.h"
#include "vvSlicer.h"
#include "vvInteractorStyleNavigator.h"
#include "vvImageWriter.h"
#include "vvSegmentationDialog.h"
#include "vvSurfaceViewerDialog.h"
#include "vvDeformationDialog.h"
#include "vvImageWarp.h"
#include "vvUtils.h"
#include "vvMidPosition.h"
#include "vvMesh.h"
#include "vvStructSelector.h"
#include "vvMeshReader.h"
#include "vvSaveState.h"
#include "vvReadState.h"
#include "clitkConfiguration.h"

// ITK include
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkByteSwapper.h>
#include <itkCommand.h>
#include <itkNumericSeriesFileNames.h>

// VTK include
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkCornerAnnotation.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkWindowToImageFilter.h>
#include <vtkBMPWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNMWriter.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkGenericMovieWriter.h>
#ifdef CLITK_EXPERIMENTAL
#  include <vvAnimatedGIFWriter.h>
#endif
#ifdef VTK_USE_VIDEO_FOR_WINDOWS
#  include <vtkAVIWriter.h>
#endif
#ifdef VTK_USE_FFMPEG_ENCODER
#  include <vtkFFMPEGWriter.h>
#endif
#ifdef VTK_USE_MPEG2_ENCODER
#  include <vtkMPEG2Writer.h>
#endif
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>

// Standard includes
#include <iostream>
#include <sstream>
#include <iomanip>

#define COLUMN_TREE 0
#define COLUMN_UL_VIEW 1
#define COLUMN_UR_VIEW 2
#define COLUMN_DL_VIEW 3
#define COLUMN_DR_VIEW 4
#define COLUMN_CLOSE_IMAGE 5
#define COLUMN_RELOAD_IMAGE 6
#define COLUMN_IMAGE_NAME 7

#ifdef CLITK_PRIVATE_FEATURES
#define EXTENSIONS "Images ( *.bmp *.png *.jpeg *.jpg *.tif *.mhd *.mha *.hdr *.vox *.his *.xdr *.SCAN *.nii *.nrrd *.nhdr *.refscan *.nii.gz *.usf)"
#else
#define EXTENSIONS "Images ( *.bmp *.png *.jpeg *.jpg *.tif *.mhd *.mha *.hdr *.vox *.his *.xdr *.SCAN *.nii *.nrrd *.nhdr *.refscan *.nii.gz)"
#endif


/*Data Tree values
  0,Qt::UserRole full filename
  1,Qt::CheckStateRole checkbutton UL View
  1,Qt::UserRole overlay, fusion or vector
  2,Qt::CheckStateRole checkbutton UR View
  3,Qt::CheckStateRole checkbutton DL View
  4,Qt::CheckStateRole checkbutton DR View
  5,0  short filename
  5,Qt::UserRole mSlicerManager id*/

//------------------------------------------------------------------------------
vvMainWindow::vvMainWindow():vvMainWindowBase()
{
  setupUi(this); // this sets up the GUI

  mInputPathName = "";
  mMenuTools = menuTools;
  //  mMenuSegmentation = menuSegmentation;
  mContextMenu = &contextMenu;
  mMenuExperimentalTools = menuExperimental;
  mMainWidget = this;
  mCurrentTime = -1;
  mCurrentSelectedImageId = "";
  mCurrentPickedImageId = "";
  mCurrentPickedImageIndex = 0;

  //Init the contextMenu
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  contextActions.resize(0);
  QAction* actionOpen_new_image = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/fileopen.png")),
                                  tr("O&pen new Image"));
  actionOpen_new_image->setShortcut(QKeySequence(tr("Ctrl+O")));
  connect(actionOpen_new_image,SIGNAL(triggered()),this,SLOT(OpenImages()));
  contextActions.push_back(actionOpen_new_image);
  contextMenu.addSeparator();

  QAction* actionClose_Image = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/exit.png")),
                               tr("Close Current Image"));
  connect(actionClose_Image,SIGNAL(triggered()),this,SLOT(CloseImage()));
  contextActions.push_back(actionClose_Image);

  QAction* actionReload_image = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/rotateright.png")),
                                tr("Reload Current Image"));
  connect(actionReload_image,SIGNAL(triggered()),this,SLOT(ReloadImage()));
  contextActions.push_back(actionReload_image);

  QAction* actionSave_image = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/filesave.png")),
                              tr("Save Current Image"));
  connect(actionSave_image,SIGNAL(triggered()),this,SLOT(SaveAs()));
  contextActions.push_back(actionSave_image);

  QAction* actionSave_state = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/filesave.png")),
                              tr("Save Current State"));
  connect(actionSave_state,SIGNAL(triggered()),this,SLOT(SaveCurrentState()));
  contextActions.push_back(actionSave_state);

  QAction* actionRead_state = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/filesave.png")),
                              tr("Read Saved State"));
  connect(actionRead_state,SIGNAL(triggered()),this,SLOT(ReadSavedState()));
  contextActions.push_back(actionRead_state);

  contextMenu.addSeparator();

  contextMenu.addAction(actionAdd_VF_to_current_Image);
  contextActions.push_back(actionAdd_VF_to_current_Image);

  //QAction* actionAdd_Overlay_to_current_Image = menuOverlay->addAction(QIcon(QString::fromUtf8(":/common/icons/GPSup.png")),
  //    tr("Add overlay image to current image"));
  contextMenu.addAction(actionAdd_overlay_image_to_current_image);
  contextActions.push_back(actionAdd_overlay_image_to_current_image);

  contextMenu.addAction(actionAdd_fusion_image);
  contextActions.push_back(actionAdd_fusion_image);

  contextMenu.addSeparator();
  QAction* actionResetMatrix = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/identity.png")),
                                                      tr("Reset transformation to identity"));
  connect(actionResetMatrix, SIGNAL(triggered()), this,SLOT(ResetTransformationToIdentity()));

  // TRIAL DS
  /*
  QMenu * m = new QMenu(menubar);
  m->setTitle("TOTO");
  //  m->setObjectName(QString::fromUtf8("TOTOTO"));
  contextMenu.addMenu(m);
  QAction * a = m->addAction(QIcon(QString::fromUtf8(":/common/icons/GPSup.png")),
  		     tr("BIDON"));
  QAction * b = m->addAction(QIcon(QString::fromUtf8(":/common/icons/GPSup.png")),
  		     tr("BIDON2"));
  m->addAction(a);
  m->addAction(b);
  connect(a,SIGNAL(triggered()),this,SLOT(AddFusionImage()));
  */

  //init the DataTree
  mSlicerManagers.resize(0);

  QStringList header;
  header.append("");
  header.append("TL");
  header.append("TR");
  header.append("BL");
  header.append("BR");
  header.append("");
  header.append("");
  header.append("Name");

  DataTree->setHeaderLabels(header);
  DataTree->resizeColumnToContents(COLUMN_TREE);
  DataTree->resizeColumnToContents(COLUMN_UL_VIEW);
  DataTree->resizeColumnToContents(COLUMN_UR_VIEW);
  DataTree->resizeColumnToContents(COLUMN_DL_VIEW);
  DataTree->resizeColumnToContents(COLUMN_DR_VIEW);
  DataTree->resizeColumnToContents(COLUMN_CLOSE_IMAGE);
  DataTree->resizeColumnToContents(COLUMN_RELOAD_IMAGE);
  DataTree->resizeColumnToContents(COLUMN_IMAGE_NAME);

  viewMode = 1;
  documentation = new vvDocumentation();
  help_dialog = new vvHelpDialog();
  dicomSeriesSelector = new vvDicomSeriesSelector();

  inverseButton->setEnabled(0);
  actionAdd_overlay_image_to_current_image->setEnabled(0);
  actionSave_As->setEnabled(0);
  actionAdd_VF_to_current_Image->setEnabled(0);
  actionAdd_fusion_image->setEnabled(0);

  //init the sliders
  verticalSliders.push_back(NOVerticalSlider);
  verticalSliders.push_back(NEVerticalSlider);
  verticalSliders.push_back(SOVerticalSlider);
  verticalSliders.push_back(SEVerticalSlider);

  for (int i =0; i < 4; i++)
    verticalSliders[i]->hide();

  horizontalSliders.push_back(NOHorizontalSlider);
  horizontalSliders.push_back(NEHorizontalSlider);
  horizontalSliders.push_back(SOHorizontalSlider);
  horizontalSliders.push_back(SEHorizontalSlider);

  for (int i =0; i < 4; i++)
    horizontalSliders[i]->hide();


  connect(NOVerticalSlider,SIGNAL(valueChanged(int)),this,SLOT(NOVerticalSliderChanged()));
  connect(NEVerticalSlider,SIGNAL(valueChanged(int)),this,SLOT(NEVerticalSliderChanged()));
  connect(SOVerticalSlider,SIGNAL(valueChanged(int)),this,SLOT(SOVerticalSliderChanged()));
  connect(SEVerticalSlider,SIGNAL(valueChanged(int)),this,SLOT(SEVerticalSliderChanged()));

  connect(NOHorizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(NOHorizontalSliderMoved()));
  connect(NEHorizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(NEHorizontalSliderMoved()));
  connect(SOHorizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(SOHorizontalSliderMoved()));
  connect(SEHorizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(SEHorizontalSliderMoved()));

  //connect everything
  connect(actionCompute_mid_position_image,SIGNAL(triggered()),this,SLOT(ComputeMidPosition()));
  connect(actionDeformable_Registration,SIGNAL(triggered()),this,SLOT(ComputeDeformableRegistration()));
  connect(actionWarp_image_with_vector_field,SIGNAL(triggered()),this,SLOT(WarpImage()));
  connect(actionLoad_images,SIGNAL(triggered()),this,SLOT(OpenImages()));
  connect(actionOpen_Dicom,SIGNAL(triggered()),this,SLOT(OpenDicom()));
  connect(actionOpen_Dicom_Struct,SIGNAL(triggered()),this,SLOT(OpenDCStructContour()));
  connect(actionOpen_VTK_contour,SIGNAL(triggered()),this,SLOT(OpenVTKContour()));
  connect(actionOpen_Multiple_Images_As_One,SIGNAL(triggered()),this,SLOT(MergeImages()));
  connect(actionSlice_Image_As_Multiple_Images,SIGNAL(triggered()),this,SLOT(SliceImages()));
  connect(actionOpen_Image_With_Time,SIGNAL(triggered()),this,SLOT(OpenImageWithTime()));
  connect(actionMerge_images_as_n_dim_t, SIGNAL(triggered()), this, SLOT(MergeImagesWithTime()));
  connect(actionSave_As,SIGNAL(triggered()),this,SLOT(SaveAs()));
  connect(actionSave_current_state,SIGNAL(triggered()),this,SLOT(SaveCurrentState()));
  connect(actionRead_saved_state,SIGNAL(triggered()),this,SLOT(ReadSavedState()));
  connect(actionExit,SIGNAL(triggered()),this,SLOT(close()));
  connect(actionAdd_VF_to_current_Image,SIGNAL(triggered()),this,SLOT(OpenField()));
  connect(actionAdd_fusion_image,SIGNAL(triggered()),this,SLOT(SelectFusionImage()));
  connect(actionAdd_overlay_image_to_current_image,SIGNAL(triggered()), this,SLOT(SelectOverlayImage()));  connect(actionNavigation_Help,SIGNAL(triggered()),this,SLOT(ShowHelpDialog()));
  connect(actionDocumentation,SIGNAL(triggered()),this,SLOT(ShowDocumentation()));
  connect(actionRegister_vv,SIGNAL(triggered()),this,SLOT(PopupRegisterForm()));

  ///////////////////////////////////////////////
  connect(actionSegmentation,SIGNAL(triggered()),this,SLOT(SegmentationOnCurrentImage()));
  connect(actionSurface_Viewer,SIGNAL(triggered()),this,SLOT(SurfaceViewerLaunch()));
  ///////////////////////////////////////////////

  actionNorth_East_Window->setEnabled(0);
  actionNorth_West_Window->setEnabled(0);
  actionSouth_East_Window->setEnabled(0);
  actionSouth_West_Window->setEnabled(0);

  connect(actionNorth_East_Window,SIGNAL(triggered()),this,SLOT(SaveNEScreenshot()));
  connect(actionNorth_West_Window,SIGNAL(triggered()),this,SLOT(SaveNOScreenshot()));
  connect(actionSouth_East_Window,SIGNAL(triggered()),this,SLOT(SaveSEScreenshot()));
  connect(actionSouth_West_Window,SIGNAL(triggered()),this,SLOT(SaveSOScreenshot()));
  connect(actionSave_all_slices,SIGNAL(triggered()),this,SLOT(SaveScreenshotAllSlices()));

  connect(DataTree,SIGNAL(itemSelectionChanged()),this,SLOT(ImageInfoChanged()));
  connect(DataTree,SIGNAL(itemClicked(QTreeWidgetItem*, int)),this,
          SLOT(DisplayChanged(QTreeWidgetItem*, int)));

  connect(viewButton,SIGNAL(clicked()),this, SLOT(ChangeViewMode()) );
  connect(windowSpinBox,SIGNAL(editingFinished()),this,SLOT(WindowLevelEdited()));
  connect(levelSpinBox,SIGNAL(editingFinished()),this,SLOT(WindowLevelEdited()));
  connect(colorMapComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateColorMap()));
  connect(presetComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateWindowLevel()));
  connect(slicingPresetComboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateSlicingPreset()));
  connect(inverseButton,SIGNAL(clicked()),this,SLOT(SwitchWindowLevel()));
  connect(applyWindowLevelToAllButton,SIGNAL(clicked()),this,SLOT(ApplyWindowLevelToAllImages()));

  connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowContextMenu(QPoint)));

  connect(linkPanel,SIGNAL(addLink(QString,QString,bool)),this,SLOT(AddLink(QString,QString,bool)));
  connect(linkPanel,SIGNAL(removeLink(QString,QString)),this,SLOT(RemoveLink(QString,QString)));
  connect(overlayPanel,SIGNAL(VFPropertyUpdated(int,int,int,int,double,double,double)),this,SLOT(SetVFProperty(int,int,int,int,double,double,double)));
  connect(overlayPanel,SIGNAL(OverlayPropertyUpdated(int,int,double,double)),
          this,SLOT(SetOverlayProperty(int,int,double,double)));
  connect(overlayPanel,SIGNAL(FusionPropertyUpdated(int,int,int,double,double, bool)),
          this,SLOT(SetFusionProperty(int,int,int,double,double, bool)));
  connect(landmarksPanel,SIGNAL(UpdateRenderWindows()),this,SLOT(UpdateRenderWindows()));

  playMode = 0;//pause
  mFrameRate = 10;
  playButton->setEnabled(0);
  frameRateLabel->setEnabled(0);
  frameRateSpinBox->setEnabled(0);
  connect(playButton, SIGNAL(clicked()),this,SLOT(PlayPause()));
  connect(frameRateSpinBox, SIGNAL(valueChanged(int)),this,SLOT(ChangeFrameRate(int)));

  goToCursorPushButton->setEnabled(0);
  connect(goToCursorPushButton, SIGNAL(clicked()),this,SLOT(GoToCursor()));

  NOViewWidget->hide();
  NEViewWidget->hide();
  SOViewWidget->hide();
  SEViewWidget->hide();

  //Recently opened files
  std::list<std::string> recent_files = GetRecentlyOpenedImages();
  recentlyOpenedFilesMenu=NULL;
  if ( !recent_files.empty() ) {
    createRecentlyOpenedFilesMenu();
    updateRecentlyOpenedFilesMenu(recent_files);
  }

  // Adding all new tools (insertion in the menu)
  vvToolManager::GetInstance()->InsertToolsInMenu(this);
  vvToolManager::GetInstance()->EnableToolsInMenu(this, false);

  if (!CLITK_EXPERIMENTAL)
    menuExperimental->menuAction()->setVisible(false);


  QTimer * timerMemory = new QTimer(this);
  //timerMemory->setInterval(5);
  connect(timerMemory, SIGNAL(timeout()), this, SLOT(UpdateMemoryUsage()));
  timerMemory->start(2000);
}
//------------------------------------------------------------------------------
void vvMainWindow::show()
{
  vvMainWindowBase::show();
  PopupRegisterForm(true);
}
//------------------------------------------------------------------------------
void vvMainWindow::UpdateMemoryUsage()
{
  //  clitk::PrintMemory(true);
  if (clitk::GetMemoryUsageInMb() == 0) infoPanel->setMemoryInMb("NA");
  else infoPanel->setMemoryInMb(QString::number(clitk::GetMemoryUsageInMb())+" MiB");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::createRecentlyOpenedFilesMenu()
{
  recentlyOpenedFilesMenu = new QMenu("Recently opened files...");
  recentlyOpenedFilesMenu->setIcon(QIcon(QString::fromUtf8(":/common/icons/open.png")));
  menuFile->insertMenu(actionOpen_Image_With_Time,recentlyOpenedFilesMenu);
  menuFile->insertSeparator(actionOpen_Image_With_Time);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

void vvMainWindow::updateRecentlyOpenedFilesMenu(const std::list<std::string> &recent_files)
{
  if(recentlyOpenedFilesMenu==NULL) {
    createRecentlyOpenedFilesMenu();
  } else {
    recentlyOpenedFilesMenu->clear();
  }
  for (std::list<std::string>::const_iterator i = recent_files.begin(); i!=recent_files.end(); i++) {
    QAction* current=new QAction(QIcon(QString::fromUtf8(":/common/icons/open.png")), i->c_str(),this);
    recentlyOpenedFilesMenu->addAction(current);
    connect(current,SIGNAL(triggered()),this,SLOT(OpenRecentImage()));
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::ComputeMidPosition()
{
  bool ok;
  int index=GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  int ref = QInputDialog::getInteger(this,"Chose reference phase","Reference phase",0,0,\
                                     mSlicerManagers[index]->GetImage()->GetVTKImages().size()-1,1,&ok);
  if (ok) {
    vvMidPosition midp;
    midp.slicer_manager = mSlicerManagers[index];
    midp.reference_image_index = ref;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    midp.Update();
    if (midp.error)
      QMessageBox::warning(this, "Error computing midposition image",midp.error_message.c_str());
    else {
      QFileInfo info(midp.slicer_manager->GetFileName().c_str());
      AddImage(midp.output,info.path().toStdString()+"/"+info.completeBaseName().toStdString()+"_midposition.mhd");
    }
    QApplication::restoreOverrideCursor();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::AddContour(int image_index, vvMesh::Pointer contour, bool propagation)
{
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setData(0,Qt::UserRole,"filename.vtk");
  item->setData(1,Qt::UserRole,tr("contour"));
  QBrush brush;
  brush.setColor(QColor(contour->r*255,contour->g*255,contour->b*255));
  brush.setStyle(Qt::SolidPattern);
  item->setData(COLUMN_IMAGE_NAME,Qt::BackgroundRole,brush);
  item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,contour->structure_name.c_str());

  for (int j = 1; j <= 4; j++)
    item->setData(j,Qt::CheckStateRole,DataTree->topLevelItem(image_index)->data(j,Qt::CheckStateRole));

  QTreePushButton* cButton = new QTreePushButton;
  cButton->setItem(item);
  cButton->setColumn(COLUMN_CLOSE_IMAGE);
  cButton->setToolTip(tr("close image"));
  cButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/exit.png")));
  connect(cButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
          this,SLOT(CloseImage(QTreeWidgetItem*, int)));

  QTreePushButton* rButton = new QTreePushButton;
  rButton->setItem(item);
  rButton->setColumn(COLUMN_RELOAD_IMAGE);
  rButton->setToolTip(tr("reload image"));
  rButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/rotateright.png")));
  rButton->setEnabled(false);
  //Not implemented
  //connect(rButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
  //this,SLOT(ReloadImage(QTreeWidgetItem*, int)));

  DataTree->topLevelItem(image_index)->setExpanded(1);
  DataTree->topLevelItem(image_index)->addChild(item);
  DataTree->setItemWidget(item, COLUMN_CLOSE_IMAGE, cButton);
  DataTree->setItemWidget(item, COLUMN_RELOAD_IMAGE, rButton);
  QString id = DataTree->topLevelItem(image_index)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString();
  item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());

  UpdateTree();
  mSlicerManagers[image_index]->AddContour(contour,propagation);
  mSlicerManagers[image_index]->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::OpenVTKContour()
{
  if (mSlicerManagers.size() > 0) {
    QString Extensions = "Images ( *.vtk *.obj)";
    Extensions += ";;All Files (*)";
    QString file = QFileDialog::getOpenFileName(this,tr("Open vtkPolyData"),mInputPathName,Extensions);
    if (file.isNull())
      return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
    vvMeshReader reader;
    reader.SetImage(mSlicerManagers[index]->GetImage());
    reader.SetModeToVTK();
    reader.SetFilename(file.toStdString());
    reader.Update();
    AddContour(index,reader.GetOutput()[0],false);
    QApplication::restoreOverrideCursor();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::AddDCStructContour(int index, QString file)
{
    vvMeshReader reader;
    reader.SetFilename(file.toStdString());
    vvStructSelector selector;
    selector.SetStructures(reader.GetROINames());
    if (!mSlicerManagers[index]->GetVF().IsNull())
      selector.EnablePropagationCheckBox();
    if (selector.exec()) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      reader.SetSelectedItems(selector.getSelectedItems());
      reader.SetImage(mSlicerManagers[index]->GetImage());
      if (selector.PropagationEnabled())
        reader.SetPropagationVF(mSlicerManagers[index]->GetVF());
      reader.Update();
      std::vector<vvMesh::Pointer> contours=reader.GetOutput();
      for (std::vector<vvMesh::Pointer>::iterator i=contours.begin();
           i!=contours.end(); i++)
        AddContour(index,*i,selector.PropagationEnabled());
      QApplication::restoreOverrideCursor();
    }
}

//------------------------------------------------------------------------------
void vvMainWindow::OpenDCStructContour()
{
  if (mSlicerManagers.size() > 0) {
    QString Extensions = "Dicom Files ( *.dcm RS*)";
    Extensions += ";;All Files (*)";
    QString file = QFileDialog::getOpenFileName(this,tr("Merge Images"),mInputPathName,Extensions);
    if (file.isNull())
      return;
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
    AddDCStructContour(index, file);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::ComputeDeformableRegistration()
{
  if (mSlicerManagers.size() > 0) {
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
    vvDeformationDialog dialog(index,mSlicerManagers);
    if (dialog.exec()) {
      std::string base_name=itksys::SystemTools::GetFilenameWithoutExtension(mSlicerManagers[dialog.GetInputFileIndex()]->GetFileName());
      AddField(dialog.GetOutput(),dialog.getFieldFile(),dialog.GetInputFileIndex());
      WarpImage(dialog.GetSelectedSlicer(),dialog.GetReferenceFrameIndex());
    } else
      std::cout << "Error or user cancellation while computing deformation field..." << std::endl;
  } else QMessageBox::information(this, "Need to open image","You must open an image first.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::WarpImage()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if (!mSlicerManagers[index]->GetVF().IsNull()) {
    bool ok;
    int ref = QInputDialog::getInteger(this,"Chose reference phase","Reference phase",0,0,\
                                       mSlicerManagers[index]->GetImage()->GetVTKImages().size()-1,1,&ok);
    if (ok) {
      WarpImage(mSlicerManagers[index],ref);
    }
  } else
    QMessageBox::warning(this,tr("No vector field"),tr("Sorry, can't warp without a vector field"));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::WarpImage(vvSlicerManager* selected_slicer,int reference_phase)
{
  if (!selected_slicer->GetVF().IsNull()) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFileInfo info(selected_slicer->GetFileName().c_str());
    vvImageWarp warp(selected_slicer->GetImage(),selected_slicer->GetVF(),
                     reference_phase,this);
    if (warp.ComputeWarpedImage()) {
      AddImage(warp.GetWarpedImage(),info.path().toStdString()+"/"+info.completeBaseName().toStdString()+"_warped.mhd");
      AddImage(warp.GetDiffImage()  ,info.path().toStdString()+"/"+info.completeBaseName().toStdString()+"_diff.mhd");
      AddImage(warp.GetJacobianImage()  ,info.path().toStdString()+"/"+info.completeBaseName().toStdString()+"_jacobian.mhd");
      QApplication::restoreOverrideCursor();
    } else {
      QApplication::restoreOverrideCursor();
      QMessageBox::warning(this,tr("Different spacings"),tr("The vector field and image spacings must be the same in order to warp."));
    }
  } else
    QMessageBox::warning(this,tr("No vector field"),tr("Sorry, can't warp without a vector field."));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvMainWindow::~vvMainWindow()
{
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (mSlicerManagers[i] != NULL)
      delete mSlicerManagers[i];
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QTabWidget * vvMainWindow::GetTab()
{
  return tabWidget;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::MergeImages()
{
  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";
  QStringList files = QFileDialog::getOpenFileNames(this,tr("Merge Images"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;
  mInputPathName = itksys::SystemTools::GetFilenamePath(files[0].toStdString()).c_str();
  std::vector<std::string> vector;

  unsigned int currentDim = 0;
  std::vector<double> currentSpacing;
  std::vector<int> currentSize;
  std::vector<double> currentOrigin;

  for (int i = 0; i < files.size(); i++) {
    itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(
                                         files[i].toStdString().c_str(), itk::ImageIOFactory::ReadMode);
    reader->SetFileName(files[i].toStdString().c_str());
    reader->ReadImageInformation();
    if (reader)        {
      //NOViewWidget->hide();
      //NEViewWidget->hide();
      //SOViewWidget->hide();
      //SEViewWidget->hide();
      if (i == 0)
        currentDim = reader->GetNumberOfDimensions();
      bool IsOk = true;
      for (unsigned int j = 0; j < currentDim; j++) {
        if (i == 0) {
          if (j == 0) {
            currentSpacing.resize(currentDim);
            currentSize.resize(currentDim);
            currentOrigin.resize(currentDim);
          }
          currentOrigin[j] = reader->GetOrigin(j);
          currentSpacing[j] = reader->GetSpacing(j);
          currentSize[j] = reader->GetDimensions(j);
        } else if (currentDim != reader->GetNumberOfDimensions()
                   || currentSpacing[j] != reader->GetSpacing(j)
                   || currentSize[j] != (int)reader->GetDimensions(j)
                   || currentOrigin[j] != reader->GetOrigin(j)) {
          QString error = "Cannot read file (too different from others ";
          error += files[i].toStdString().c_str();
          QMessageBox::information(this,tr("Reading problem"),error);
          IsOk = false;
          break;
        }
      }
      if (IsOk)
        vector.push_back(files[i].toStdString());
    }
  }
  if (vector.size() > 0)
    LoadImages(vector, vvImageReader::MERGED);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SliceImages()
{
  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";

  QStringList files = QFileDialog::getOpenFileNames(this,tr("Slice Images"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;
  mInputPathName = itksys::SystemTools::GetFilenamePath(files[0].toStdString()).c_str();
  std::vector<std::string> vector;
  for (int i = 0; i < files.size(); i++)
    vector.push_back(files[i].toStdString());
  LoadImages(vector, vvImageReader::SLICED);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::MergeImagesWithTime()
{
  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";
  QStringList files = QFileDialog::getOpenFileNames(this,tr("Merge Images With Time"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;
  mInputPathName = itksys::SystemTools::GetFilenamePath(files[0].toStdString()).c_str();
  std::vector<std::string> vector;

  for (int i = 0; i < files.size(); i++)
    vector.push_back(files[i].toStdString());
  sort(vector.begin(),vector.end());
  if (vector.size() > 1)
    LoadImages(vector, vvImageReader::MERGEDWITHTIME);
  else
    QMessageBox::warning(this,tr("Reading problem"),"You need to select at least two images to merge images with time.\nIf you only want to open one image, please use the \"Open Image\" function.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::OpenDicom()
{
  std::vector<std::string> files;

  std::cout << "dicomSeriesSelector " << std::endl;
  if (dicomSeriesSelector->exec() == QDialog::Accepted) {
    files = *(dicomSeriesSelector->GetFilenames());
    LoadImages(files, vvImageReader::DICOM);
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::OpenImages()
{
  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";

  QStringList files = QFileDialog::getOpenFileNames(this,tr("Load Images"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;
  mInputPathName = itksys::SystemTools::GetFilenamePath(files[0].toStdString()).c_str();
  std::vector<std::string> vector;
  for (int i = 0; i < files.size(); i++)
    vector.push_back(files[i].toStdString());
  LoadImages(vector, vvImageReader::IMAGE);
}
//------------------------------------------------------------------------------
void vvMainWindow::OpenRecentImage()
{
  QAction * caller = qobject_cast<QAction*>(sender());
  std::vector<std::string> images;
  images.push_back(caller->text().toStdString());
  mInputPathName = itksys::SystemTools::GetFilenamePath(images[0]).c_str();
  LoadImages(images, vvImageReader::IMAGE);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::OpenImageWithTime()
{
  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";

  QStringList files = QFileDialog::getOpenFileNames(this,tr("Load Images With Time"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;
  mInputPathName = itksys::SystemTools::GetFilenamePath(files[0].toStdString()).c_str();
  std::vector<std::string> vector;
  for (int i = 0; i < files.size(); i++) {
    vector.push_back(files[i].toStdString());
  }
  LoadImages(vector, vvImageReader::IMAGEWITHTIME);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::LoadImages(std::vector<std::string> files, vvImageReader::LoadedImageType filetype)
{
  //Separate the way to open images and dicoms
  int fileSize;
  if (filetype == vvImageReader::IMAGE || filetype == vvImageReader::IMAGEWITHTIME)
    fileSize = files.size();
  else
    fileSize = 1;

  // For SLICED, we need the number of slices (ndim and #slices)
  std::vector<unsigned int> nSlices;
  nSlices.resize(files.size());
  std::fill(nSlices.begin(), nSlices.end(), 1);
  if (filetype == vvImageReader::SLICED) {
    for (int i = 0; i < fileSize; i++) {
      itk::ImageIOBase::Pointer header = clitk::readImageHeader(files[i]);
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      if (!header) {
        nSlices[i] = 0;
        QString error = "Cannot open file \n";
        error += files[i].c_str();
        QMessageBox::information(this,tr("Reading problem"),error);
        return;
      }
      if (header->GetNumberOfDimensions() < 3) {
        nSlices[i] = 0;
        QString error = "Dimension problem. Cannot slice \n";
        error += files[i].c_str();
        QMessageBox::information(this,tr("Reading problem"),error);
        return;
      }
      nSlices[i] = header->GetDimensions( header->GetNumberOfDimensions()-1 );
    }
  }

  //Only add to the list of recently opened files when a single file is opened,
  //to avoid polluting the list of recently opened files
  if (files.size() == 1) {
    QFileInfo finfo=tr(files[0].c_str());
    AddToRecentlyOpenedImages(finfo.absoluteFilePath().toStdString());
    updateRecentlyOpenedFilesMenu(GetRecentlyOpenedImages());
  }
  //init the progress events
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vvProgressDialog progress("Opening " + files[0],fileSize>1);
  qApp->processEvents();

  int numberofsuccesulreads=0;
  //open images as 1 or multiples
  for (int i = 0; i < fileSize; i++) {

    progress.SetText("Opening " + files[i]);
    progress.SetProgress(i,fileSize);
    qApp->processEvents();

    for (unsigned int j = 0; j < nSlices[i]; j++) {
      //read the image and put it in mSlicerManagers
      vvSlicerManager* imageManager = new vvSlicerManager(4);
      qApp->processEvents();

      bool SetImageSucceed=false;

      // Change filename if an image with the same already exist
      int number = GetImageDuplicateFilenameNumber(files[i] + std::string("_slice"));

      if (filetype == vvImageReader::IMAGE || filetype == vvImageReader::IMAGEWITHTIME || filetype == vvImageReader::SLICED)
        SetImageSucceed = imageManager->SetImage(files[i],filetype, number, j);
      else {
        SetImageSucceed = imageManager->SetImages(files,filetype, number);
      }
      if (!SetImageSucceed) {
        QApplication::restoreOverrideCursor();
        QString error = "Cannot open file \n";
        error += imageManager->GetLastError().c_str();
        QMessageBox::information(this,tr("Reading problem"),error);
        delete imageManager;
      } else {
        mSlicerManagers.push_back(imageManager);

        //create an item in the tree with good settings
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setData(0,Qt::UserRole,files[i].c_str());
        QFileInfo fileinfo(imageManager->GetFileName().c_str()); //Do not show the path
        item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,fileinfo.fileName());
        item->setData(1,Qt::UserRole,tr("image"));
        item->setToolTip(COLUMN_IMAGE_NAME, imageManager->GetListOfAbsoluteFilePathInOneString("image").c_str());
        qApp->processEvents();

        //Create the buttons for reload and close
        qApp->processEvents();
        QTreePushButton* cButton = new QTreePushButton;
        cButton->setItem(item);
        cButton->setColumn(COLUMN_CLOSE_IMAGE);
        cButton->setToolTip(tr("close image"));
        cButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/exit.png")));
        connect(cButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
                this,SLOT(CloseImage(QTreeWidgetItem*, int)));

        QTreePushButton* rButton = new QTreePushButton;
        rButton->setItem(item);
        rButton->setColumn(COLUMN_RELOAD_IMAGE);
        rButton->setToolTip(tr("reload image"));
        rButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/rotateright.png")));
        connect(rButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
                this,SLOT(ReloadImage(QTreeWidgetItem*, int)));

        DataTree->addTopLevelItem(item);
        DataTree->setItemWidget(item, COLUMN_CLOSE_IMAGE, cButton);
        DataTree->setItemWidget(item, COLUMN_RELOAD_IMAGE, rButton);

        //set the id of the image
        QString id = QDir::current().absoluteFilePath(files[i].c_str()) + QString::number(mSlicerManagers.size()-1);
        item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
        mSlicerManagers.back()->SetId(id.toStdString());

        linkPanel->addImage(imageManager->GetFileName(), id.toStdString());

        connect(mSlicerManagers.back(), SIGNAL(currentImageChanged(std::string)),
                this,SLOT(CurrentImageChanged(std::string)));
        connect(mSlicerManagers.back(), SIGNAL(currentPickedImageChanged(std::string)),
                this, SLOT(CurrentPickedImageChanged(std::string)));
        connect(mSlicerManagers.back(), SIGNAL(UpdatePosition(int, double, double, double, double, double, double, double)),
                this,SLOT(MousePositionChanged(int,double, double, double, double, double, double, double)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateVector(int, double, double, double, double)),
                this, SLOT(VectorChanged(int,double,double,double, double)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateOverlay(int, double, double)),
                this, SLOT(OverlayChanged(int,double,double)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateFusion(int, double)),
                this, SLOT(FusionChanged(int,double)));
        connect(mSlicerManagers.back(), SIGNAL(WindowLevelChanged()),
                this,SLOT(WindowLevelChanged()));
        connect(mSlicerManagers.back(), SIGNAL(UpdateSlice(int,int)),
                this,SLOT(UpdateSlice(int,int)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateTSlice(int, int)),
                this,SLOT(UpdateTSlice(int, int)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateTSlice(int, int)),
                this,SLOT(ImageInfoChanged()));
        connect(mSlicerManagers.back(), SIGNAL(UpdateSliceRange(int,int,int,int,int)),
                this,SLOT(UpdateSliceRange(int,int,int,int,int)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateLinkManager(std::string,int,double,double,double,int)),
                this,SLOT(UpdateLinkManager(std::string,int,double,double,double,int)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateLinkedNavigation(std::string,vvSlicerManager*,vvSlicer*)),
                this,SLOT(UpdateLinkedNavigation(std::string,vvSlicerManager*,vvSlicer*)));
        connect(mSlicerManagers.back(), SIGNAL(ChangeImageWithIndexOffset(vvSlicerManager*,int,int)),
                this,SLOT(ChangeImageWithIndexOffset(vvSlicerManager*,int,int)));
        connect(mSlicerManagers.back(),SIGNAL(LandmarkAdded()),landmarksPanel,SLOT(AddPoint()));
        InitSlicers();
        numberofsuccesulreads++;
      }
    }
  }
  if (numberofsuccesulreads) {
    NOViewWidget->show();
    NEViewWidget->show();
    SOViewWidget->show();
    SEViewWidget->show();
    UpdateTree();
    InitDisplay();
    ShowLastImage();

    // Try to guess default WindowLevel
    double range[2];
    mSlicerManagers.back()->GetImage()->GetFirstVTKImageData()->GetScalarRange(range);
    if ((range[0] == 0) && (range[1] == 1)) {
      presetComboBox->setCurrentIndex(5);// binary
    } else {
      // TODO
    }
  }
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateTree()
{
  DataTree->resizeColumnToContents(COLUMN_TREE);
  DataTree->resizeColumnToContents(COLUMN_UL_VIEW);
  DataTree->resizeColumnToContents(COLUMN_UR_VIEW);
  DataTree->resizeColumnToContents(COLUMN_DL_VIEW);
  DataTree->resizeColumnToContents(COLUMN_DR_VIEW);
  DataTree->resizeColumnToContents(COLUMN_IMAGE_NAME);
  DataTree->resizeColumnToContents(COLUMN_CLOSE_IMAGE);
  DataTree->resizeColumnToContents(COLUMN_RELOAD_IMAGE);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::CurrentImageChanged(std::string id)
{
  if (id == mCurrentSelectedImageId) return; // Do nothing
  int selected = 0;
  for (int i = 0; i < DataTree->topLevelItemCount(); i++) {
    if (DataTree->topLevelItem(i)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString().toStdString() == id) {
      selected = i;
    } else {
      DataTree->topLevelItem(i)->setSelected(0);
    }
    for (int child = 0; child < DataTree->topLevelItem(i)->childCount(); child++)
      DataTree->topLevelItem(i)->child(child)->setSelected(0);

  }
  DataTree->topLevelItem(selected)->setSelected(1);
  mCurrentSelectedImageId = id;
  emit SelectedImageHasChanged(mSlicerManagers[selected]);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::CurrentPickedImageChanged(std::string id)
{
  if (id == mCurrentPickedImageId) return; // Do nothing
  int selected = 0;
  for (int i = 0; i < DataTree->topLevelItemCount(); i++) {
    if (DataTree->topLevelItem(i)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString().toStdString() == id) {
      selected = i;
    } else {
      DataTree->topLevelItem(i)->setSelected(0);
    }
    for (int child = 0; child < DataTree->topLevelItem(i)->childCount(); child++)
      DataTree->topLevelItem(i)->child(child)->setSelected(0);

  }
  DataTree->topLevelItem(selected)->setSelected(1);
  mCurrentPickedImageId = id;
  mCurrentPickedImageIndex = selected;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ImageInfoChanged()
{
  contextActions[6]->setEnabled(1);
  contextActions[5]->setEnabled(1);
  actionSave_As->setEnabled(1);
  actionAdd_VF_to_current_Image->setEnabled(1);
  actionAdd_fusion_image->setEnabled(1);
  actionAdd_overlay_image_to_current_image->setEnabled(1);
  actionNorth_East_Window->setEnabled(1);
  actionNorth_West_Window->setEnabled(1);
  actionSouth_East_Window->setEnabled(1);
  actionSouth_West_Window->setEnabled(1);
  vvToolManager::GetInstance()->EnableToolsInMenu(this, true);
  inverseButton->setEnabled(1);

  goToCursorPushButton->setEnabled(1);

  if (DataTree->selectedItems().size()) {
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);

    colorMapComboBox->setEnabled(1);
    for (int i = 0; i < DataTree->topLevelItem(index)->childCount(); i++) {
      if (DataTree->topLevelItem(index)->child(i)->data(1,Qt::UserRole).toString() == "overlay" ||
          DataTree->topLevelItem(index)->child(i)->data(1,Qt::UserRole).toString() == "fusion") {
        colorMapComboBox->setEnabled(0);
        break;
      }
    }

    std::vector<double> origin;
    std::vector<double> inputSpacing;
    std::vector<int> inputSize;
    std::vector<double> sizeMM;
    vtkSmartPointer<vtkMatrix4x4> transformation;
    int dimension=0;
    QString pixelType;
    QString inputSizeInBytes;
    QString image = DataTree->selectedItems()[0]->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();

    int nframes = mSlicerManagers[index]->GetSlicer(0)->GetTMax();
    if (nframes > 1 || playMode == 1) {
      playButton->setEnabled(1);
      frameRateLabel->setEnabled(1);
      frameRateSpinBox->setEnabled(1);
    } else {
      playButton->setEnabled(0);
      frameRateLabel->setEnabled(0);
      frameRateSpinBox->setEnabled(0);
    }

    //read image header
    int NPixel = 1;

    int tSlice = 0;
    vvImage::Pointer imageSelected;
    if (DataTree->topLevelItem(index) == DataTree->selectedItems()[0]) {
      imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetImage();
      tSlice = mSlicerManagers[index]->GetSlicer(0)->GetTSlice();
    } else if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "vector") {
      imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetVF();
      tSlice = mSlicerManagers[index]->GetSlicer(0)->GetOverlayTSlice();
    } else if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "overlay") {
      imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetOverlay();
      tSlice = mSlicerManagers[index]->GetSlicer(0)->GetOverlayTSlice();
    } else if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "fusion") {
      imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetFusion();
      tSlice = mSlicerManagers[index]->GetSlicer(0)->GetFusionTSlice();
    }
    else if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "contour") {
      imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetImage();
      tSlice = mSlicerManagers[index]->GetSlicer(0)->GetTSlice();
    }
    else {
      imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetImage();
      tSlice = mSlicerManagers[index]->GetSlicer(0)->GetTSlice();
    }

    dimension = imageSelected->GetNumberOfDimensions();
    origin.resize(dimension);
    inputSpacing.resize(dimension);
    inputSize.resize(dimension);
    sizeMM.resize(dimension);
    pixelType = mSlicerManagers[index]->GetImage()->GetScalarTypeAsITKString().c_str();
    for (int i = 0; i < dimension; i++) {
      origin[i] = imageSelected->GetOrigin()[i];
      inputSpacing[i] = imageSelected->GetSpacing()[i];
      inputSize[i] = imageSelected->GetSize()[i];
      sizeMM[i] = inputSize[i]*inputSpacing[i];
      NPixel *= inputSize[i];
    }
    inputSizeInBytes = GetSizeInBytes(imageSelected->GetActualMemorySize()*1000);
    
    QString dim = QString::number(dimension) + " (";
    dim += pixelType + ")";

    infoPanel->setFileName(image);
    infoPanel->setDimension(dim);
    infoPanel->setSizePixel(GetVectorIntAsString(inputSize));
    infoPanel->setSizeMM(GetVectorDoubleAsString(sizeMM));
    infoPanel->setOrigin(GetVectorDoubleAsString(origin));
    infoPanel->setSpacing(GetVectorDoubleAsString(inputSpacing));
    infoPanel->setNPixel(QString::number(NPixel)+" ("+inputSizeInBytes+")");
    transformation = imageSelected->GetTransform()[tSlice]->GetMatrix();
    infoPanel->setTransformation(Get4x4MatrixDoubleAsString(transformation));

    landmarksPanel->SetCurrentLandmarks(mSlicerManagers[index]->GetLandmarks(),
                                        mSlicerManagers[index]->GetSlicer(0)->GetImage()->GetVTKImages().size());
    landmarksPanel->SetCurrentPath(mInputPathName.toStdString());
    landmarksPanel->SetCurrentImage(mSlicerManagers[index]->GetFileName().c_str());

    overlayPanel->getCurrentImageName(mSlicerManagers[index]->GetFileName().c_str());
    for (int i = 0; i < 4; i++) {
      if (DataTree->selectedItems()[0]->data(i+1,Qt::CheckStateRole).toInt() > 0 || i == 3) {
        mSlicerManagers[index]->UpdateInfoOnCursorPosition(i);
        break;
      }
    }

    infoPanel->setFileName(image);
    infoPanel->setDimension(dim);
    infoPanel->setSizePixel(GetVectorIntAsString(inputSize));
    infoPanel->setSizeMM(GetVectorDoubleAsString(sizeMM));
    infoPanel->setOrigin(GetVectorDoubleAsString(origin));
    infoPanel->setSpacing(GetVectorDoubleAsString(inputSpacing));
    infoPanel->setNPixel(QString::number(NPixel)+" ("+inputSizeInBytes+")");

    landmarksPanel->SetCurrentLandmarks(mSlicerManagers[index]->GetLandmarks(),
                                        mSlicerManagers[index]->GetSlicer(0)->GetImage()->GetVTKImages().size());
    landmarksPanel->SetCurrentPath(mInputPathName.toStdString());
    landmarksPanel->SetCurrentImage(mSlicerManagers[index]->GetFileName().c_str());

    overlayPanel->getCurrentImageName(mSlicerManagers[index]->GetFileName().c_str());
    for (int i = 0; i < 4; i++) {
      if (DataTree->selectedItems()[0]->data(i+1,Qt::CheckStateRole).toInt() > 0 || i == 3) {
        mSlicerManagers[index]->UpdateInfoOnCursorPosition(i);
        break;
      }
    }
    WindowLevelChanged();
    slicingPresetComboBox->setCurrentIndex(mSlicerManagers[index]->GetSlicingPreset());

    if (mSlicerManagers[index]->GetSlicer(0)->GetVF()) {
      overlayPanel->getVFName(mSlicerManagers[index]->GetVFName().c_str());
      overlayPanel->getVFProperty(mSlicerManagers[index]->GetSlicer(0)->GetVFSubSampling(),
                                  mSlicerManagers[index]->GetSlicer(0)->GetVFScale(),
                                  mSlicerManagers[index]->GetSlicer(0)->GetVFLog());
    } else {
      overlayPanel->getVFName(mSlicerManagers[index]->GetVFName().c_str());
      overlayPanel->getVFProperty(-1,-1,-1);
    }
    if (mSlicerManagers[index]->GetSlicer(0)->GetOverlay()) {
      overlayPanel->getOverlayName(mSlicerManagers[index]->GetOverlayName().c_str());
    } else {
      overlayPanel->getOverlayName(mSlicerManagers[index]->GetOverlayName().c_str());
    }
    
    if (mSlicerManagers[index]->GetSlicer(0)->GetFusion()) {
      overlayPanel->getFusionName(mSlicerManagers[index]->GetFusionName().c_str());
    } else {
      overlayPanel->getFusionName(mSlicerManagers[index]->GetFusionName().c_str());
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ShowDocumentation()
{
  documentation->show();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::PopupRegisterForm(bool checkCanPush)
{
  vvRegisterForm* registerForm = new vvRegisterForm(QUrl("http://www.creatis.insa-lyon.fr/~dsarrut/vvregister/write.php"), getVVSettingsPath(), getSettingsOptionFormat());
  if(!checkCanPush) {
    registerForm->show();
  } else {
    if(registerForm->canPush()) {
      registerForm->show();
      registerForm->acquitPushed();//too bad if there is not internet connection anymore.
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ShowHelpDialog()
{
  help_dialog->show();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ChangeViewMode()
{
  typedef struct _SIZE{
    QSplitter* splitter;
    QList<int> size1, size2;
    int cols[3];
  }SplitterSize;
  SplitterSize sizes[4];
  sizes[0].splitter = OSplitter;
  sizes[0].size1.push_back(1);
  sizes[0].size1.push_back(0);
  sizes[0].size2.push_back(1);
  sizes[0].size2.push_back(0);
  sizes[0].cols[0] = 2;
  sizes[0].cols[1] = 3;
  sizes[0].cols[2] = 4;

  sizes[1].splitter = ESplitter;
  sizes[1].size1.push_back(0);
  sizes[1].size1.push_back(1);
  sizes[1].size2.push_back(1);
  sizes[1].size2.push_back(0);
  sizes[1].cols[0] = 1;
  sizes[1].cols[1] = 3;
  sizes[1].cols[2] = 4;

  sizes[2].splitter = OSplitter;
  sizes[2].size1.push_back(1);
  sizes[2].size1.push_back(0);
  sizes[2].size2.push_back(0);
  sizes[2].size2.push_back(1);
  sizes[2].cols[0] = 1;
  sizes[2].cols[1] = 2;
  sizes[2].cols[2] = 4;

  sizes[3].splitter = ESplitter;
  sizes[3].size1.push_back(0);
  sizes[3].size1.push_back(1);
  sizes[3].size2.push_back(0);
  sizes[3].size2.push_back(1);
  sizes[3].cols[0] = 1;
  sizes[3].cols[1] = 2;
  sizes[3].cols[2] = 3;
  
  int slicer = mSlicerManagers[mCurrentPickedImageIndex]->GetSelectedSlicer();
  if (viewMode == 1) {
    if (slicer >= 0) {
      viewMode = 0;
      splitter_3->setSizes(sizes[slicer].size1);
      sizes[slicer].splitter->setSizes(sizes[slicer].size2);
      DataTree->setColumnHidden(sizes[slicer].cols[0],1);
      DataTree->setColumnHidden(sizes[slicer].cols[1],1);
      DataTree->setColumnHidden(sizes[slicer].cols[2],1);
    }
  } else {
    QList<int> size;
    if (slicer >= 0) {
      viewMode = 1;
      size.push_back(1);
      size.push_back(1);
      splitter_3->setSizes(size);
      sizes[slicer].splitter->setSizes(size);
      DataTree->setColumnHidden(sizes[slicer].cols[0],0);
      DataTree->setColumnHidden(sizes[slicer].cols[1],0);
      DataTree->setColumnHidden(sizes[slicer].cols[2],0);
    }
  }
  UpdateRenderWindows();
  /*
  ** I don't know why but for both resized QVTKWidget we also need to render
  ** the associated Slicer to redraw crosses.
  */
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
//     if (DataTree->topLevelItem(i)->data(COLUMN_UL_VIEW,Qt::CheckStateRole).toInt() > 1)
      mSlicerManagers[i]->GetSlicer(0)->Render();
      mSlicerManagers[i]->GetSlicer(1)->Render();
//     if (DataTree->topLevelItem(i)->data(COLUMN_DL_VIEW,Qt::CheckStateRole).toInt() > 1)
      mSlicerManagers[i]->GetSlicer(2)->Render();
      mSlicerManagers[i]->GetSlicer(3)->Render();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString vvMainWindow::GetSizeInBytes(unsigned long size)
{
  QString result = "";// QString::number(size);
  //result += " bytes (";
  if (size > 1000000000) {
    size /= 1000000000;
    result += QString::number(size);
    result += "Gb";//)";
  } else if (size > 1000000) {
    size /= 1000000;
    result += QString::number(size);
    result += "Mb";//)";
  } else if (size > 1000) {
    size /= 1000;
    result += QString::number(size);
    result += "kb";//)";
  }
  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString vvMainWindow::Get4x4MatrixDoubleAsString(vtkSmartPointer<vtkMatrix4x4> matrix, const int precision)
{
  std::ostringstream strmatrix;

  // Figure out the number of digits of the integer part of the largest absolute value
  // for each column
  unsigned width[4];
  for (unsigned int j = 0; j < 4; j++){
    double absmax = 0.;
    for (unsigned int i = 0; i < 4; i++)
      absmax = std::max(absmax, vnl_math_abs(matrix->GetElement(i, j)));
    unsigned ndigits = (unsigned)std::max(0.,std::log10(absmax))+1;
    width[j] = precision+ndigits+3;
  }

  // Output with correct width, aligned to the right
  for (unsigned int i = 0; i < 4; i++) {
    for (unsigned int j = 0; j < 4; j++) {
      strmatrix.setf(ios::fixed,ios::floatfield);
      strmatrix.precision(precision);
      strmatrix.fill(' ');
      strmatrix.width(width[j]);
      strmatrix << std::right << matrix->GetElement(i, j);
    }
    strmatrix << std::endl;
  }
  QString result = strmatrix.str().c_str();
  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString vvMainWindow::GetVectorDoubleAsString(std::vector<double> vectorDouble)
{
  QString result;
  for (unsigned int i= 0; i < vectorDouble.size(); i++) {
    if (i != 0)
      result += " ";
    result += QString::number(vectorDouble[i]);
  }
  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString vvMainWindow::GetVectorIntAsString(std::vector<int> vectorInt)
{
  QString result;
  for (unsigned int i= 0; i < vectorInt.size(); i++) {
    if (i != 0)
      result += " ";
    result += QString::number(vectorInt[i]);
  }
  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int vvMainWindow::GetSlicerIndexFromItem(QTreeWidgetItem* item)
{
  QString id = item->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString();
  for (int i = 0; i < DataTree->topLevelItemCount(); i++) {
    if (DataTree->topLevelItem(i)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString() == id)
      return i;
  }
  return -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QTreeWidgetItem* vvMainWindow::GetItemFromSlicerManager(vvSlicerManager* sm)
{
  QString id = sm->GetId().c_str();
  for (int i = 0; i < DataTree->topLevelItemCount(); i++) {
    if (DataTree->topLevelItem(i)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString() == id)
      return DataTree->topLevelItem(i);
  }
  return NULL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::DisplayChanged(QTreeWidgetItem *clickedItem, int column)
{
  if ( column >= COLUMN_CLOSE_IMAGE || column <= 0)
    return;

  // Get parent information (might be the same item)
  int slicerManagerIndex = GetSlicerIndexFromItem(clickedItem);
  QTreeWidgetItem* clickedParentItem = DataTree->topLevelItem(slicerManagerIndex);
  vvSlicer* clickedSlicer = mSlicerManagers[slicerManagerIndex]->GetSlicer(column-1);

  // Go over the complete item tree (only 2 levels, parents and children)
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    // Trick to avoid redoing twice the job for a key (sr)
    mSlicerManagers[i]->GetSlicer(column-1)->GetRenderWindow()->GetInteractor()->SetKeySym("Crap");

    QTreeWidgetItem* currentParentItem = DataTree->topLevelItem(i);
    if(currentParentItem != clickedParentItem) {
      // Not the branch of the clicked item, uncheck all

      // Parent
      currentParentItem->setData(column,Qt::CheckStateRole, 0);
      mSlicerManagers[i]->UpdateSlicer(column-1, false);

      // Children
      for (int iChild = 0; iChild < currentParentItem->childCount(); iChild++) {
        currentParentItem->child(iChild)->setData(column,Qt::CheckStateRole, 0);
      }
    }
    else {
      // Branch of the clicked one: get check status from actor visibility in slicer
      // and toggle the clicked one

      // Parent
      bool vis = clickedSlicer->GetActorVisibility("image", 0);
      bool draw = clickedSlicer->GetRenderer()->GetDraw();

      // Update slicer (after getting visibility)
      mSlicerManagers[slicerManagerIndex]->UpdateSlicer(column-1, true);
      mSlicerManagers[slicerManagerIndex]->UpdateInfoOnCursorPosition(column-1);
      DisplaySliders(slicerManagerIndex, column-1);
      if(clickedParentItem == clickedItem) {
        // Toggle
        vis = !draw || !vis;
      }
      clickedSlicer->SetActorVisibility("image", 0, vis);
      clickedParentItem->setData(column, Qt::CheckStateRole, vis?2:0);

      // Children
      std::map<std::string, int> actorTypeCounts;      
      for (int iChild = 0; iChild < clickedParentItem->childCount(); iChild++) {
        QTreeWidgetItem* currentChildItem = clickedParentItem->child(iChild);
        std::string actorType = currentChildItem->data(1,Qt::UserRole).toString().toStdString();
        vis = clickedSlicer->GetActorVisibility(actorType, actorTypeCounts[actorType]);
        if(currentChildItem == clickedItem) {
          // Toggle or force visibility if it was not on this branch so far
          vis = !draw || !vis;
          clickedSlicer->SetActorVisibility(actorType, actorTypeCounts[actorType], vis);
        }
        currentChildItem->setData(column, Qt::CheckStateRole, vis?2:0);
        actorTypeCounts[actorType]++;
      }
    }
  }

  clickedSlicer->Render();
}
//------------------------------------------------------------------------------

void vvMainWindow::InitSlicers()
{
  if (mSlicerManagers.size()) {
    mSlicerManagers.back()->GenerateDefaultLookupTable();

    mSlicerManagers.back()->SetSlicerWindow(0,NOViewWidget->GetRenderWindow());
    mSlicerManagers.back()->SetSlicerWindow(1,NEViewWidget->GetRenderWindow());
    mSlicerManagers.back()->SetSlicerWindow(2,SOViewWidget->GetRenderWindow());
    mSlicerManagers.back()->SetSlicerWindow(3,SEViewWidget->GetRenderWindow());
  }
}

//------------------------------------------------------------------------------
void vvMainWindow::InitDisplay()
{
  if (mSlicerManagers.size()) {
    //BE CAREFUL : this is absolutely necessary to set the interactor style
    //in order to have the same style instanciation for all SlicerManagers in
    // a same window
    for (int j = 0; j < 4; j++) {
      vvInteractorStyleNavigator* style = vvInteractorStyleNavigator::New();
      style->SetAutoAdjustCameraClippingRange(1);
      bool AlreadySelected = false;
      for (int i = 0; i < DataTree->topLevelItemCount(); i++) {
        mSlicerManagers[i]->SetInteractorStyleNavigator(j,style);

        //select the image only if previous are not selected
        if (DataTree->topLevelItem(i)->data(j+1,Qt::CheckStateRole).toInt() > 1) {
          mSlicerManagers[i]->UpdateSlicer(j,1);
          AlreadySelected = true;
        } else if (i == DataTree->topLevelItemCount()-1 && !AlreadySelected) {
          if (DataTree->selectedItems().size() == 0)
            DataTree->topLevelItem(i)->setSelected(1);
          DataTree->topLevelItem(i)->setData(j+1,Qt::CheckStateRole,2);
          mSlicerManagers[i]->UpdateSlicer(j,1);
          DisplaySliders(i,j);
        } else {
          DataTree->topLevelItem(i)->setData(j+1,Qt::CheckStateRole,0);
          mSlicerManagers[i]->UpdateSlicer(j,0);
        }
      }
      style->Delete();
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::DisplaySliders(int slicer, int window)
{
  if(!mSlicerManagers[slicer]->GetSlicer(window)->GetRenderer()->GetDraw())
    return;

  int range[2];
  mSlicerManagers[slicer]->GetSlicer(window)->GetSliceRange(range);
  int position = mSlicerManagers[slicer]->GetSlicer(window)->GetSlice();
  if (range[1]>0)
    verticalSliders[window]->show();
  else
    verticalSliders[window]->hide();
  verticalSliders[window]->setRange(range[0],range[1]);
  verticalSliders[window]->setValue(position);

  int tRange[2];
  tRange[0] = 0;
  tRange[1] = mSlicerManagers[slicer]->GetSlicer(window)->GetTMax();
  if (tRange[1]>0)
    horizontalSliders[window]->show();
  else
    horizontalSliders[window]->hide();
  horizontalSliders[window]->setRange(tRange[0],tRange[1]);
  int tPosition = mSlicerManagers[slicer]->GetSlicer(window)->GetMaxCurrentTSlice();
  horizontalSliders[window]->setValue(tPosition);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::CloseImage(QTreeWidgetItem* item, int column)
{
  int index = GetSlicerIndexFromItem(item);

  if (DataTree->topLevelItem(index) != item) {
    QString warning = "Do you really want to close the overlay : ";
    warning += item->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
    QMessageBox msgBox(QMessageBox::Warning, tr("Close Overlay"),
                       warning, 0, this);
    msgBox.addButton(tr("Close"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole) {
      std::string overlay_type=item->data(1,Qt::UserRole).toString().toStdString();
      int overlay_index=0;
      for (int child = 0; child < DataTree->topLevelItem(index)->childCount(); child++) {
        if (DataTree->topLevelItem(index)->\
            child(child)->data(1,Qt::UserRole).toString().toStdString() == overlay_type)
          overlay_index++;
        if (DataTree->topLevelItem(index)->child(child) == item) break;
      }
      mSlicerManagers[index]->RemoveActor(overlay_type, overlay_index-1);
      mSlicerManagers[index]->SetColorMap(0);
      DataTree->topLevelItem(index)->takeChild(DataTree->topLevelItem(index)->indexOfChild(item));
      mSlicerManagers[index]->Render();
    }
  } else if (DataTree->topLevelItemCount() <= 1) {
    QString warning = "Do you really want to close the image : ";
    warning += item->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
    warning += "\nThis is the last image, you're about to close vv !!!";
    QMessageBox msgBox(QMessageBox::Warning, tr("Close Image"),
                       warning, 0, this);
    msgBox.addButton(tr("Close vv"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole) {
      this->close();
    }
  } else {
    QString warning = "Do you really want to close the image : ";
    warning += item->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
    QMessageBox msgBox(QMessageBox::Warning, tr("Close Image"),
                       warning, 0, this);
    msgBox.addButton(tr("Close"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole) {

      // Tell tools that we close an image
      emit AnImageIsBeingClosed(mSlicerManagers[index]);

      std::vector<vvSlicerManager*>::iterator Manageriter = mSlicerManagers.begin();
      DataTree->takeTopLevelItem(index);
      for (int i = 0; i < index; i++) {
        Manageriter++;
      }
      linkPanel->removeImage(index);
      mSlicerManagers[index]->RemoveActors();
      delete mSlicerManagers[index];
      mSlicerManagers.erase(Manageriter);

      //
      InitDisplay();
    }
  }
  ImageInfoChanged();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ReloadImage(QTreeWidgetItem* item, int column)
{
  // int index = GetSlicerIndexFromItem(item);
  //   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  //   if (item->data(1,Qt::UserRole).toString() == "vector")
  //     mSlicerManagers[index]->ReloadVF();
  //   else
  //     mSlicerManagers[index]->Reload();

  //   QApplication::restoreOverrideCursor();
  int index = GetSlicerIndexFromItem(item);
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  QString role=item->data(1,Qt::UserRole).toString();
  if ( role == "vector"){
    mSlicerManagers[index]->ReloadVF();
  }
  else if (role == "overlay"){
    mSlicerManagers[index]->ReloadOverlay();
  }
  else if (role == "fusion"){
    mSlicerManagers[index]->ReloadFusion();
  }
  else{
    mSlicerManagers[index]->Reload();
  }
  // Update view and info
  ImageInfoChanged();
  mSlicerManagers[index]->Render();
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::MousePositionChanged(int visibility,double x, double y, double z, double X, double Y, double Z , double value)
{
  infoPanel->setCurrentInfo(visibility,x,y,z,X,Y,Z,value);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::VectorChanged(int visibility,double x, double y, double z, double value)
{
  overlayPanel->getCurrentVectorInfo(visibility,x,y,z,value);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::OverlayChanged(int visibility, double valueOver, double valueRef)
{
  overlayPanel->getCurrentOverlayInfo(visibility,valueOver, valueRef);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::FusionChanged(int visibility, double value)
{
  overlayPanel->getCurrentFusionInfo(visibility,value);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::WindowLevelChanged()
{
  // Base image
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if(index==-1) return;
  windowSpinBox->setValue(mSlicerManagers[index]->GetColorWindow());
  levelSpinBox->setValue(mSlicerManagers[index]->GetColorLevel());
  colorMapComboBox->setCurrentIndex(mSlicerManagers[index]->GetColorMap());
  presetComboBox->setCurrentIndex(mSlicerManagers[index]->GetPreset());

  // Overlay image
  if (mSlicerManagers[index]->GetSlicer(0)->GetOverlay())
    overlayPanel->getOverlayProperty(mSlicerManagers[index]->GetOverlayColor(),
                                     mSlicerManagers[index]->GetLinkOverlayWindowLevel(),
                                     mSlicerManagers[index]->GetOverlayColorWindow(),
                                     mSlicerManagers[index]->GetOverlayColorLevel());
  else
    overlayPanel->getOverlayProperty(-1,0,0.,0.);

  // Fusion image
  if (mSlicerManagers[index]->GetSlicer(0)->GetFusion())
    overlayPanel->getFusionProperty(mSlicerManagers[index]->GetFusionOpacity(),
                                    mSlicerManagers[index]->GetFusionThresholdOpacity(),
                                    mSlicerManagers[index]->GetFusionColorMap(),
                                    mSlicerManagers[index]->GetFusionWindow(),
                                    mSlicerManagers[index]->GetFusionLevel());
  else
    overlayPanel->getFusionProperty(-1, -1, -1, -1, -1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::WindowLevelEdited()
{
  presetComboBox->setCurrentIndex(6);
  UpdateWindowLevel();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SetWindowLevel(double w, double l)
{
  windowSpinBox->setValue(w);
  levelSpinBox->setValue(l);
  presetComboBox->setCurrentIndex(6);
  colorMapComboBox->setCurrentIndex(0);
  UpdateWindowLevel();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateWindowLevel()
{
  if (DataTree->selectedItems().size()) {
    if (presetComboBox->currentIndex() == 7) //For ventilation
      colorMapComboBox->setCurrentIndex(5);
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
    mSlicerManagers[index]->SetColorWindow(windowSpinBox->value());
    mSlicerManagers[index]->SetColorLevel(levelSpinBox->value());
    mSlicerManagers[index]->SetPreset(presetComboBox->currentIndex());
    mSlicerManagers[index]->Render();
    WindowLevelChanged();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateSlicingPreset()
{
  if (DataTree->selectedItems().size()) {
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
    mSlicerManagers[index]->SetSlicingPreset(vvSlicerManager::SlicingPresetType(slicingPresetComboBox->currentIndex()));
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateColorMap()
{
  if (DataTree->selectedItems().size()) {
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
    mSlicerManagers[index]->SetColorMap(colorMapComboBox->currentIndex());
    mSlicerManagers[index]->Render();
  }
}
//------------------------------------------------------------------------------
void vvMainWindow::SwitchWindowLevel()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  int window = mSlicerManagers[index]->GetColorWindow();
  presetComboBox->setCurrentIndex(6);
  windowSpinBox->setValue(-window);
  UpdateWindowLevel();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ApplyWindowLevelToAllImages()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if(index==-1) return;
  double window = mSlicerManagers[index]->GetColorWindow();
  double level = mSlicerManagers[index]->GetColorLevel();

  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (mSlicerManagers[i] == NULL)
      continue;
    mSlicerManagers[i]->SetColorWindow(window);
    mSlicerManagers[i]->SetColorLevel(level);
    mSlicerManagers[i]->SetPreset(6);
    mSlicerManagers[i]->Render();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ApplyWindowToSetOfImages(double window, unsigned int indexMin, unsigned int indexMax)
{
  for (unsigned int i = indexMin; i <= indexMax && i < mSlicerManagers.size(); i++) {
    if (mSlicerManagers[i] == NULL)
      continue;
    mSlicerManagers[i]->SetColorWindow(window);
    mSlicerManagers[i]->SetPreset(6);
    mSlicerManagers[i]->Render();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ApplyLevelToSetOfImages(double level, unsigned int indexMin, unsigned int indexMax)
{
  for (unsigned int i = indexMin; i <= indexMax && i < mSlicerManagers.size(); i++) {
    if (mSlicerManagers[i] == NULL)
      continue;
    mSlicerManagers[i]->SetColorLevel(level);
    mSlicerManagers[i]->SetPreset(6);
    mSlicerManagers[i]->Render();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateLinkManager(std::string id, int slicer, double x, double y, double z, int temps)
{
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (mSlicerManagers[i]->GetId() == id) {
      //mSlicerManagers[i]->SetTSlice(temps);
      mSlicerManagers[i]->GetSlicer(slicer)->SetCurrentPosition(x,y,z,temps);
      mSlicerManagers[i]->UpdateViews(0,slicer);
      break;
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateLinkedNavigation(std::string id, vvSlicerManager * sm, vvSlicer* refSlicer)
{
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (id == mSlicerManagers[i]->GetId()) {
      mSlicerManagers[i]->UpdateLinkedNavigation(refSlicer);
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ShowContextMenu(QPoint point)
{
  if (!DataTree->selectedItems().size()) {
    contextActions[1]->setEnabled(0);
    contextActions[2]->setEnabled(0);
    contextActions[3]->setEnabled(0);
    contextActions[4]->setEnabled(0);
    contextActions[5]->setEnabled(0);
    contextActions[6]->setEnabled(0);
  } else {
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
    contextActions[1]->setEnabled(1);
    contextActions[2]->setEnabled(
      DataTree->itemWidget(DataTree->selectedItems()[0],
                           COLUMN_RELOAD_IMAGE)->isEnabled());
    contextActions[3]->setEnabled(1);
    contextActions[5]->setEnabled(1);
    contextActions[6]->setEnabled(1);

    if (mSlicerManagers[index]->GetDimension() < 3)
      contextActions[4]->setEnabled(0);
    else
      contextActions[4]->setEnabled(1);
  }
  contextMenu.exec(QCursor::pos());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::CloseImage()
{
  CloseImage(DataTree->selectedItems()[0],0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ReloadImage()
{
  ReloadImage(DataTree->selectedItems()[0],0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SelectOverlayImage()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);

  //check if one overlay image is added
  for (int child = 0; child < DataTree->topLevelItem(index)->childCount(); child++)
    if (DataTree->topLevelItem(index)->child(child)->data(1,Qt::UserRole).toString() == "overlay") {
      QString error = "Cannot add more than one compared image\n";
      error += "Please remove first ";
      error += DataTree->topLevelItem(index)->child(child)->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
      QMessageBox::information(this,tr("Problem adding compared image !"),error);
      return;
    }

  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";
  QStringList files = QFileDialog::getOpenFileNames(this,tr("Load Overlay image"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;

  std::vector<std::string> vecFileNames;
  for (int i = 0; i < files.size(); i++) {
    vecFileNames.push_back(files[i].toStdString());
  }
  AddOverlayImage(index,vecFileNames,vvImageReader::IMAGE);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::AddOverlayImage(int index, std::vector<std::string> fileNames, vvImageReader::LoadedImageType type)
{
  QString file(fileNames[0].c_str());
  if (QFile::exists(file))
  {
    mInputPathName = itksys::SystemTools::GetFilenamePath(file.toStdString()).c_str();
    itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(
        file.toStdString().c_str(), itk::ImageIOFactory::ReadMode);
    reader->SetFileName(fileNames[0].c_str());
    reader->ReadImageInformation();
    std::string component = reader->GetComponentTypeAsString(reader->GetComponentType());
    int dimension = reader->GetNumberOfDimensions();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    vvProgressDialog progress("Opening " + file.toStdString());
    qApp->processEvents();

    std::string filename = itksys::SystemTools::GetFilenameWithoutExtension(file.toStdString()).c_str();
    if (mSlicerManagers[index]->SetOverlay(fileNames,dimension, component,type)) {
      //create an item in the tree with good settings
      QTreeWidgetItem *item = new QTreeWidgetItem();
      item->setData(0,Qt::UserRole,file.toStdString().c_str());
      item->setData(1,Qt::UserRole,tr("overlay"));
      QFileInfo fileinfo(file); //Do not show the path
      item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,fileinfo.fileName());
      item->setToolTip(COLUMN_IMAGE_NAME, mSlicerManagers[index]->GetListOfAbsoluteFilePathInOneString("overlay").c_str());
      qApp->processEvents();

      for (int j = 1; j <= 4; j++) {
        item->setData(j,Qt::CheckStateRole,DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole));
      }

      //Create the buttons for reload and close
      qApp->processEvents();
      QTreePushButton* cButton = new QTreePushButton;
      cButton->setItem(item);
      cButton->setColumn(COLUMN_CLOSE_IMAGE);
      cButton->setToolTip(tr("close image"));
      cButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/exit.png")));
      connect(cButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
          this,SLOT(CloseImage(QTreeWidgetItem*, int)));

      QTreePushButton* rButton = new QTreePushButton;
      rButton->setItem(item);
      rButton->setColumn(COLUMN_RELOAD_IMAGE);
      rButton->setToolTip(tr("reload image"));
      rButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/rotateright.png")));
      connect(rButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
          this,SLOT(ReloadImage(QTreeWidgetItem*, int)));

      DataTree->topLevelItem(index)->setExpanded(1);
      DataTree->topLevelItem(index)->addChild(item);
      DataTree->setItemWidget(item, COLUMN_CLOSE_IMAGE, cButton);
      DataTree->setItemWidget(item, COLUMN_RELOAD_IMAGE, rButton);

      //set the id of the image
      QString id = DataTree->topLevelItem(index)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString();
      item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
      UpdateTree();
      qApp->processEvents();
      ImageInfoChanged();
      QApplication::restoreOverrideCursor();

      // Update the display to update, e.g., the sliders
      for(int i=0; i<4; i++)
        DisplaySliders(index, i);
    } else {
      QApplication::restoreOverrideCursor();
      QString error = "Cannot import the new image.\n";
      error += mSlicerManagers[index]->GetLastError().c_str();
      QMessageBox::information(this,tr("Problem reading image !"),error);
    }
    WindowLevelChanged();
  }
  else
    QMessageBox::information(this,tr("Problem reading Overlay !"),"File doesn't exist!");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::AddROI(int index, QString file)
{
  /*
  // Get slice manager

  // Load image

  vvImageReader * mReader = new vvImageReader;
  mReader->SetInputFilename(filename.toStdString());
  mReader->Update(IMAGE);
  if (mReader->GetLastError().size() != 0) {
    std::cerr << "Error while reading " << filename.toStdString() << std::endl;
    QString error = "Cannot open file \n";
    error += mReader->GetLastError().c_str();
    QMessageBox::information(this,tr("Reading problem"),error);
    delete mReader;
    return;
  }
  vvImage::Pointer roi = mReader->GetOutput();

  // Create roi in new tool
  vvToolStructureSetManager::AddImage(mCurrentSlicerManager, roi);
  */
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SelectFusionImage()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);

  //check if one fusion image is added
  for (int child = 0; child < DataTree->topLevelItem(index)->childCount(); child++)
    if (DataTree->topLevelItem(index)->child(child)->data(1,Qt::UserRole).toString() == "fusion") {
      QString error = "Cannot add more than one fusion image\n";
      error += "Please remove first ";
      error += DataTree->topLevelItem(index)->child(child)->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
      QMessageBox::information(this,tr("Problem adding fusion image !"),error);
      return;
    }

  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";
  QString file = QFileDialog::getOpenFileName(this,tr("Load Fusion image"),mInputPathName,Extensions);
  if (!file.isEmpty())
    AddFusionImage(index,file);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ResetTransformationToIdentity()
{
  std::string actorType = DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString().toStdString();
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  mSlicerManagers[index]->ResetTransformationToIdentity(actorType);
  ImageInfoChanged();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::AddFusionImage(int index, QString file)
{
  if (QFile::exists(file))
  {
    mInputPathName = itksys::SystemTools::GetFilenamePath(file.toStdString()).c_str();
    itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(
        file.toStdString().c_str(), itk::ImageIOFactory::ReadMode);
    reader->SetFileName(file.toStdString().c_str());
    reader->ReadImageInformation();
    std::string component = reader->GetComponentTypeAsString(reader->GetComponentType());
    if (reader) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      vvProgressDialog progress("Opening fusion");
      qApp->processEvents();

      std::string filename = itksys::SystemTools::GetFilenameWithoutExtension(file.toStdString()).c_str();
      if (mSlicerManagers[index]->SetFusion(file.toStdString(),
            reader->GetNumberOfDimensions(), component)) {
        //create an item in the tree with good settings
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setData(0,Qt::UserRole,file.toStdString().c_str());
        item->setData(1,Qt::UserRole,tr("fusion"));
        QFileInfo fileinfo(filename.c_str()); //Do not show the path
        item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,fileinfo.fileName());
        item->setToolTip(COLUMN_IMAGE_NAME, mSlicerManagers[index]->GetListOfAbsoluteFilePathInOneString("fusion").c_str());
        qApp->processEvents();

        for (int j = 1; j <= 4; j++) {
          item->setData(j,Qt::CheckStateRole,DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole));
        }

        //Create the buttons for reload and close
        qApp->processEvents();
        QTreePushButton* cButton = new QTreePushButton;
        cButton->setItem(item);
        cButton->setColumn(COLUMN_CLOSE_IMAGE);
        cButton->setToolTip(tr("close image"));
        cButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/exit.png")));
        connect(cButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
            this,SLOT(CloseImage(QTreeWidgetItem*, int)));

        QTreePushButton* rButton = new QTreePushButton;
        rButton->setItem(item);
        rButton->setColumn(COLUMN_RELOAD_IMAGE);
        rButton->setToolTip(tr("reload image"));
        rButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/rotateright.png")));
        connect(rButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
            this,SLOT(ReloadImage(QTreeWidgetItem*, int)));

        DataTree->topLevelItem(index)->setExpanded(1);
        DataTree->topLevelItem(index)->addChild(item);
        DataTree->setItemWidget(item, COLUMN_CLOSE_IMAGE, cButton);
        DataTree->setItemWidget(item, COLUMN_RELOAD_IMAGE, rButton);

        //set the id of the image
        QString id = DataTree->topLevelItem(index)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString();
        item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
        UpdateTree();
        qApp->processEvents();
        ImageInfoChanged();
        QApplication::restoreOverrideCursor();
      } else {
        QApplication::restoreOverrideCursor();
        QString error = "Cannot import the new image.\n";
        error += mSlicerManagers[index]->GetLastError().c_str();
        QMessageBox::information(this,tr("Problem reading image !"),error);
      }
    } else {
      QString error = "Cannot import the new image.\n";
      QMessageBox::information(this,tr("Problem reading image !"),error);
    }
  }
  else
    QMessageBox::information(this,tr("Problem reading Fusion !"),"File doesn't exist!");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::OpenField()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  //check if a vector field has already been added
  for (int child = 0; child < DataTree->topLevelItem(index)->childCount(); child++)
    if (DataTree->topLevelItem(index)->child(child)->data(1,Qt::UserRole).toString() == "vector") {
      QString error = "Cannot add more than one vector field\n";
      error += "Please remove first ";
      error += DataTree->topLevelItem(index)->child(child)->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
      QMessageBox::information(this,tr("Problem adding vector field!"),error);
      return;
    }

  QString Extensions = "Images ( *.mhd)";
  Extensions += ";;Images ( *.mha)";
  Extensions += ";;VF Images ( *.vf)";
  Extensions += ";;nii Images ( *.nii)";
  Extensions += ";;nrrd Images ( *.nrrd)";
  Extensions += ";;nhdr Images ( *.nhdr)";
  Extensions += ";;All Files (*)";
  QString file = QFileDialog::getOpenFileName(this,tr("Load deformation field"),mInputPathName,Extensions);
  if (!file.isEmpty())
    AddField(file,index);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::AddFieldEntry(QString filename,int index,bool from_disk)
{
  //create an item in the tree with good settings
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setData(0,Qt::UserRole,filename.toStdString().c_str());
  item->setData(1,Qt::UserRole,tr("vector"));
  QFileInfo fileinfo(filename); //Do not show the path
  item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,fileinfo.fileName());
  item->setToolTip(COLUMN_IMAGE_NAME, mSlicerManagers[index]->GetListOfAbsoluteFilePathInOneString("vector").c_str());
  qApp->processEvents();

  for (int j = 1; j <= 4; j++) {
    item->setData(j,Qt::CheckStateRole,DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole));
  }

  //Create the buttons for reload and close
  qApp->processEvents();
  QTreePushButton* cButton = new QTreePushButton;
  cButton->setItem(item);
  cButton->setColumn(COLUMN_CLOSE_IMAGE);
  cButton->setToolTip(tr("close vector field"));
  cButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/exit.png")));
  connect(cButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
          this,SLOT(CloseImage(QTreeWidgetItem*, int)));

  QTreePushButton* rButton = new QTreePushButton;
  rButton->setItem(item);
  rButton->setColumn(COLUMN_RELOAD_IMAGE);
  rButton->setToolTip(tr("reload vector field"));
  rButton->setEnabled(from_disk);
  rButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/rotateright.png")));
  connect(rButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
          this,SLOT(ReloadImage(QTreeWidgetItem*, int)));

  DataTree->topLevelItem(index)->setExpanded(1);
  DataTree->topLevelItem(index)->addChild(item);
  DataTree->setItemWidget(item, COLUMN_RELOAD_IMAGE, rButton);
  DataTree->setItemWidget(item, COLUMN_CLOSE_IMAGE, cButton);

  //set the id of the image
  QString id = DataTree->topLevelItem(index)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString();
  item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
  UpdateTree();
  qApp->processEvents();
  ImageInfoChanged();
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::AddField(vvImage::Pointer vf,QString file,int index)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vvSlicerManager* imageManager = mSlicerManagers[index];
  if (imageManager->SetVF(vf,file.toStdString())) {
    AddFieldEntry(file,index,false);
  } else {
    QString error = "Cannot import the vector field for this image.\n";
    error += imageManager->GetLastError().c_str();
    QMessageBox::information(this,tr("Problem reading VF !"),error);
  }
  QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::AddField(QString file,int index)
{
  if (QFile::exists(file)) {
    mInputPathName = itksys::SystemTools::GetFilenamePath(file.toStdString()).c_str();

    //init the progress events
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    vvProgressDialog progress("Opening " + file.toStdString());
    qApp->processEvents();

    //read the vector and put it in the current mSlicerManager
    vvSlicerManager* imageManager = mSlicerManagers[index];
    qApp->processEvents();

    std::string filename = itksys::SystemTools::GetFilenameWithoutExtension(file.toStdString()).c_str();
    if (imageManager->SetVF(file.toStdString())) {
      imageManager->Render();
      AddFieldEntry(file,index,true);
    } else {
      QApplication::restoreOverrideCursor();
      QString error = "Cannot import the vector field for this image.\n";
      error += imageManager->GetLastError().c_str();
      QMessageBox::information(this,tr("Problem reading VF !"),error);
    }
  } else
    QMessageBox::information(this,tr("Problem reading VF !"),"File doesn't exist!");

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SetVFProperty(int subsampling, int scale, int log, int width, double r, double g, double b)
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if (mSlicerManagers[index]->GetSlicer(0)->GetVF()) {
    for (int i = 0; i < 4; i++) {
      mSlicerManagers[index]->GetSlicer(i)->SetVFSubSampling(subsampling);
      mSlicerManagers[index]->GetSlicer(i)->SetVFScale(scale);
      mSlicerManagers[index]->GetSlicer(i)->SetVFWidth(width);
      mSlicerManagers[index]->GetSlicer(i)->SetVFColor(r,g,b);
      if (log > 0)
        mSlicerManagers[index]->GetSlicer(i)->SetVFLog(1);
      else
        mSlicerManagers[index]->GetSlicer(i)->SetVFLog(0);
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SetOverlayProperty(int color, int linked, double window, double level)
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if (mSlicerManagers[index]->GetSlicer(0)->GetOverlay()) {
    mSlicerManagers[index]->SetOverlayColor(color);
    mSlicerManagers[index]->SetColorMap(0);
    mSlicerManagers[index]->SetLinkOverlayWindowLevel(linked);
    mSlicerManagers[index]->SetOverlayColorWindow(window);
    mSlicerManagers[index]->SetOverlayColorLevel(level);
    mSlicerManagers[index]->Render();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SetFusionProperty(int opacity, int thresOpacity, int colormap,double window, double level, bool showLegend)
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if (mSlicerManagers[index]->GetSlicer(0)->GetFusion()) {
    mSlicerManagers[index]->SetFusionColorMap(colormap);
    mSlicerManagers[index]->SetFusionOpacity(opacity);
    mSlicerManagers[index]->SetFusionThresholdOpacity(thresOpacity);
    mSlicerManagers[index]->SetFusionWindow(window);
    mSlicerManagers[index]->SetFusionLevel(level);
    mSlicerManagers[index]->SetFusionShowLegend(showLegend);
    mSlicerManagers[index]->SetColorMap(0);
    mSlicerManagers[index]->Render();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveAs()
{
  if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "vector") {
    QMessageBox::warning(this,tr("Unsupported type"),tr("Sorry, saving a vector field is unsupported for the moment"));
    return;
  }

  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  int dimension = mSlicerManagers[index]->GetDimension();
  QStringList OutputListeFormat;
  OutputListeFormat.clear();
  if (dimension == 1) {
    OutputListeFormat.push_back(".mhd");
    OutputListeFormat.push_back(".mha");
  }
  if (dimension == 2) {
    OutputListeFormat.push_back(".bmp");
    OutputListeFormat.push_back(".png");
    OutputListeFormat.push_back(".jpeg");
    OutputListeFormat.push_back(".tif");
    OutputListeFormat.push_back(".mhd");
    OutputListeFormat.push_back(".mha");
    OutputListeFormat.push_back(".hdr");
    OutputListeFormat.push_back(".vox");
  } else if (dimension == 3) {
    OutputListeFormat.push_back(".mhd");
    OutputListeFormat.push_back(".mha");
    OutputListeFormat.push_back(".nii");
    OutputListeFormat.push_back(".nrrd");
    OutputListeFormat.push_back(".nhdr");
    OutputListeFormat.push_back(".hdr");
    OutputListeFormat.push_back(".vox");
  } else if (dimension == 4) {
    OutputListeFormat.push_back(".mhd");
    OutputListeFormat.push_back(".mha");
    OutputListeFormat.push_back(".nii");
    OutputListeFormat.push_back(".nrrd");
    OutputListeFormat.push_back(".nhdr");
  }
  QString Extensions = "AllFiles(*.*)";
  for (int i = 0; i < OutputListeFormat.count(); i++) {
    Extensions += ";;Images ( *";
    Extensions += OutputListeFormat[i];
    Extensions += ")";
  }
  QString fileName = QFileDialog::getSaveFileName(this,
                     tr("Save As"),
                     mSlicerManagers[index]->GetFileName().c_str(),
                     Extensions);
  if (!fileName.isEmpty()) {
    std::string fileformat = itksys::SystemTools::GetFilenameLastExtension(fileName.toStdString());
    if (OutputListeFormat.contains(
          fileformat.c_str())) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      std::string action = "Saving";
      vvProgressDialog progress("Saving "+fileName.toStdString());
      qApp->processEvents();
      vvImageWriter::Pointer writer = vvImageWriter::New();
      writer->SetOutputFileName(fileName.toStdString());
      writer->SetInput(mSlicerManagers[index]->GetImage());

      // Check on transform and prompt user
      writer->SetSaveTransform(false);
      bool bId = true;
      for(int i=0; i<4; i++)
        for(int j=0; j<4; j++) {
          // TODO SR and BP: check on the list of transforms and not the first only
          double elt = mSlicerManagers[index]->GetImage()->GetTransform()[0]->GetMatrix()->GetElement(i,j);
          if(i==j && elt!=1.)
            bId = false;
          if(i!=j && elt!=0.)
            bId = false;
        }
      if( !bId ) {
        QString warning = "The image has an associated linear transform. Do you want to save it along?";
        QMessageBox msgBox(QMessageBox::Warning, tr("Save transform"), warning, 0, this);
        msgBox.addButton(tr("Yes"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("No"), QMessageBox::RejectRole);
        if (msgBox.exec() == QMessageBox::AcceptRole)
          writer->SetSaveTransform(true);
      }

      writer->Update();
      QApplication::restoreOverrideCursor();
      if (writer->GetLastError().size()) {
        QString error = "Saving did not succeed\n";
        error += writer->GetLastError().c_str();
        QMessageBox::information(this,tr("Saving Problem"),error);
        SaveAs();
      }
    } else {
      QString error = fileformat.c_str();
      if (error.isEmpty())
        error += "no file format specified !";
      else
        error += " format unknown !!!\n";
      QMessageBox::information(this,tr("Saving Problem"),error);
      SaveAs();
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveCurrentState()
{
  QString Extensions = "XML Files(*.xml)";
  QString fileName = QFileDialog::getSaveFileName(this,
                     tr("Save Current Window State"),
                     "",
                     Extensions);
                     
  SaveCurrentStateAs(fileName.toStdString());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveCurrentStateAs(const std::string& stateFile)
{
  vvSaveState save_state;
  save_state.Run(this, stateFile);
}

//------------------------------------------------------------------------------
void vvMainWindow::ReadSavedState()
{
  QString Extensions = "XML Files(*.xml)";
  QString fileName = QFileDialog::getOpenFileName(this,
                     tr("Load Window State"),
                     "",
                     Extensions);
                     
  ReadSavedStateFile(fileName.toStdString());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ReadSavedStateFile(const std::string& stateFile)
{
  vvReadState read_state;
  read_state.Run(this, stateFile);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::LinkAllImages()
{
	linkPanel->linkAll();
}

//------------------------------------------------------------------------------
void vvMainWindow::AddLink(QString image1,QString image2,bool fromPanel)
{
  if (!fromPanel) {
    // delegate to linkPanel if call came from elsewhere...
    linkPanel->addLinkFromIds(image1, image2);
    return;
  }
  
  unsigned int sm1 = 0;
  unsigned int sm2 = 0;

  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (image1.toStdString() == mSlicerManagers[i]->GetId()) {
      mSlicerManagers[i]->AddLink(image2.toStdString());
      sm1 = i;
    }
    if (image2.toStdString() == mSlicerManagers[i]->GetId()) {
      mSlicerManagers[i]->AddLink(image1.toStdString());
      sm2 = i;
    }
  }

  if (linkPanel->isLinkAll())	{
    emit UpdateLinkedNavigation(mSlicerManagers[sm1]->GetId(), mSlicerManagers[mCurrentPickedImageIndex], mSlicerManagers[mCurrentPickedImageIndex]->GetSlicer(0));
    emit UpdateLinkedNavigation(mSlicerManagers[sm2]->GetId(), mSlicerManagers[mCurrentPickedImageIndex], mSlicerManagers[mCurrentPickedImageIndex]->GetSlicer(0));
  } else {
    emit UpdateLinkedNavigation(mSlicerManagers[sm2]->GetId(), mSlicerManagers[sm1], mSlicerManagers[sm1]->GetSlicer(0));
  }
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::RemoveLink(QString image1,QString image2)
{
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (image1.toStdString() == mSlicerManagers[i]->GetId()) {
      mSlicerManagers[i]->RemoveLink(image2.toStdString());
    }
    if (image2.toStdString() == mSlicerManagers[i]->GetId()) {
      mSlicerManagers[i]->RemoveLink(image1.toStdString());
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ChangeImageWithIndexOffset(vvSlicerManager *sm, int slicer, int offset)
{
  if(mSlicerManagers.size()==1)
    return;

  int index = 0;
  while(sm != mSlicerManagers[index])
    index++;
  index = (index+offset+mSlicerManagers.size()) % mSlicerManagers.size();

  QTreeWidgetItem* item = GetItemFromSlicerManager(mSlicerManagers[index]);
  item->setData(slicer+1,Qt::CheckStateRole,2);         //change checkbox
  CurrentImageChanged(mSlicerManagers[index]->GetId()); //select new image
  DisplayChanged(item,slicer+1);
}
//------------------------------------------------------------------------------

void vvMainWindow::HorizontalSliderMoved(int value,int column, int slicer_index)
{
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (DataTree->topLevelItem(i)->data(column,Qt::CheckStateRole).toInt() > 1) {
      for (int j = 0; j < 4; j++) {
        mSlicerManagers[i]->SetTSliceInSlicer(value,j);
        //if (mSlicerManagers[i]->GetSlicer(j)->GetImageActor()->GetVisibility())
        //UpdateTSlice(j,value);
      }
      mSlicerManagers[i]->GetSlicer(slicer_index)->Render();
      break;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::NOHorizontalSliderMoved()
{
  // if (mCurrentTime == NOHorizontalSlider->value()) return;
  HorizontalSliderMoved(NOHorizontalSlider->value(),COLUMN_UL_VIEW,0);
//  mCurrentTime = NOHorizontalSlider->value();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::NEHorizontalSliderMoved()
{
  // if (mCurrentTime == NEHorizontalSlider->value()) return;
  HorizontalSliderMoved(NEHorizontalSlider->value(),COLUMN_UR_VIEW,1);
//  mCurrentTime = NEHorizontalSlider->value();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SOHorizontalSliderMoved()
{
  // if (mCurrentTime == SOHorizontalSlider->value()) return;
  HorizontalSliderMoved(SOHorizontalSlider->value(),COLUMN_DL_VIEW,2);
  // mCurrentTime = SOHorizontalSlider->value();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SEHorizontalSliderMoved()
{
  // if (mCurrentTime == SEHorizontalSlider->value()) return;
  HorizontalSliderMoved(SEHorizontalSlider->value(),COLUMN_DR_VIEW,3);
  // mCurrentTime = SEHorizontalSlider->value();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::NOVerticalSliderChanged()
{
  static int value=-1;
  if (value == NOVerticalSlider->value()) return;
  else value = NOVerticalSlider->value();
  //  int value = NOVerticalSlider->value();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (DataTree->topLevelItem(i)->data(COLUMN_UL_VIEW,Qt::CheckStateRole).toInt() > 1) {
      if (mSlicerManagers[i]->GetSlicer(0)->GetSlice() != value) {
        mSlicerManagers[i]->GetSlicer(0)->SetSlice(value);
        mSlicerManagers[i]->VerticalSliderHasChanged(0, value);

        // If nor Update/Render -> slider not work
        // only render = ok navigation, but for contour Update needed but slower ?

        mSlicerManagers[i]->UpdateSlice(0);  // <-- DS add this. Not too much update ? YES. but needed for ImageContour ...
        //mSlicerManagers[i]->GetSlicer(0)->Render(); // <-- DS add this, needed for contour, seems ok ? not too slow ?
      }
      break;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::NEVerticalSliderChanged()
{
  static int value=-1;
  if (value == NEVerticalSlider->value()) return;
  else value = NEVerticalSlider->value();
  //  int value = NEVerticalSlider->value();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (DataTree->topLevelItem(i)->data(COLUMN_UR_VIEW,Qt::CheckStateRole).toInt() > 1) {
      if (mSlicerManagers[i]->GetSlicer(1)->GetSlice() != value) {
        mSlicerManagers[i]->GetSlicer(1)->SetSlice(value);
        mSlicerManagers[i]->VerticalSliderHasChanged(1, value);
        mSlicerManagers[i]->UpdateSlice(1);
        //mSlicerManagers[i]->GetSlicer(1)->Render(); // <-- DS add this, needed for contour, seems ok ? not too slow ?
      }
      break;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SOVerticalSliderChanged()
{
  static int value=-1;
  if (value == SOVerticalSlider->value()) return;
  else value = SOVerticalSlider->value();
  //int value = SOVerticalSlider->value();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (DataTree->topLevelItem(i)->data(COLUMN_DL_VIEW,Qt::CheckStateRole).toInt() > 1) {
      if (mSlicerManagers[i]->GetSlicer(2)->GetSlice() != value) {
        mSlicerManagers[i]->GetSlicer(2)->SetSlice(value);
        mSlicerManagers[i]->VerticalSliderHasChanged(2, value);
        mSlicerManagers[i]->UpdateSlice(2);
        //mSlicerManagers[i]->GetSlicer(2)->Render(); // <-- DS add this, needed for contour, seems ok ? not too slow ?
      }
      // else { DD("avoid SOVerticalSlider slicer update"); }
      break;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SEVerticalSliderChanged()
{
  static int value=-1;
  if (value == SEVerticalSlider->value()) return;
  else value = SEVerticalSlider->value();
  // int value = SEVerticalSlider->value();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    if (DataTree->topLevelItem(i)->data(COLUMN_DR_VIEW,Qt::CheckStateRole).toInt() > 1) {
      if (mSlicerManagers[i]->GetSlicer(3)->GetSlice() != value) {
        mSlicerManagers[i]->GetSlicer(3)->SetSlice(value);
        mSlicerManagers[i]->VerticalSliderHasChanged(3, value);
        mSlicerManagers[i]->UpdateSlice(3);
        //mSlicerManagers[i]->GetSlicer(3)->Render(); // <-- DS add this, needed for contour, seems ok ? not too slow ?
      }
      break;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::UpdateSlice(int slicer, int slice)
{
  // DD("vvMainWindow::UpdateSlice");
//   DD(slicer);
//   DD(slice);
  if (slicer == 0) {
    //    if (slice != NOVerticalSlider->value())
    NOVerticalSlider->setValue(slice);
  } else {
    if (slicer == 1)
      NEVerticalSlider->setValue(slice);
    else {
      if (slicer == 2)
        SOVerticalSlider->setValue(slice);
      else {
        if (slicer == 3)
          SEVerticalSlider->setValue(slice);
      }
    }
  }
  // DD("vvMainWindow:UpdateSlice END");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::UpdateTSlice(int slicer, int slice)
{
  switch (slicer) {
  case 0:
    NOHorizontalSlider->setValue(slice);
    break;
  case 1:
    NEHorizontalSlider->setValue(slice);
    break;
  case 2:
    SOHorizontalSlider->setValue(slice);
    break;
  case 3:
    SEHorizontalSlider->setValue(slice);
    break;
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::UpdateSliceRange(int slicer, int min, int max, int tmin, int tmax)
{
  //int position = int((min+max)/2);
  int position = mSlicerManagers[mCurrentPickedImageIndex]->GetSlicer(slicer)->GetSlice();
  if (slicer == 0) {
    NOVerticalSlider->setRange(min,max);
    NOHorizontalSlider->setRange(tmin,tmax);
    NOVerticalSlider->setValue(position);
  } else if (slicer == 1) {
    NEVerticalSlider->setRange(min,max);
    NEHorizontalSlider->setRange(tmin,tmax);
    NEVerticalSlider->setValue(position);
  } else if (slicer == 2) {
    SOVerticalSlider->setRange(min,max);
    SOHorizontalSlider->setRange(tmin,tmax);
    SOVerticalSlider->setValue(position);
  } else if (slicer == 3) {
    SEVerticalSlider->setRange(min,max);
    SEHorizontalSlider->setRange(tmin,tmax);
    SEVerticalSlider->setValue(position);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SaveNOScreenshot()
{
  SaveScreenshot(NOViewWidget);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SaveNEScreenshot()
{
  SaveScreenshot(NEViewWidget);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SaveSOScreenshot()
{
  SaveScreenshot(SOViewWidget);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SaveSEScreenshot()
{
  SaveScreenshot(SEViewWidget);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SaveScreenshotAllSlices()
{
  QVTKWidget *widget = NOViewWidget;

  int index = 0;// GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  vvSlicerManager * SM = mSlicerManagers[index];
  vvImage * image = SM->GetImage();
  vvSlicer * slicer = SM->GetSlicer(0);
  int orientation = slicer->GetOrientation();
  int nbSlices = image->GetSize()[orientation];
  vtkSmartPointer<vtkRenderWindow>  renderWindow = widget->GetRenderWindow();

  // Select filename base
  QString filename = QFileDialog::getSaveFileName(this,
                                                  tr("Save As (filename will be completed by slice number)"),
                                                  itksys::SystemTools::GetFilenamePath(mSlicerManagers[index]->GetFileName()).c_str(),
                                                  "Images( *.png);;Images( *.jpg)");

  // Loop on slices
  for(int i=0; i<nbSlices; i++) {
    // Change the slice
    slicer->SetSlice(i); // -> change the slice of the current slicer
    SM->UpdateSlice(0); // --> this one emit UpdateSlice
    QCoreApplication::flush(); // -> needed to force display of contours 

    // Screenshot  
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->SetMagnification(1);
    windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
    windowToImageFilter->Update();
    
    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    std::string fn = itksys::SystemTools::GetFilenameWithoutLastExtension(filename.toStdString());
    std::string num = clitk::toString(i);
    if (i<10) num = "0"+num;
    if (i<100) num = "0"+num;
    if (i<1000) num = "0"+num;

    fn = itksys::SystemTools::GetFilenamePath(filename.toStdString()) + "/"+ fn 
      + "_" + num + itksys::SystemTools::GetFilenameLastExtension(filename.toStdString());
    writer->SetFileName(fn.c_str());
    writer->SetInput(windowToImageFilter->GetOutput());
    writer->Write();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::SaveScreenshot(QVTKWidget *widget)
{
  QString Extensions = "Images( *.png);;";
  Extensions += "Images( *.jpg);;";
  Extensions += "Images( *.bmp);;";
  Extensions += "Images( *.tif);;";
  Extensions += "Images( *.ppm)";
#if defined(VTK_USE_FFMPEG_ENCODER) || defined(VTK_USE_VIDEO_FOR_WINDOWS)
  Extensions += ";;Video( *.avi)";
#endif
#ifdef VTK_USE_MPEG2_ENCODER
  Extensions += ";;Video( *.mpg)";
#endif
#ifdef CLITK_EXPERIMENTAL
  Extensions += ";;Video( *.gif)";
#endif

  int smIndex=GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  QString fileName = QFileDialog::getSaveFileName(this,
                     tr("Save As"),
                     itksys::SystemTools::GetFilenamePath(mSlicerManagers[smIndex]->GetFileName()).c_str(),
                     Extensions);

  if (!fileName.isEmpty()) {
    vtkSmartPointer<vtkWindowToImageFilter> w2i = vtkSmartPointer<vtkWindowToImageFilter>::New();
    w2i->SetInput(widget->GetRenderWindow());
    w2i->Update();
    vtkImageData *image = w2i->GetOutput();

    std::string ext(itksys::SystemTools::GetFilenameLastExtension(fileName.toStdString()));

    // Image
    vtkImageWriter *imgwriter = NULL;
    if (ext==".bmp")
      imgwriter = vtkBMPWriter::New();
    else if (ext==".tif")
      imgwriter = vtkTIFFWriter::New();
    else if (ext==".ppm")
      imgwriter = vtkPNMWriter::New();
    else if (ext==".png")
      imgwriter = vtkPNGWriter::New();
    else if (ext==".jpg")
      imgwriter = vtkJPEGWriter::New();

    // Snapshot image if not null
    if(imgwriter!=NULL) {
      imgwriter->SetInput(image);
      imgwriter->SetFileName(fileName.toStdString().c_str());
      imgwriter->Write();
      return;
    }

    // Video
    vtkGenericMovieWriter *vidwriter = NULL;
#if CLITK_EXPERIMENTAL == 1
    if (ext==".gif") {
      vvAnimatedGIFWriter *gif = vvAnimatedGIFWriter::New();
      vidwriter = gif;

      // FPS
      bool ok;
      int fps = QInputDialog::getInteger(this, tr("Number of frames per second"),
                                     tr("FPS:"), 5, 0, 1000, 1, &ok);
      if(ok)
        gif->SetRate(fps);

      // Loops
      int loops = QInputDialog::getInteger(this, tr("Loops"),
                                     tr("Number of loops (0 means infinite):"), 0, 0, 1000000000, 1, &ok);
      if(ok)
        gif->SetLoops(loops);

      // Dithering
      QString msg = "Would you like to activate dithering?";
      QMessageBox msgBox(QMessageBox::Question, tr("Dithering"),msg, 0, this);
      msgBox.addButton(tr("Yes"), QMessageBox::AcceptRole);
      msgBox.addButton(tr("No"), QMessageBox::RejectRole);
      gif->SetDither(msgBox.exec() == QMessageBox::AcceptRole);
    }
#endif
#ifdef VTK_USE_VIDEO_FOR_WINDOWS
    if (ext==".avi") {
      vtkAVIWriter *mpg = vtkAVIWriter::New();
      vidwriter = mpg;
      mpg->SetQuality(2);
      bool ok;
      int fps = QInputDialog::getInteger(this, tr("Number of frames per second"),
                                     tr("FPS:"), 5, 0, 1024, 1, &ok);
      if(!ok)
        fps = 5;
      mpg->SetRate(fps);
    }
#endif
#ifdef VTK_USE_FFMPEG_ENCODER
    if (ext==".avi") {
      vtkFFMPEGWriter *mpg = vtkFFMPEGWriter::New();
      vidwriter = mpg;
      mpg->SetQuality(2);
      bool ok;
      int fps = QInputDialog::getInteger(this, tr("Number of frames per second"),
                                     tr("FPS:"), 5, 0, 1024, 1, &ok);
      if(!ok)
        fps = 5;
      mpg->SetRate(fps);
      mpg->SetBitRateTolerance(int(ceil(12.0*1024*1024/fps)));
    }
#endif
#ifdef VTK_USE_MPEG2_ENCODER
    if (ext==".mpg") {
      vtkMPEG2Writer *mpg = vtkMPEG2Writer::New();
      vidwriter = mpg;
    }
#endif

    // Take video if not null
    if(vidwriter!=NULL){
      vidwriter->SetInput(image);
      vidwriter->SetFileName(fileName.toStdString().c_str());
      vidwriter->Start();
      int nSlice = mSlicerManagers[smIndex]->GetSlicer(0)->GetTMax();
      for(int i=0; i<=nSlice; i++) {
        mSlicerManagers[smIndex]->SetNextTSlice(0);
        vtkSmartPointer<vtkWindowToImageFilter> w2i = vtkSmartPointer<vtkWindowToImageFilter>::New();
        w2i->SetInput(widget->GetRenderWindow());
        w2i->Update();
        vidwriter->SetInput(w2i->GetOutput());
        vidwriter->Write();
      }
      vidwriter->End();
      vidwriter->Delete();
      return;
    }

    QMessageBox::information(this,tr("Problem saving screenshot !"),tr("Cannot save image.\nPlease set a file extension !!!"));
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::GoToCursor()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  for (int column = 1; column < 5; column++) {
    if (DataTree->selectedItems()[0]->data(column,Qt::CheckStateRole).toInt() > 1) {
      double* cursorPos = mSlicerManagers[index]->GetSlicer(column-1)->GetCursorPosition();
      mSlicerManagers[index]->GetSlicer(column-1)->SetCurrentPosition(
        cursorPos[0],cursorPos[1],cursorPos[2],cursorPos[3]);
      mSlicerManagers[index]->UpdateViews(1,column-1);
      mSlicerManagers[index]->UpdateLinked(column-1);
      break;
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::PlayPause()
{
  if (playMode) {
    playMode = 0;
    playButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/player_play.png")));
    ImageInfoChanged();
    return;
  } else {
    int image_number=DataTree->topLevelItemCount();
    bool has_temporal;
    for (int i=0; i<image_number; i++)
      if (mSlicerManagers[i]->GetSlicer(0)->GetTMax() > 0) {
        has_temporal=true;
        break;
      }
    if (has_temporal) {
      playMode = 1;
      playButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/player_pause.png")));
      QTimer::singleShot(1000/mFrameRate, this, SLOT(PlayNext()));
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::PlayNext()
{
  if (playMode && !this->isHidden()) {
    int image_number=DataTree->topLevelItemCount();
    ///Only play one slicer per SM, and only if the SM is being displayed
    for (int i=0; i<image_number; i++)
      for (int j=0; j<4; j++)
        if (mSlicerManagers[i]->GetSlicer(0)->GetTMax() > 0 &&
            DataTree->topLevelItem(i)->data(j+1,Qt::CheckStateRole).toInt() > 0) {
          mSlicerManagers[i]->SetNextTSlice(j);
          break;
        }
    QTimer::singleShot(1000/mFrameRate, this, SLOT(PlayNext()));
  }
}
//------------------------------------------------------------------------------

void vvMainWindow::ShowLastImage()
{
  if (mSlicerManagers.size() > 1) {
    QTreeWidgetItem * item=DataTree->topLevelItem(DataTree->topLevelItemCount()-1);
    CurrentImageChanged(mSlicerManagers.back()->GetId()); //select new image
    item->setData(1,Qt::CheckStateRole,2); //show the new image in the first panel
    DisplayChanged(item,1);
  }
}

//------------------------------------------------------------------------------
void vvMainWindow::UpdateRenderWindows()
{
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
    mSlicerManagers[i]->GetSlicer(0)->UpdateLandmarks();
    mSlicerManagers[i]->GetSlicer(1)->UpdateLandmarks();
    mSlicerManagers[i]->GetSlicer(2)->UpdateLandmarks();
    mSlicerManagers[i]->GetSlicer(3)->UpdateLandmarks();
  }
  if (NOViewWidget->GetRenderWindow()) NOViewWidget->GetRenderWindow()->Render();
  if (NEViewWidget->GetRenderWindow()) NEViewWidget->GetRenderWindow()->Render();
  if (SOViewWidget->GetRenderWindow()) SOViewWidget->GetRenderWindow()->Render();
  if (SEViewWidget->GetRenderWindow()) SEViewWidget->GetRenderWindow()->Render();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SegmentationOnCurrentImage()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);

  vvSegmentationDialog segmentation;
  segmentation.SetImage(mSlicerManagers[index]->GetSlicer(0)->GetImage());
  segmentation.exec();
}
//------------------------------------------------------------------------------

void vvMainWindow::SurfaceViewerLaunch()
{
  vvSurfaceViewerDialog surfaceViewer;
  surfaceViewer.exec();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int vvMainWindow::GetImageDuplicateFilenameNumber(std::string filename)
{
  int number=0;
  for(unsigned int l=0; l<mSlicerManagers.size(); l++) {
    vvSlicerManager * v = mSlicerManagers[l];
    if (v->GetBaseFileName() ==
        vtksys::SystemTools::GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(filename))) {
      number = std::max(number, v->GetBaseFileNameNumber()+1);
    }
  }
  return number;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvSlicerManager* vvMainWindow::AddImage(vvImage::Pointer image,std::string filename)
{
  // Change filename if another image exist with the same name
  int number = GetImageDuplicateFilenameNumber(filename);

  // Create new SliceManager
  vvSlicerManager* slicer_manager = new vvSlicerManager(4);
  slicer_manager->SetImage(image);//, IMAGE, number);
  //  filename = filename+"_"+clitk::toString(number);
  slicer_manager->SetFilename(filename, number);
  mSlicerManagers.push_back(slicer_manager);

  //create an item in the tree with good settings
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setData(0,Qt::UserRole,slicer_manager->GetFileName().c_str());//files[i].c_str());
  item->setData(1,Qt::UserRole,tr("image"));
  item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,slicer_manager->GetFileName().c_str());//filename.c_str());
  qApp->processEvents();

  for (int j = 1; j <= 4; j++) item->setData(j,Qt::CheckStateRole,1);

  //Create the buttons for reload and close
  qApp->processEvents();
  QTreePushButton* cButton = new QTreePushButton;
  cButton->setItem(item);
  cButton->setColumn(COLUMN_CLOSE_IMAGE);
  cButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/exit.png")));
  connect(cButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
          this,SLOT(CloseImage(QTreeWidgetItem*, int)));

  QTreePushButton* rButton = new QTreePushButton;
  rButton->setItem(item);
  rButton->setColumn(COLUMN_RELOAD_IMAGE);
  rButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/rotateright.png")));
  rButton->setEnabled(0);
  connect(rButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
          this,SLOT(ReloadImage(QTreeWidgetItem*, int)));

  DataTree->addTopLevelItem(item);
  DataTree->setItemWidget(item, COLUMN_CLOSE_IMAGE, cButton);
  DataTree->setItemWidget(item, COLUMN_RELOAD_IMAGE, rButton);

  //set the id of the image
  QString id = QDir::current().absoluteFilePath(slicer_manager->GetFileName().c_str()) + QString::number(mSlicerManagers.size()-1);
  item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
  mSlicerManagers.back()->SetId(id.toStdString());

  linkPanel->addImage(slicer_manager->GetFileName().c_str()// filename
                      , id.toStdString());

  connect(mSlicerManagers.back(), SIGNAL(currentImageChanged(std::string)),
          this, SLOT(CurrentImageChanged(std::string)));
  connect(mSlicerManagers.back(), SIGNAL(currentPickedImageChanged(std::string)),
          this, SLOT(CurrentPickedImageChanged(std::string)));
  connect(mSlicerManagers.back(), SIGNAL(UpdatePosition(int, double, double, double, double, double, double, double)),
          this, SLOT(MousePositionChanged(int,double, double, double, double, double, double, double)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateVector(int, double, double, double, double)),
          this, SLOT(VectorChanged(int,double,double,double, double)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateOverlay(int, double, double)),
          this, SLOT(OverlayChanged(int,double,double)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateFusion(int, double)),
          this, SLOT(FusionChanged(int,double)));
  connect(mSlicerManagers.back(), SIGNAL(WindowLevelChanged()),
          this,SLOT(WindowLevelChanged()));
  connect(mSlicerManagers.back(), SIGNAL(UpdateSlice(int,int)),
          this,SLOT(UpdateSlice(int,int)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateTSlice(int, int)),
          this,SLOT(UpdateTSlice(int, int)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateTSlice(int, int)),
          this,SLOT(ImageInfoChanged()));
  connect(mSlicerManagers.back(), SIGNAL(UpdateSliceRange(int,int,int,int,int)),
          this,SLOT(UpdateSliceRange(int,int,int,int,int)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateLinkManager(std::string,int,double,double,double,int)),
          this,SLOT(UpdateLinkManager(std::string,int,double,double,double,int)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateLinkedNavigation(std::string,vvSlicerManager*,vvSlicer*)),
          this,SLOT(UpdateLinkedNavigation(std::string,vvSlicerManager*,vvSlicer*)));
  connect(mSlicerManagers.back(), SIGNAL(ChangeImageWithIndexOffset(vvSlicerManager*,int,int)),
          this,SLOT(ChangeImageWithIndexOffset(vvSlicerManager*,int,int)));
  connect(mSlicerManagers.back(), SIGNAL(LandmarkAdded()),landmarksPanel,SLOT(AddPoint()));
  UpdateTree();
  qApp->processEvents();
  InitSlicers();
  ShowLastImage();
  InitDisplay();
  qApp->processEvents();

  // End
  ImageInfoChanged();
  return slicer_manager;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::UpdateCurrentSlicer()
{
  int index = -1;
  if (DataTree->selectedItems().size() > 0) {
    index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  }
  mSlicerManagerCurrentIndex = index;
}
//------------------------------------------------------------------------------

