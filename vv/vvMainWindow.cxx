/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

#include <algorithm>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include "QTreePushButton.h"

// VV include
#include "vvMainWindow.h"
#include "vvHelpDialog.h"
#include "vvDocumentation.h"
#include "vvProgressDialog.h"
#include "vvQDicomSeriesSelector.h"
#include "vvSlicerManager.h"
#include "clitkImageCommon.h"
#include "vvSlicer.h"
#include "vvInteractorStyleNavigator.h"
#include "vvImageWriter.h"
#include "vvResamplerDialog.h"
#include "vvSegmentationDialog.h"
#include "vvSurfaceViewerDialog.h"
#include "vvDeformationDialog.h"
#include "vvImageWarp.h"
#include "vvUtils.h"
#include "vvMaximumIntensityProjection.h"
#include "vvMidPosition.h"
#include "vvMesh.h"
#include "vvStructSelector.h"
#include "vvMeshReader.h"
#include "vvConstants.h"

#ifdef CLITK_VV_USE_BDCM
#include <bdcmDicomFilesSelectorDialog.h>
#endif

// ITK include
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkByteSwapper.h"
#include "itkCommand.h"
#include "itkNumericSeriesFileNames.h"

// VTK include
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkCornerAnnotation.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkPNMWriter.h"
#include "vtkPNGWriter.h"
#include "vtkJPEGWriter.h"

// Standard includes
#include <iostream>

#define COLUMN_TREE 0
#define COLUMN_UL_VIEW 1
#define COLUMN_UR_VIEW 2
#define COLUMN_DL_VIEW 3
#define COLUMN_DR_VIEW 4
#define COLUMN_CLOSE_IMAGE 5
#define COLUMN_RELOAD_IMAGE 6
#define COLUMN_IMAGE_NAME 7

#define EXTENSIONS "Images ( *.bmp *.png *.jpeg *.jpg *.tif *.mhd *.hdr *.vox *.his *.xdr)"

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
vvMainWindow::vvMainWindow():vvMainWindowBase() {
  setupUi(this); // this sets up the GUI

  mInputPathName = "";
  mMenuTools = menuTools;
  mContextMenu = &contextMenu;
  mMenuExperimentalTools = menuExperimental;
  mMainWidget = this;

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

  contextMenu.addSeparator();

  // QAction* actionCrop_image = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/crop.png")),
  //                                                   tr("Crop Current Image"));
  // connect(actionCrop_image,SIGNAL(triggered()),this,SLOT(CropImage()));
  // contextActions.push_back(actionCrop_image);

  QAction* actionSplit_image = contextMenu.addAction(QIcon(QString::fromUtf8(":/common/icons/cut.png")),
                                                     tr("Split Current Image"));
  connect(actionSplit_image,SIGNAL(triggered()),this,SLOT(SplitImage()));
  contextActions.push_back(actionSplit_image);

  contextMenu.addSeparator();

  contextMenu.addAction(actionAdd_VF_to_current_Image);
  contextActions.push_back(actionAdd_VF_to_current_Image);

  QAction* actionAdd_Overlay_to_current_Image = menuOverlay->addAction(QIcon(QString::fromUtf8(":/common/icons/GPSup.png")),
                                                                       tr("Add overlay image to current image"));
  contextMenu.addAction(actionAdd_Overlay_to_current_Image);
  contextActions.push_back(actionAdd_Overlay_to_current_Image);

  connect(actionAdd_Overlay_to_current_Image,SIGNAL(triggered()), this,SLOT(SelectOverlayImage()));

  contextMenu.addAction(actionAdd_fusion_image);
  connect(actionAdd_fusion_image,SIGNAL(triggered()),this,SLOT(AddFusionImage()));
  contextActions.push_back(actionAdd_fusion_image);

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
  actionAdd_Overlay_to_current_Image->setEnabled(0);
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
  connect(actionMaximum_Intensity_Projection,SIGNAL(triggered()),this,SLOT(ComputeMIP()));
  connect(actionCompute_mid_position_image,SIGNAL(triggered()),this,SLOT(ComputeMidPosition()));
  connect(actionDeformable_Registration,SIGNAL(triggered()),this,SLOT(ComputeDeformableRegistration()));
  connect(actionWarp_image_with_vector_field,SIGNAL(triggered()),this,SLOT(WarpImage()));
  connect(actionLoad_images,SIGNAL(triggered()),this,SLOT(OpenImages()));
  connect(actionOpen_Dicom,SIGNAL(triggered()),this,SLOT(OpenDicom()));
  connect(actionOpen_Dicom_Struct,SIGNAL(triggered()),this,SLOT(OpenDCStructContour()));
  connect(actionOpen_VTK_contour,SIGNAL(triggered()),this,SLOT(OpenVTKContour()));
  connect(actionOpen_Multiple_Images_As_One,SIGNAL(triggered()),this,SLOT(MergeImages()));
  connect(actionOpen_Image_With_Time,SIGNAL(triggered()),this,SLOT(OpenImageWithTime()));
  connect(actionMerge_images_as_n_dim_t, SIGNAL(triggered()), this, SLOT(MergeImagesWithTime()));
  connect(actionSave_As,SIGNAL(triggered()),this,SLOT(SaveAs()));
  connect(actionExit,SIGNAL(triggered()),this,SLOT(close()));
  connect(actionAdd_VF_to_current_Image,SIGNAL(triggered()),this,SLOT(OpenField()));
  connect(actionNavigation_Help,SIGNAL(triggered()),this,SLOT(ShowHelpDialog()));
  connect(actionDocumentation,SIGNAL(triggered()),this,SLOT(ShowDocumentation()));

  ///////////////////////////////////////////////
  contextMenu.addAction(actionResampler);
  connect(actionResampler,SIGNAL(triggered()),this,SLOT(ResampleCurrentImage()));
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

  connect(DataTree,SIGNAL(itemSelectionChanged()),this,SLOT(ImageInfoChanged()));
  connect(DataTree,SIGNAL(itemClicked(QTreeWidgetItem*, int)),this,
          SLOT(DisplayChanged(QTreeWidgetItem*, int)));

  connect(viewButton,SIGNAL(clicked()),this, SLOT(ChangeViewMode()) );
  connect(windowSpinBox,SIGNAL(editingFinished()),this,SLOT(WindowLevelEdited()));
  connect(levelSpinBox,SIGNAL(editingFinished()),this,SLOT(WindowLevelEdited()));
  connect(colorMapComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateColorMap()));
  connect(presetComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateWindowLevel()));
  connect(inverseButton,SIGNAL(clicked()),this,SLOT(SwitchWindowLevel()));


  connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowContextMenu(QPoint)));

  connect(linkPanel,SIGNAL(addLink(QString,QString)),this,SLOT(AddLink(QString,QString)));
  connect(linkPanel,SIGNAL(removeLink(QString,QString)),this,SLOT(RemoveLink(QString,QString)));
  connect(overlayPanel,SIGNAL(VFPropertyUpdated(int,int,int)),this,SLOT(SetVFProperty(int,int,int)));
  connect(overlayPanel,SIGNAL(OverlayPropertyUpdated(int)),this,SLOT(SetOverlayProperty(int)));
  connect(overlayPanel,SIGNAL(FusionPropertyUpdated(int,int,double,double)),
          this,SLOT(SetFusionProperty(int,int,double,double)));
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
  if ( !recent_files.empty() )
    {
      QMenu * rmenu = new QMenu("Recently opened files...");
      rmenu->setIcon(QIcon(QString::fromUtf8(":/common/icons/open.png")));
      menuFile->insertMenu(actionOpen_Image_With_Time,rmenu);
      for (std::list<std::string>::iterator i = recent_files.begin();i!=recent_files.end();i++)
        {
          QAction* current=new QAction(QIcon(QString::fromUtf8(":/common/icons/open.png")),
                                       (*i).c_str(),this);
          rmenu->addAction(current);
          connect(current,SIGNAL(triggered()),this,SLOT(OpenRecentImage()));
        }
    }

  // Adding all new tools (insertion in the menu)
  vvToolManager::GetInstance()->InsertToolsInMenu(this);

  if (!CLITK_EXPERIMENTAL)
      menuExperimental->menuAction()->setVisible(false);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::ComputeMIP()
{
  vvMaximumIntensityProjection mip;
  vvSlicerManager* selected_slicer = mSlicerManagers[GetSlicerIndexFromItem(DataTree->selectedItems()[0])];
  QFileInfo info(selected_slicer->GetFileName().c_str());
  mip.Compute(selected_slicer);
  if (!mip.error)
      AddImage(mip.GetOutput(),info.path().toStdString()+"/"+info.completeBaseName().toStdString()+"_mip.mhd");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::ComputeMidPosition()
{
  bool ok;
  int index=GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  int ref = QInputDialog::getInteger(this,"Chose reference phase","Reference phase",0,0,\
                                     mSlicerManagers[index]->GetImage()->GetVTKImages().size()-1,1,&ok);
  if (ok)
    {
      vvMidPosition midp;
      midp.slicer_manager = mSlicerManagers[index];
      midp.reference_image_index = ref;
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      midp.Update();
      if (midp.error)
        QMessageBox::warning(this, "Error computing midposition image",midp.error_message.c_str());
      else
        {
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
  if (mSlicerManagers.size() > 0)
    {
      QString Extensions = "Images ( *.vtk; *.obj)";
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
void vvMainWindow::OpenDCStructContour()
{
  if (mSlicerManagers.size() > 0)
    {
      QString Extensions = "Dicom Files ( *.dcm; RS*)";
      Extensions += ";;All Files (*)";
      QString file = QFileDialog::getOpenFileName(this,tr("Merge Images"),mInputPathName,Extensions);
      if (file.isNull())
        return;
      int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
      vvMeshReader reader;
      reader.SetFilename(file.toStdString());
      vvStructSelector selector;
      selector.SetStructures(reader.GetROINames());
      if (!mSlicerManagers[index]->GetVF().IsNull())
        selector.EnablePropagationCheckBox();
      if (selector.exec())
        {
          QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
          reader.SetSelectedItems(selector.getSelectedItems());
          reader.SetImage(mSlicerManagers[index]->GetImage());
          if (selector.PropagationEnabled())
            reader.SetPropagationVF(mSlicerManagers[index]->GetVF());
          reader.Update();
          std::vector<vvMesh::Pointer> contours=reader.GetOutput();
          for (std::vector<vvMesh::Pointer>::iterator i=contours.begin();
               i!=contours.end();i++)
            AddContour(index,*i,selector.PropagationEnabled());
          QApplication::restoreOverrideCursor();
        }
    }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::ComputeDeformableRegistration()
{
  if (mSlicerManagers.size() > 0)
    {
      int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
      vvDeformationDialog dialog(index,mSlicerManagers);
      if (dialog.exec())
        {
          std::string base_name=itksys::SystemTools::GetFilenameWithoutExtension(mSlicerManagers[dialog.GetInputFileIndex()]->GetFileName());
          AddField(dialog.GetOutput(),dialog.getFieldFile(),dialog.GetInputFileIndex());
          WarpImage(dialog.GetSelectedSlicer(),dialog.GetReferenceFrameIndex());
        }
      else
        std::cout << "Error or user cancellation while computing deformation field..." << std::endl;
    }
  else QMessageBox::information(this, "Need to open image","You must open an image first.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::WarpImage()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if (!mSlicerManagers[index]->GetVF().IsNull())
    {
      bool ok;
      int ref = QInputDialog::getInteger(this,"Chose reference phase","Reference phase",0,0,\
                                         mSlicerManagers[index]->GetImage()->GetVTKImages().size()-1,1,&ok);
      if (ok)
        {
          WarpImage(mSlicerManagers[index],ref);
        }
    }
  else
    QMessageBox::warning(this,tr("No vector field"),tr("Sorry, can't warp without a vector field"));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::WarpImage(vvSlicerManager* selected_slicer,int reference_phase)
{
  if (!selected_slicer->GetVF().IsNull())
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      QFileInfo info(selected_slicer->GetFileName().c_str());
      vvImageWarp warp(selected_slicer->GetImage(),selected_slicer->GetVF(),
                       reference_phase,this);
      if (warp.ComputeWarpedImage())
        {
          AddImage(warp.GetWarpedImage(),info.path().toStdString()+"/"+info.completeBaseName().toStdString()+"_warped.mhd");
          AddImage(warp.GetDiffImage()  ,info.path().toStdString()+"/"+info.completeBaseName().toStdString()+"_diff.mhd");
          AddImage(warp.GetJacobianImage()  ,info.path().toStdString()+"/"+info.completeBaseName().toStdString()+"_jacobian.mhd");
          QApplication::restoreOverrideCursor();
        }
      else
        {
          QApplication::restoreOverrideCursor();
          QMessageBox::warning(this,tr("Different spacings"),tr("The vector field and image spacings must be the same in order to warp."));
        }
    }
  else
    QMessageBox::warning(this,tr("No vector field"),tr("Sorry, can't warp without a vector field."));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvMainWindow::~vvMainWindow() {
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++)
    {
      if (mSlicerManagers[i] != NULL)
        delete mSlicerManagers[i];
    }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::MergeImages() {
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

  for (int i = 0; i < files.size(); i++)
    {
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
          for (unsigned int j = 0;j < currentDim; j++)
          {
              if (i == 0)
              {
                  if (j == 0)
                  {
                      currentSpacing.resize(currentDim);
                      currentSize.resize(currentDim);
                      currentOrigin.resize(currentDim);
                  }
                  currentOrigin[j] = reader->GetOrigin(j);
                  currentSpacing[j] = reader->GetSpacing(j);
                  currentSize[j] = reader->GetDimensions(j);
              }
              else if (currentDim != reader->GetNumberOfDimensions()
                      || currentSpacing[j] != reader->GetSpacing(j)
                      || currentSize[j] != (int)reader->GetDimensions(j)
                      || currentOrigin[j] != reader->GetOrigin(j))
              {
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
    LoadImages(vector, MERGED);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::MergeImagesWithTime() {
  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";
  QStringList files = QFileDialog::getOpenFileNames(this,tr("Merge Images With Time"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;
  mInputPathName = itksys::SystemTools::GetFilenamePath(files[0].toStdString()).c_str();
  std::vector<std::string> vector;

  unsigned int currentDim = 0;
  std::vector<double> currentSpacing;
  std::vector<int> currentSize;
  std::vector<double> currentOrigin;

  for (int i = 0; i < files.size(); i++)
    {
      itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(
                                                                            files[i].toStdString().c_str(), itk::ImageIOFactory::ReadMode);
      if (reader)
        {
          reader->SetFileName(files[i].toStdString().c_str());
          reader->ReadImageInformation();
          if (i == 0)
            currentDim = reader->GetNumberOfDimensions();
          bool IsOk = true;
          for (unsigned int j = 0;j < currentDim; j++)
            {
              if (i == 0)
                {
                  if (j == 0)
                    {
                      currentSpacing.resize(currentDim);
                      currentSize.resize(currentDim);
                      currentOrigin.resize(currentDim);
                    }
                  currentOrigin[j] = reader->GetOrigin(j);
                  currentSpacing[j] = reader->GetSpacing(j);
                  currentSize[j] = reader->GetDimensions(j);
                }
              else if (currentDim != reader->GetNumberOfDimensions()
                       || currentSpacing[j] != reader->GetSpacing(j)
                       || currentSize[j] != (int)reader->GetDimensions(j)
                       || currentOrigin[j] != reader->GetOrigin(j))
                {
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
      else
        {
          QString error = "Cannot read file info for ";
          error += files[i].toStdString().c_str();
          error += "\n";
          error += "Maybe you're trying to open an image in an unsupported format?\n";
          QMessageBox::information(this,tr("Reading problem"),error);
        }
    }
  sort(vector.begin(),vector.end());
  if (vector.size() > 1)
    LoadImages(vector, MERGEDWITHTIME);
  else
    QMessageBox::warning(this,tr("Reading problem"),"You need to select at least two images to merge images with time.\nIf you only want to open one image, please use the \"Open Image\" function.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::OpenDicom() {
  std::vector<std::string> files;

#ifdef CLITK_VV_USE_BDCM
  bool r = bdcm::OpenDicomFilesSelectorDialog(files,
                                              "DicomFilesSelectorDialog test",
                                              0,0,800,800,1);

  if (r) {
    std::cout << "$$$$ main : user clicked 'OK' $$$$"<<std::endl;
    std::cout << "$$$$ selected files : "<<std::endl;
    std::vector<std::string>::iterator i;
    for (i=files.begin();i!=files.end();++i) {
      std::cout << *i << std::endl;
    }
    std::cout << "$$$$ "<<std::endl;

    LoadImages(files,DICOM);
  }
  else {
    std::cout << "$$$$ main : user clicked 'CANCEL' $$$$"<<std::endl;
  }
#else
  std::cout << "dicomSeriesSelector " << std::endl;
  if (dicomSeriesSelector->exec() == QDialog::Accepted) {
    files = *(dicomSeriesSelector->GetFilenames());
    LoadImages(files,DICOM);
  }
#endif

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::OpenImages() {
  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";

  QStringList files = QFileDialog::getOpenFileNames(this,tr("Load Images"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;
  mInputPathName = itksys::SystemTools::GetFilenamePath(files[0].toStdString()).c_str();
  std::vector<std::string> vector;
  for (int i = 0; i < files.size(); i++)
    vector.push_back(files[i].toStdString());
  LoadImages(vector, IMAGE);
}
//------------------------------------------------------------------------------
void vvMainWindow::OpenRecentImage()
{
  QAction * caller = qobject_cast<QAction*>(sender());
  std::vector<std::string> images;
  images.push_back(caller->text().toStdString());
  mInputPathName = itksys::SystemTools::GetFilenamePath(images[0]).c_str();
  LoadImages(images,IMAGE);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::OpenImageWithTime() {
  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";

  QStringList files = QFileDialog::getOpenFileNames(this,tr("Load Images With Time"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;
  mInputPathName = itksys::SystemTools::GetFilenamePath(files[0].toStdString()).c_str();
  std::vector<std::string> vector;
  for (int i = 0; i < files.size(); i++)
    {
      vector.push_back(files[i].toStdString());
    }
  LoadImages(vector, IMAGEWITHTIME);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::LoadImages(std::vector<std::string> files, LoadedImageType filetype) {
  //Separate the way to open images and dicoms
  int fileSize;
  if (filetype == IMAGE || filetype == IMAGEWITHTIME)
    fileSize = files.size();
  else
    fileSize = 1;

  //Only add to the list of recently opened files when a single file is opened,
  //to avoid polluting the list of recently opened files
  if (files.size() == 1)
    {
      QFileInfo finfo=tr(files[0].c_str());
      AddToRecentlyOpenedImages(finfo.absoluteFilePath().toStdString());
    }
  //init the progress events
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vvProgressDialog progress("Opening " + files[0],fileSize>1);
  qApp->processEvents();

  int numberofsuccesulreads=0;
  //open images as 1 or multiples
  for (int i = 0; i < fileSize; i++) {

    progress.Update("Opening " + files[i]);
    progress.SetProgress(i,fileSize);
    qApp->processEvents();

    //read the image and put it in mSlicerManagers
    vvSlicerManager* imageManager = new vvSlicerManager(4);
    qApp->processEvents();

    bool SetImageSucceed=false;

    // Change filename if an image with the same already exist
    //    DD(files[i]);
    int number=0;
    for(unsigned int l=0; l<mSlicerManagers.size(); l++) {
      vvSlicerManager * v = mSlicerManagers[l];
      //      DD(v->GetBaseFileName());
      // DD(v->GetFileName());
      if (v->GetBaseFileName() == 
          vtksys::SystemTools::GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(files[i]))) {
        number = std::max(number, v->GetBaseFileNameNumber()+1);
      }
    }


    if (filetype == IMAGE || filetype == IMAGEWITHTIME)
      SetImageSucceed = imageManager->SetImage(files[i],filetype, number);
    else
      {
        SetImageSucceed = imageManager->SetImages(files,filetype, number);
      }
    if (SetImageSucceed == false)
      {
        QApplication::restoreOverrideCursor();
        QString error = "Cannot open file \n";
        error += imageManager->GetLastError().c_str();
        QMessageBox::information(this,tr("Reading problem"),error);
        delete imageManager;
      }
    else
      {
        mSlicerManagers.push_back(imageManager);

        //create an item in the tree with good settings
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setData(0,Qt::UserRole,files[i].c_str());
        QFileInfo fileinfo(imageManager->GetFileName().c_str()); //Do not show the path
        item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,fileinfo.fileName());
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
        QString id = files[i].c_str() + QString::number(mSlicerManagers.size()-1);
        item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
        mSlicerManagers.back()->SetId(id.toStdString());

        linkPanel->addImage(imageManager->GetFileName(), id.toStdString());

        connect(mSlicerManagers.back(), SIGNAL(currentImageChanged(std::string)),
                this,SLOT(CurrentImageChanged(std::string)));
        connect(mSlicerManagers.back(), SIGNAL(UpdatePosition(int, double, double, double, double, double, double, double)),
             this,SLOT(MousePositionChanged(int,double, double, double, double, double, double, double)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateVector(int, double, double, double, double)),
                this, SLOT(VectorChanged(int,double,double,double, double)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateOverlay(int, double, double)),
                this, SLOT(OverlayChanged(int,double,double)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateFusion(int, double)),
                this, SLOT(FusionChanged(int,double)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateWindows(int, int, int)),
                this,SLOT(WindowsChanged(int, int, int)));
        connect(mSlicerManagers.back(), SIGNAL(WindowLevelChanged(double, double,int, int)),
                this,SLOT(WindowLevelChanged(double, double, int, int)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateSlice(int,int)),
                this,SLOT(UpdateSlice(int,int)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateTSlice(int, int)),
                this,SLOT(UpdateTSlice(int, int)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateSliceRange(int,int,int,int,int)),
                this,SLOT(UpdateSliceRange(int,int,int,int,int)));
        connect(mSlicerManagers.back(), SIGNAL(UpdateLinkManager(std::string,int,double,double,double,int)),
                this,SLOT(UpdateLinkManager(std::string,int,double,double,double,int)));
        connect(mSlicerManagers.back(), SIGNAL(ChangeImageWithIndexOffset(vvSlicerManager*,int,int)),
                this,SLOT(ChangeImageWithIndexOffset(vvSlicerManager*,int,int)));
        connect(mSlicerManagers.back(),SIGNAL(LandmarkAdded()),landmarksPanel,SLOT(AddPoint()));
        InitSlicers();
        numberofsuccesulreads++;
      }
  }
  if (numberofsuccesulreads)
    {
      NOViewWidget->show();
      NEViewWidget->show();
      SOViewWidget->show();
      SEViewWidget->show();
      UpdateTree();
      InitDisplay();
      ShowLastImage();
    }
  QApplication::restoreOverrideCursor();

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateTree() {
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
void vvMainWindow::CurrentImageChanged(std::string id) {
  int selected = 0;
  for (int i = 0; i < DataTree->topLevelItemCount(); i++)
    {
      if (DataTree->topLevelItem(i)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString().toStdString() == id)
        {
          selected = i;
        }
      else
        {
          DataTree->topLevelItem(i)->setSelected(0);
        }
      for (int child = 0; child < DataTree->topLevelItem(i)->childCount();child++)
        DataTree->topLevelItem(i)->child(child)->setSelected(0);

    }
  DataTree->topLevelItem(selected)->setSelected(1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ImageInfoChanged() {
  contextActions[7]->setEnabled(1);
  contextActions[6]->setEnabled(1);
  actionSave_As->setEnabled(1);
  actionAdd_VF_to_current_Image->setEnabled(1);
  actionAdd_fusion_image->setEnabled(1);
  actionNorth_East_Window->setEnabled(1);
  actionNorth_West_Window->setEnabled(1);
  actionSouth_East_Window->setEnabled(1);
  actionSouth_West_Window->setEnabled(1);
  inverseButton->setEnabled(1);

  goToCursorPushButton->setEnabled(1);

  if (DataTree->selectedItems().size()) {
    int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);

    colorMapComboBox->setEnabled(1);
    for (int i = 0; i < DataTree->topLevelItem(index)->childCount();i++)
      {
        if (DataTree->topLevelItem(index)->child(i)->data(1,Qt::UserRole).toString() == "overlay" ||
            DataTree->topLevelItem(index)->child(i)->data(1,Qt::UserRole).toString() == "fusion")
          {
            colorMapComboBox->setEnabled(0);
            break;
          }
      }

    std::vector<double> origin;
    std::vector<double> inputSpacing;
    std::vector<int> inputSize;
    std::vector<double> sizeMM;
    int dimension=0;
    QString pixelType;
    QString inputSizeInBytes;
    QString image = DataTree->selectedItems()[0]->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();

    if (mSlicerManagers[index]->GetSlicer(0)->GetImage()->GetVTKImages().size() > 1 || playMode == 1)
      {
        playButton->setEnabled(1);
        frameRateLabel->setEnabled(1);
        frameRateSpinBox->setEnabled(1);
      }
    else
      {
        playButton->setEnabled(0);
        frameRateLabel->setEnabled(0);
        frameRateSpinBox->setEnabled(0);
      }

    //read image header
    int NPixel = 1;

    if (DataTree->topLevelItem(index) == DataTree->selectedItems()[0])
      {
        vvImage::Pointer imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetImage();
        dimension = imageSelected->GetNumberOfDimensions();
        origin.resize(dimension);
        inputSpacing.resize(dimension);
        inputSize.resize(dimension);
        sizeMM.resize(dimension);
        pixelType = mSlicerManagers[index]->GetImage()->GetScalarTypeAsString().c_str();
        for (int i = 0;i < dimension;i++)
          {
            origin[i] = imageSelected->GetOrigin()[i];
            inputSpacing[i] = imageSelected->GetSpacing()[i];
            inputSize[i] = imageSelected->GetSize()[i];
            sizeMM[i] = inputSize[i]*inputSpacing[i];
            NPixel *= inputSize[i];
          }
        inputSizeInBytes = GetSizeInBytes(imageSelected->GetActualMemorySize()*1000);
      }
    else if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "vector")
      {
        vvImage::Pointer imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetVF();
        dimension = imageSelected->GetNumberOfDimensions();
        origin.resize(dimension);
        inputSpacing.resize(dimension);
        inputSize.resize(dimension);
        sizeMM.resize(dimension);
        pixelType = mSlicerManagers[index]->GetVF()->GetScalarTypeAsString().c_str();
        for (int i = 0;i < dimension;i++)
          {
            origin[i] = imageSelected->GetOrigin()[i];
            inputSpacing[i] = imageSelected->GetSpacing()[i];
            inputSize[i] = imageSelected->GetSize()[i];
            sizeMM[i] = inputSize[i]*inputSpacing[i];
            NPixel *= inputSize[i];
          }
        inputSizeInBytes = GetSizeInBytes(imageSelected->GetActualMemorySize()*1000);
      }
    else if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "overlay")
      {
        vvImage::Pointer imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetOverlay();
        dimension = imageSelected->GetNumberOfDimensions();
        origin.resize(dimension);
        inputSpacing.resize(dimension);
        inputSize.resize(dimension);
        sizeMM.resize(dimension);
        pixelType = mSlicerManagers[index]->GetImage()->GetScalarTypeAsString().c_str();
        for (int i = 0;i < dimension;i++)
          {
            origin[i] = imageSelected->GetOrigin()[i];
            inputSpacing[i] = imageSelected->GetSpacing()[i];
            inputSize[i] = imageSelected->GetSize()[i];
            sizeMM[i] = inputSize[i]*inputSpacing[i];
            NPixel *= inputSize[i];
          }
        inputSizeInBytes = GetSizeInBytes(imageSelected->GetActualMemorySize()*1000);
      }
    else if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "fusion")
      {
        vvImage::Pointer imageSelected = mSlicerManagers[index]->GetSlicer(0)->GetFusion();
        dimension = imageSelected->GetNumberOfDimensions();
        origin.resize(dimension);
        inputSpacing.resize(dimension);
        inputSize.resize(dimension);
        sizeMM.resize(dimension);
        pixelType = mSlicerManagers[index]->GetImage()->GetScalarTypeAsString().c_str();
        for (int i = 0;i < dimension;i++)
          {
            origin[i] = imageSelected->GetOrigin()[i];
            inputSpacing[i] = imageSelected->GetSpacing()[i];
            inputSize[i] = imageSelected->GetSize()[i];
            sizeMM[i] = inputSize[i]*inputSpacing[i];
            NPixel *= inputSize[i];
          }
        inputSizeInBytes = GetSizeInBytes(imageSelected->GetActualMemorySize()*1000);
      }

    QString dim = QString::number(dimension) + " (";
    dim += pixelType + ")";

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
    for (int i = 0; i < 4;i++)
      {
        if (DataTree->selectedItems()[0]->data(i+1,Qt::CheckStateRole).toInt() > 0 || i == 3)
          {
            mSlicerManagers[index]->UpdateInfoOnCursorPosition(i);
            break;
          }
      }
    windowSpinBox->setValue(mSlicerManagers[index]->GetColorWindow());
    levelSpinBox->setValue(mSlicerManagers[index]->GetColorLevel());
    // DD(mSlicerManagers[index]->GetColorMap());
    // DD(mSlicerManagers[index]->GetPreset());
    presetComboBox->setCurrentIndex(mSlicerManagers[index]->GetPreset());
    colorMapComboBox->setCurrentIndex(mSlicerManagers[index]->GetColorMap());

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
    for (int i = 0; i < 4;i++)
      {
        if (DataTree->selectedItems()[0]->data(i+1,Qt::CheckStateRole).toInt() > 0 || i == 3)
          {
            mSlicerManagers[index]->UpdateInfoOnCursorPosition(i);
            break;
          }
      }
    windowSpinBox->setValue(mSlicerManagers[index]->GetColorWindow());
    levelSpinBox->setValue(mSlicerManagers[index]->GetColorLevel());
    presetComboBox->setCurrentIndex(mSlicerManagers[index]->GetPreset());
    colorMapComboBox->setCurrentIndex(mSlicerManagers[index]->GetColorMap());

    if (mSlicerManagers[index]->GetSlicer(0)->GetVF())
      {
        overlayPanel->getVFName(mSlicerManagers[index]->GetVFName().c_str());
        overlayPanel->getVFProperty(mSlicerManagers[index]->GetSlicer(0)->GetVFSubSampling(),
                                    mSlicerManagers[index]->GetSlicer(0)->GetVFScale(),
                                    mSlicerManagers[index]->GetSlicer(0)->GetVFLog());
      }
    else
      {
        overlayPanel->getVFName(mSlicerManagers[index]->GetVFName().c_str());
        overlayPanel->getVFProperty(-1,-1,-1);
      }
    if (mSlicerManagers[index]->GetSlicer(0)->GetOverlay())
      {
        overlayPanel->getOverlayName(mSlicerManagers[index]->GetOverlayName().c_str());
        overlayPanel->getOverlayProperty(mSlicerManagers[index]->GetOverlayColor());
      }
    else
      {
        overlayPanel->getOverlayName(mSlicerManagers[index]->GetOverlayName().c_str());
        overlayPanel->getOverlayProperty(-1);
      }
    if (mSlicerManagers[index]->GetSlicer(0)->GetFusion())
      {
        overlayPanel->getFusionName(mSlicerManagers[index]->GetFusionName().c_str());
        overlayPanel->getFusionProperty(mSlicerManagers[index]->GetFusionOpacity(),
                                        mSlicerManagers[index]->GetFusionColorMap(),
                                        mSlicerManagers[index]->GetFusionWindow(),
                                        mSlicerManagers[index]->GetFusionLevel());
      }
    else
      {
        overlayPanel->getFusionName(mSlicerManagers[index]->GetFusionName().c_str());
        overlayPanel->getFusionProperty(-1, -1,-1,-1);
      }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ShowDocumentation() {
  documentation->show();
}
//------------------------------------------------------------------------------
void vvMainWindow::ShowHelpDialog() {
  help_dialog->show();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ChangeViewMode() {
  QListIterator<int> it0(splitter_3->sizes());
  QListIterator<int> it1(splitter_3->sizes());
  int max0 = 0;
  int max1 = 1;
  while (it0.hasNext())
    {
      max0 += it0.next();
    }
  while (it1.hasNext())
    {
      max1 += it1.next();
    }
  QList<int> size0;
  QList<int> size1;
  if (viewMode == 1)
    {
      viewMode = 0;
      size0.push_back(max0);
      size0.push_back(0);
      size1.push_back(max1);
      size1.push_back(0);
      splitter_3->setSizes(size0);
      OSplitter->setSizes(size1);
      DataTree->setColumnHidden(2,1);
      DataTree->setColumnHidden(3,1);
      DataTree->setColumnHidden(4,1);
    }
  else
    {
      viewMode = 1;
      size0.push_back(int(max0/2));
      size0.push_back(int(max0/2));
      size1.push_back(int(max1/2));
      size1.push_back(int(max1/2));
      splitter_3->setSizes(size0);
      OSplitter->setSizes(size1);
      DataTree->setColumnHidden(2,0);
      DataTree->setColumnHidden(3,0);
      DataTree->setColumnHidden(4,0);
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString vvMainWindow::GetSizeInBytes(unsigned long size) {
  QString result = "";// QString::number(size);
  //result += " bytes (";
  if (size > 1000000000)
    {
      size /= 1000000000;
      result += QString::number(size);
      result += "Gb";//)";
    }
  else if (size > 1000000)
    {
      size /= 1000000;
      result += QString::number(size);
      result += "Mb";//)";
    }
  else if (size > 1000)
    {
      size /= 1000;
      result += QString::number(size);
      result += "kb";//)";
    }
  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString vvMainWindow::GetVectorDoubleAsString(std::vector<double> vectorDouble) {
  QString result;
  for (unsigned int i= 0; i < vectorDouble.size(); i++)
    {
      if (i != 0)
        result += " ";
      result += QString::number(vectorDouble[i]);
    }
  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString vvMainWindow::GetVectorIntAsString(std::vector<int> vectorInt) {
  QString result;
  for (unsigned int i= 0; i < vectorInt.size(); i++)
    {
      if (i != 0)
        result += " ";
      result += QString::number(vectorInt[i]);
    }
  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int vvMainWindow::GetSlicerIndexFromItem(QTreeWidgetItem* item) {
  QString id = item->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString();
  for (int i = 0; i < DataTree->topLevelItemCount(); i++)
    {
      if (DataTree->topLevelItem(i)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString() == id)
        return i;
    }
  return -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QTreeWidgetItem* vvMainWindow::GetItemFromSlicerManager(vvSlicerManager* sm) {
    QString id = sm->GetId().c_str();
    for (int i = 0; i < DataTree->topLevelItemCount(); i++)
    {
        if (DataTree->topLevelItem(i)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString() == id)
            return DataTree->topLevelItem(i);
    }
    return NULL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::DisplayChanged(QTreeWidgetItem *clicked_item, int column) {
  int index = GetSlicerIndexFromItem(clicked_item);
  if ( column >= COLUMN_CLOSE_IMAGE || column <= 0)
    return;
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++)
    {
     //Trick to avoid redoing twice the job for a key (sr)
      mSlicerManagers[i]->GetSlicer(column-1)->GetRenderWindow()-> GetInteractor()->SetKeySym("Crap");
      
      QTreeWidgetItem* current_row=DataTree->topLevelItem(i);
      if (DataTree->topLevelItem(index) == current_row)
        {
          vvSlicer* clicked_slicer=mSlicerManagers[i]->GetSlicer(column-1);
          if (current_row == clicked_item) 
            {
              //If we just activated a slicer
              if (current_row->data(column,Qt::CheckStateRole).toInt() > 0)
                {
                  mSlicerManagers[i]->UpdateSlicer(column-1,clicked_item->data(column,Qt::CheckStateRole).toInt());
                  mSlicerManagers[i]->UpdateInfoOnCursorPosition(column-1);
                  DisplaySliders(i,column-1);
                  std::map<std::string,int> overlay_counts;
                  for (int child = 0; child < current_row->childCount(); child++)
                    {
                      std::string overlay_type = 
                        current_row->child(child)->data(1,Qt::UserRole).toString().toStdString();
                      overlay_counts[overlay_type]++;
                      current_row->child(child)->setData(column,Qt::CheckStateRole,
                                                         current_row->data(column,Qt::CheckStateRole));
                      clicked_slicer->SetActorVisibility(overlay_type,overlay_counts[overlay_type]-1,true);
                    }
                }
              else //We don't allow simply desactivating a slicer
                {
                  clicked_item->setData(column,Qt::CheckStateRole,2);
                  return;
                }
            }
          //if we clicked on the vector(or overlay) and not the image
          else
            {
              if (clicked_item->data(column,Qt::CheckStateRole).toInt())
                {
                  current_row->setData(column,Qt::CheckStateRole,2);
                  mSlicerManagers[i]->UpdateSlicer(column-1,2);
                  mSlicerManagers[i]->UpdateInfoOnCursorPosition(column-1);
                  DisplaySliders(i,column-1);
                }
              int vis = clicked_item->data(column,Qt::CheckStateRole).toInt();
              std::string overlay_type = clicked_item->data(1,Qt::UserRole).toString().toStdString();
              int overlay_index=0;
              for (int child = 0; child < current_row->childCount(); child++)
                {
                  if (current_row->child(child)->data(1,Qt::UserRole).toString().toStdString() == overlay_type)
                    overlay_index++;
                  if (current_row->child(child) == clicked_item) break;
                }
              clicked_slicer->SetActorVisibility(
                                                 clicked_item->data(1,Qt::UserRole).toString().toStdString(), overlay_index-1,vis);
            }
        }
      else if (current_row->data(column,Qt::CheckStateRole).toInt() > 0)
        {
          current_row->setData(column,Qt::CheckStateRole,0);
          mSlicerManagers[i]->UpdateSlicer(column-1,0);
          std::map<std::string,int> overlay_counts;
          for (int child = 0; child < current_row->childCount(); child++)
            {
              std::string overlay_type = 
                current_row->child(child)->data(1,Qt::UserRole).toString().toStdString();
              overlay_counts[overlay_type]++;
              current_row->child(child)->setData(column,Qt::CheckStateRole,0);
              vvSlicer * current_slicer=mSlicerManagers[i]->GetSlicer(column-1);
              current_slicer->SetActorVisibility(overlay_type,overlay_counts[overlay_type]-1,false);
            }
        }
      //mSlicerManagers[i]->SetColorMap(-1);
      mSlicerManagers[i]->SetColorMap();
    }
  mSlicerManagers[index]->GetSlicer(column-1)->Render();
}
//------------------------------------------------------------------------------

void vvMainWindow::InitSlicers()
{
  if (mSlicerManagers.size())
    {
      mSlicerManagers.back()->GenerateDefaultLookupTable();

      mSlicerManagers.back()->SetSlicerWindow(0,NOViewWidget->GetRenderWindow());
      mSlicerManagers.back()->SetSlicerWindow(1,NEViewWidget->GetRenderWindow());
      mSlicerManagers.back()->SetSlicerWindow(2,SOViewWidget->GetRenderWindow());
      mSlicerManagers.back()->SetSlicerWindow(3,SEViewWidget->GetRenderWindow());
    }
}

//------------------------------------------------------------------------------
void vvMainWindow::InitDisplay() {
  if (mSlicerManagers.size())
    {
      //BE CAREFUL : this is absolutely necessary to set the interactor style
      //in order to have the same style instanciation for all SlicerManagers in
      // a same window
      for (int j = 0; j < 4; j++)
        {
          vvInteractorStyleNavigator* style = vvInteractorStyleNavigator::New();
          style->SetAutoAdjustCameraClippingRange(1);
          bool AlreadySelected = false;
          for (int i = 0; i < DataTree->topLevelItemCount(); i++)
            {
              mSlicerManagers[i]->SetInteractorStyleNavigator(j,style);

              //select the image only if previous are not selected
              if (DataTree->topLevelItem(i)->data(j+1,Qt::CheckStateRole).toInt() > 1)
                {
                  mSlicerManagers[i]->UpdateSlicer(j,1);
                  AlreadySelected = true;
                }
              else if (i == DataTree->topLevelItemCount()-1 && !AlreadySelected)
                {
                  if (DataTree->selectedItems().size() == 0)
                    DataTree->topLevelItem(i)->setSelected(1);
                  DataTree->topLevelItem(i)->setData(j+1,Qt::CheckStateRole,2);
                  mSlicerManagers[i]->UpdateSlicer(j,1);
                  DisplaySliders(i,j);
                }
              else
                {
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
void vvMainWindow::DisplaySliders(int slicer, int window) {
  int range[2];
  mSlicerManagers[slicer]->GetSlicer(window)->GetSliceRange(range);
  int position = mSlicerManagers[slicer]->GetSlicer(window)->GetSlice();

  int tRange[2];
  tRange[0] = 0;
  tRange[1] = mSlicerManagers[slicer]->GetSlicer(window)->GetTMax();
  int tPosition = mSlicerManagers[slicer]->GetSlicer(window)->GetTSlice();
  bool showHorizontal = false;
  bool showVertical = false;
  if (mSlicerManagers[slicer]->GetSlicer(window)->GetImage()->GetNumberOfDimensions() > 3
      || (mSlicerManagers[slicer]->GetSlicer(window)->GetImage()->GetNumberOfDimensions() > 2
          && mSlicerManagers[slicer]->GetType() != IMAGEWITHTIME
          && mSlicerManagers[slicer]->GetType() != MERGEDWITHTIME))
    showVertical = true;
  if (mSlicerManagers[slicer]->GetSlicer(window)->GetImage()->GetNumberOfDimensions() > 3
      || mSlicerManagers[slicer]->GetType() == IMAGEWITHTIME
      || mSlicerManagers[slicer]->GetType() == MERGEDWITHTIME)
    showHorizontal = true;

  if (showVertical)
    verticalSliders[window]->show();
  else
    verticalSliders[window]->hide();
  verticalSliders[window]->setRange(range[0],range[1]);
  verticalSliders[window]->setValue(position);

  if (showHorizontal)
    horizontalSliders[window]->show();
  else
    horizontalSliders[window]->hide();
  horizontalSliders[window]->setRange(tRange[0],tRange[1]);
  horizontalSliders[window]->setValue(tPosition);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::CloseImage(QTreeWidgetItem* item, int column) {
  int index = GetSlicerIndexFromItem(item);

  if (DataTree->topLevelItem(index) != item)
    {
      QString warning = "Do you really want to close the overlay : ";
      warning += item->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
      QMessageBox msgBox(QMessageBox::Warning, tr("Close Overlay"),
                         warning, 0, this);
      msgBox.addButton(tr("Close"), QMessageBox::AcceptRole);
      msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
      if (msgBox.exec() == QMessageBox::AcceptRole)
        {
          std::string overlay_type=item->data(1,Qt::UserRole).toString().toStdString();
          int overlay_index=0;
          for (int child = 0; child < DataTree->topLevelItem(index)->childCount(); child++)
            {
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
    }
  else if (DataTree->topLevelItemCount() <= 1)
    {
      QString warning = "Do you really want to close the image : ";
      warning += item->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
      warning += "\nThis is the last image, you're about to close vv !!!";
      QMessageBox msgBox(QMessageBox::Warning, tr("Close Image"),
                         warning, 0, this);
      msgBox.addButton(tr("Close vv"), QMessageBox::AcceptRole);
      msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
      if (msgBox.exec() == QMessageBox::AcceptRole)
        {
          this->close();
        }
    }
  else
    {
      QString warning = "Do you really want to close the image : ";
      warning += item->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
      QMessageBox msgBox(QMessageBox::Warning, tr("Close Image"),
                         warning, 0, this);
      msgBox.addButton(tr("Close"), QMessageBox::AcceptRole);
      msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
      if (msgBox.exec() == QMessageBox::AcceptRole)
        {

          // Tell tools that we close an image
          emit AnImageIsBeingClosed(mSlicerManagers[index]);

          std::vector<vvSlicerManager*>::iterator Manageriter = mSlicerManagers.begin();
          DataTree->takeTopLevelItem(index);
          for (int i = 0; i < index; i++)
            {
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
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ReloadImage(QTreeWidgetItem* item, int column) {
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
  if ( role == "vector")
    mSlicerManagers[index]->ReloadVF();
  else if (role == "overlay")
    mSlicerManagers[index]->ReloadOverlay();
  else if (role == "fusion")
    mSlicerManagers[index]->ReloadFusion();
  else
    mSlicerManagers[index]->Reload();
  QApplication::restoreOverrideCursor();  
}
//------------------------------------------------------------------------------

// void vvMainWindow::CropImage()
// {
//   int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
//   vvCropDialog crop(mSlicerManagers,index);
//   if(crop.exec())
//     AddImage(crop.GetOutput(),"cropped.mhd");
// }

//------------------------------------------------------------------------------
void vvMainWindow::SplitImage() {
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  int dim = mSlicerManagers[index]->GetDimension();
  QString warning = "Do you really want to split the ";
  warning += QString::number(dim) + "D image ";
  warning += DataTree->selectedItems()[0]->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString() + " into ";
  warning += QString::number(mSlicerManagers[index]->GetSlicer(0)->GetImage()->GetSize()[dim-1]) + " ";
  warning += QString::number(dim-1) + "D images.";
  QMessageBox msgBox(QMessageBox::Warning, tr("Split Image"),
                     warning, 0, this);
  msgBox.addButton(tr("Split"), QMessageBox::AcceptRole);
  msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
  if (msgBox.exec() == QMessageBox::AcceptRole)
    {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      if (dim > 2)
        {
          std::string filename = DataTree->selectedItems()[0]->data(0,Qt::UserRole).toString().toStdString();
          int numberOfSlice = mSlicerManagers[index]->GetSlicer(0)->GetImage()->GetSize()[dim-1];
          std::string path = itksys::SystemTools::GetFilenamePath(
                                                                  filename);
          path += "/";
          path += DataTree->selectedItems()[0]->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString().toStdString();
          path += "%03d";
          path += itksys::SystemTools::GetFilenameLastExtension(
                                                                filename).c_str();

          typedef itk::NumericSeriesFileNames NameGeneratorType;
          NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
          nameGenerator->SetSeriesFormat(path.c_str());
          nameGenerator->SetStartIndex(0);
          nameGenerator->SetEndIndex(numberOfSlice-1);
          nameGenerator->SetIncrementIndex(1);

          for (int i = 0; i < numberOfSlice; i++)
            {
              vvSlicerManager* imageManager = new vvSlicerManager(4);
              imageManager->SetExtractedImage(nameGenerator->GetFileNames()[i],
                                              mSlicerManagers[index]->GetSlicer(0)->GetImage(), i);
              mSlicerManagers.push_back(imageManager);

              //create an item in the tree with good settings
              QTreeWidgetItem *item = new QTreeWidgetItem();
              item->setData(0,Qt::UserRole,nameGenerator->GetFileNames()[i].c_str());
              std::string fileI = itksys::SystemTools::GetFilenameWithoutLastExtension(
                                                                                       nameGenerator->GetFileNames()[i]).c_str();
              item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,fileI.c_str());
              for (int j = 1; j <= 4; j++)
                {
                  for (int i = 0; i < DataTree->topLevelItemCount(); i++)
                    {
                      DataTree->topLevelItem(i)->setData(j,Qt::CheckStateRole,0);
                    }
                  item->setData(j,Qt::CheckStateRole,2);
                }

              //Create the buttons for reload and close
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
              connect(rButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
                      this,SLOT(ReloadImage(QTreeWidgetItem*, int)));

              DataTree->addTopLevelItem(item);
              DataTree->setItemWidget(item, COLUMN_CLOSE_IMAGE, cButton);
              DataTree->setItemWidget(item, COLUMN_RELOAD_IMAGE, rButton);

              //set the id of the image
              QString id = nameGenerator->GetFileNames()[i].c_str() + QString::number(mSlicerManagers.size()-1);
              item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
              mSlicerManagers.back()->SetId(id.toStdString());
              linkPanel->addImage(fileI, id.toStdString());
              connect(mSlicerManagers.back(),SIGNAL(currentImageChanged(std::string)),
                      this,SLOT(CurrentImageChanged(std::string)));
              connect(mSlicerManagers.back(),SIGNAL(
                                                    UpdatePosition(int, double, double, double, double, double, double, double)),this,
                      SLOT(MousePositionChanged(int,double, double, double, double, double, double, double)));
              connect(mSlicerManagers.back(),SIGNAL(UpdateVector(int, double, double, double, double)),
                      this, SLOT(VectorChanged(int,double,double,double, double)));
              connect(mSlicerManagers.back(),SIGNAL(UpdateOverlay(int, double, double)),
                      this, SLOT(OverlayChanged(int,double,double)));
              connect(mSlicerManagers.back(),SIGNAL(UpdateFusion(int, double)),
                      this, SLOT(FusionChanged(int,double)));
              connect(mSlicerManagers.back(),SIGNAL(UpdateWindows(int, int, int)),
                      this,SLOT(WindowsChanged(int, int, int)));
              connect(mSlicerManagers.back(),SIGNAL(WindowLevelChanged(double, double,int, int)),
                      this,SLOT(WindowLevelChanged(double, double, int, int)));
              connect(mSlicerManagers.back(),SIGNAL(UpdateSlice(int,int)),
                      this,SLOT(UpdateSlice(int,int)));
              connect(mSlicerManagers.back(),SIGNAL(UpdateTSlice(int, int)),
                      this,SLOT(UpdateTSlice(int, int)));
              connect(mSlicerManagers.back(),SIGNAL(UpdateSliceRange(int,int,int,int,int)),
                      this,SLOT(UpdateSliceRange(int,int,int,int,int)));
              connect(mSlicerManagers.back(),SIGNAL(UpdateLinkManager(std::string,int,double,double,double,int)),
                      this,SLOT(UpdateLinkManager(std::string,int,double,double,double,int)));
              connect(mSlicerManagers.back(),SIGNAL(LandmarkAdded()),landmarksPanel,SLOT(AddPoint()));
              UpdateTree();
              qApp->processEvents();
              InitSlicers();
              InitDisplay();
              qApp->processEvents();
            }
          QApplication::restoreOverrideCursor();
        }
      else
        {
          QApplication::restoreOverrideCursor();
          QString error = "Cannot split file (dimensions must be greater than 2) ";
          QMessageBox::information(this,tr("Splitting problem"),error);
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::MousePositionChanged(int visibility,double x, double y, double z, double X, double Y, double Z , double value) {
  infoPanel->setCurrentInfo(visibility,x,y,z,X,Y,Z,value);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::VectorChanged(int visibility,double x, double y, double z, double value) {
  overlayPanel->getCurrentVectorInfo(visibility,x,y,z,value);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::OverlayChanged(int visibility, double valueOver, double valueRef) {
  overlayPanel->getCurrentOverlayInfo(visibility,valueOver, valueRef);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::FusionChanged(int visibility, double value) {
  overlayPanel->getCurrentFusionInfo(visibility,value);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::WindowsChanged(int window, int view, int slice) {
  infoPanel->setViews(window, view, slice);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::WindowLevelChanged(double window, double level,int preset,int colormap) {
  windowSpinBox->setValue(window);
  levelSpinBox->setValue(level);
  colorMapComboBox->setCurrentIndex(colormap);
  presetComboBox->setCurrentIndex(preset);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::WindowLevelEdited() {
  presetComboBox->setCurrentIndex(6);
  UpdateWindowLevel();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateWindowLevel() {
  if (DataTree->selectedItems().size())
    {
      if (presetComboBox->currentIndex() == 7) //For ventilation
        colorMapComboBox->setCurrentIndex(5);
      int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
      mSlicerManagers[index]->SetColorWindow(windowSpinBox->value());
      mSlicerManagers[index]->SetColorLevel(levelSpinBox->value());
      mSlicerManagers[index]->SetPreset(presetComboBox->currentIndex());
      mSlicerManagers[index]->Render();
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateColorMap() {
  if (DataTree->selectedItems().size())
    {
      int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
      mSlicerManagers[index]->SetColorMap(colorMapComboBox->currentIndex());
      mSlicerManagers[index]->Render();
    }
}
//------------------------------------------------------------------------------
void vvMainWindow::SwitchWindowLevel() {
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  int window = mSlicerManagers[index]->GetColorWindow();
  presetComboBox->setCurrentIndex(6);
  windowSpinBox->setValue(-window);
  UpdateWindowLevel();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateLinkManager(std::string id, int slicer, double x, double y, double z, int temps) {
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++)
    {
      if (mSlicerManagers[i]->GetId() == id)
        {
          //mSlicerManagers[i]->SetTSlice(temps);
          mSlicerManagers[i]->GetSlicer(slicer)->SetCurrentPosition(x,y,z,temps);
          mSlicerManagers[i]->UpdateViews(0,slicer);
          break;
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ShowContextMenu(QPoint point) {
  if (!DataTree->selectedItems().size())
    {
      contextActions[1]->setEnabled(0);
      contextActions[2]->setEnabled(0);
      contextActions[3]->setEnabled(0);
      contextActions[4]->setEnabled(0);
      contextActions[5]->setEnabled(0);
      contextActions[6]->setEnabled(0);
      contextActions[7]->setEnabled(0);
    }
  else
    {
      int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
      contextActions[1]->setEnabled(1);
      contextActions[2]->setEnabled(
                                    DataTree->itemWidget(DataTree->selectedItems()[0],
                                                         COLUMN_RELOAD_IMAGE)->isEnabled());
      contextActions[3]->setEnabled(1);
      contextActions[5]->setEnabled(1);
      contextActions[6]->setEnabled(1);
      contextActions[7]->setEnabled(1);

      if (mSlicerManagers[index]->GetDimension() < 3)
        contextActions[4]->setEnabled(0);
      else
        contextActions[4]->setEnabled(1);
    }
  contextMenu.exec(QCursor::pos());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::CloseImage() {
  CloseImage(DataTree->selectedItems()[0],0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ReloadImage() {
  ReloadImage(DataTree->selectedItems()[0],0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SelectOverlayImage() {
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);

  //check if one overlay image is added
  for (int child = 0; child < DataTree->topLevelItem(index)->childCount(); child++)
    if (DataTree->topLevelItem(index)->child(child)->data(1,Qt::UserRole).toString() == "overlay")
      {
        QString error = "Cannot add more than one compared image\n";
        error += "Please remove first ";
        error += DataTree->topLevelItem(index)->child(child)->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
        QMessageBox::information(this,tr("Problem adding compared image !"),error);
        return;
      }

  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";
  QString file = QFileDialog::getOpenFileName(this,tr("Load Overlay image"),mInputPathName,Extensions);
  if (!file.isEmpty())
    AddOverlayImage(index,file);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::AddOverlayImage(int index, QString file) {
  mInputPathName = itksys::SystemTools::GetFilenamePath(file.toStdString()).c_str();
  itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(
                                                                        file.toStdString().c_str(), itk::ImageIOFactory::ReadMode);
  reader->SetFileName(file.toStdString().c_str());
  reader->ReadImageInformation();
  std::string component = reader->GetComponentTypeAsString(reader->GetComponentType());
  int dimension = reader->GetNumberOfDimensions();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vvProgressDialog progress("Opening " + file.toStdString());
  qApp->processEvents();

  std::string filename = itksys::SystemTools::GetFilenameWithoutExtension(file.toStdString()).c_str();
  if (mSlicerManagers[index]->SetOverlay(file.toStdString(),dimension, component))
    {
      //create an item in the tree with good settings
      QTreeWidgetItem *item = new QTreeWidgetItem();
      item->setData(0,Qt::UserRole,file.toStdString().c_str());
      item->setData(1,Qt::UserRole,tr("overlay"));
      item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,filename.c_str());
      qApp->processEvents();

      for (int j = 1; j <= 4; j++)
        {
          item->setData(j,Qt::CheckStateRole,DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole));
          mSlicerManagers[index]->GetSlicer(j-1)->SetActorVisibility("overlay",0,
                                                                     DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole).toInt());
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
    }
  else
    {
      QApplication::restoreOverrideCursor();
      QString error = "Cannot import the new image.\n";
      error += mSlicerManagers[index]->GetLastError().c_str();
      QMessageBox::information(this,tr("Problem reading image !"),error);
    }
}

void vvMainWindow::AddFusionImage()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);

  //check if one fusion image is added
  for (int child = 0; child < DataTree->topLevelItem(index)->childCount(); child++)
    if (DataTree->topLevelItem(index)->child(child)->data(1,Qt::UserRole).toString() == "fusion")
      {
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
    {
      mInputPathName = itksys::SystemTools::GetFilenamePath(file.toStdString()).c_str();
      itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(
                                                                            file.toStdString().c_str(), itk::ImageIOFactory::ReadMode);
      reader->SetFileName(file.toStdString().c_str());
      reader->ReadImageInformation();
      std::string component = reader->GetComponentTypeAsString(reader->GetComponentType());
      if (reader)
        {
          QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
          vvProgressDialog progress("Opening fusion");
          qApp->processEvents();

          std::string filename = itksys::SystemTools::GetFilenameWithoutExtension(file.toStdString()).c_str();
          if (mSlicerManagers[index]->SetFusion(file.toStdString(),
                                                reader->GetNumberOfDimensions(), component))
            {
              //create an item in the tree with good settings
              QTreeWidgetItem *item = new QTreeWidgetItem();
              item->setData(0,Qt::UserRole,file.toStdString().c_str());
              item->setData(1,Qt::UserRole,tr("fusion"));
              item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,filename.c_str());
              qApp->processEvents();

              for (int j = 1; j <= 4; j++)
                {
                  item->setData(j,Qt::CheckStateRole,DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole));
                  mSlicerManagers[index]->GetSlicer(j-1)->SetActorVisibility("fusion",0,
                                                                             DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole).toInt());
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
            }
          else
            {
              QApplication::restoreOverrideCursor();
              QString error = "Cannot import the new image.\n";
              error += mSlicerManagers[index]->GetLastError().c_str();
              QMessageBox::information(this,tr("Problem reading image !"),error);
            }
        }
      else
        {
          QString error = "Cannot import the new image.\n";
          QMessageBox::information(this,tr("Problem reading image !"),error);
        }
    }
}


void vvMainWindow::OpenField()
{
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  //check if a vector field has already been added
  for (int child = 0; child < DataTree->topLevelItem(index)->childCount(); child++)
    if (DataTree->topLevelItem(index)->child(child)->data(1,Qt::UserRole).toString() == "vector")
      {
        QString error = "Cannot add more than one vector field\n";
        error += "Please remove first ";
        error += DataTree->topLevelItem(index)->child(child)->data(COLUMN_IMAGE_NAME,Qt::DisplayRole).toString();
        QMessageBox::information(this,tr("Problem adding vector field!"),error);
        return;
      }

  QString Extensions = "Images ( *.mhd)";
  Extensions += ";;Images ( *.vf)";
  QString file = QFileDialog::getOpenFileName(this,tr("Load deformation field"),mInputPathName,Extensions);
  if (!file.isEmpty())
    AddField(file,index);
}

void vvMainWindow::AddFieldEntry(QString filename,int index,bool from_disk)
{
  //create an item in the tree with good settings
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setData(0,Qt::UserRole,filename.toStdString().c_str());
  item->setData(1,Qt::UserRole,tr("vector"));
  item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,filename);
  qApp->processEvents();

  for (int j = 1; j <= 4; j++)
    {
      item->setData(j,Qt::CheckStateRole,DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole));
      mSlicerManagers[index]->GetSlicer(j-1)->SetActorVisibility("vector",0,
                                                                 DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole).toInt());
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

void vvMainWindow::AddField(vvImage::Pointer vf,QString file,int index)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vvSlicerManager* imageManager = mSlicerManagers[index];
  if (imageManager->SetVF(vf,file.toStdString()))
    {
      AddFieldEntry(file,index,false);
    }
  else
    {
      QString error = "Cannot import the vector field for this image.\n";
      error += imageManager->GetLastError().c_str();
      QMessageBox::information(this,tr("Problem reading VF !"),error);
    }
  QApplication::restoreOverrideCursor();
}

void vvMainWindow::AddField(QString file,int index)
{
  if (QFile::exists(file))
    {
      mInputPathName = itksys::SystemTools::GetFilenamePath(file.toStdString()).c_str();

      //init the progress events
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      vvProgressDialog progress("Opening " + file.toStdString());
      qApp->processEvents();

      //read the vector and put it in the current mSlicerManager
      vvSlicerManager* imageManager = mSlicerManagers[index];
      qApp->processEvents();

      std::string filename = itksys::SystemTools::GetFilenameWithoutExtension(file.toStdString()).c_str();
      if (imageManager->SetVF(file.toStdString()))
        {
          imageManager->Render();
          AddFieldEntry(file,index,true);
        }
      else
        {
          QApplication::restoreOverrideCursor();
          QString error = "Cannot import the vector field for this image.\n";
          error += imageManager->GetLastError().c_str();
          QMessageBox::information(this,tr("Problem reading VF !"),error);
        }
    }
  else
    QMessageBox::information(this,tr("Problem reading VF !"),"File doesn't exist!");

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SetVFProperty(int subsampling, int scale, int log) {
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if (mSlicerManagers[index]->GetSlicer(0)->GetVF())
    {
      for (int i = 0; i < 4; i++)
        {
          mSlicerManagers[index]->GetSlicer(i)->SetVFSubSampling(subsampling);
          mSlicerManagers[index]->GetSlicer(i)->SetVFScale(scale);
          if (log > 0)
            mSlicerManagers[index]->GetSlicer(i)->SetVFLog(1);
          else
            mSlicerManagers[index]->GetSlicer(i)->SetVFLog(0);
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SetOverlayProperty(int color) {
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if (mSlicerManagers[index]->GetSlicer(0)->GetOverlay())
    {
      mSlicerManagers[index]->SetOverlayColor(color);
      mSlicerManagers[index]->SetColorMap(0);
      mSlicerManagers[index]->Render();
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SetFusionProperty(int opacity, int colormap,double window, double level) {
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  if (mSlicerManagers[index]->GetSlicer(0)->GetFusion())
    {
      mSlicerManagers[index]->SetFusionOpacity(opacity);
      mSlicerManagers[index]->SetFusionColorMap(colormap);
      mSlicerManagers[index]->SetFusionWindow(window);
      mSlicerManagers[index]->SetFusionLevel(level);
      mSlicerManagers[index]->SetColorMap(0);
      mSlicerManagers[index]->Render();
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveAs() {
  if (DataTree->selectedItems()[0]->data(1,Qt::UserRole).toString() == "vector")
    {
      QMessageBox::warning(this,tr("Unsupported type"),tr("Sorry, saving a vector field is unsupported for the moment"));
      return;
    }

  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  int dimension = mSlicerManagers[index]->GetDimension();
  QStringList OutputListeFormat;
  OutputListeFormat.clear();
  if (dimension == 1)
    {
      OutputListeFormat.push_back(".mhd");
    }
  if (dimension == 2)
    {
      OutputListeFormat.push_back(".bmp");
      OutputListeFormat.push_back(".png");
      OutputListeFormat.push_back(".jpeg");
      OutputListeFormat.push_back(".tif");
      OutputListeFormat.push_back(".mhd");
      OutputListeFormat.push_back(".hdr");
      OutputListeFormat.push_back(".vox");
    }
  else if (dimension == 3)
    {
      OutputListeFormat.push_back(".mhd");
      OutputListeFormat.push_back(".hdr");
      OutputListeFormat.push_back(".vox");
    }
  else if (dimension == 4)
    {
      OutputListeFormat.push_back(".mhd");
    }
  QString Extensions = "AllFiles(*.*)";
  for (int i = 0; i < OutputListeFormat.count(); i++)
    {
      Extensions += ";;Images ( *";
      Extensions += OutputListeFormat[i];
      Extensions += ")";
    }
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save As"),
                                                  mSlicerManagers[index]->GetFileName().c_str(),
                                                  Extensions);
  if (!fileName.isEmpty())
    {
      std::string fileformat = itksys::SystemTools::GetFilenameLastExtension(fileName.toStdString());
      if (OutputListeFormat.contains(
                                     fileformat.c_str()))
        {
          QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
          std::string action = "Saving";
          vvProgressDialog progress("Saving "+fileName.toStdString());
          qApp->processEvents();
          vvImageWriter *writer = new vvImageWriter;
          writer->SetOutputFileName(fileName.toStdString());
          writer->SetInput(mSlicerManagers[index]->GetImage());
          writer->Update();
          QApplication::restoreOverrideCursor();
          if (writer->GetLastError().size())
            {
              QString error = "Saving did not succeed\n";
              error += writer->GetLastError().c_str();
              QMessageBox::information(this,tr("Saving Problem"),error);
              SaveAs();
            }
        }
      else
        {
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
void vvMainWindow::AddLink(QString image1,QString image2) {
  for (unsigned int i = 0; i < mSlicerManagers.size();i++)
    {
      if (image1.toStdString() == mSlicerManagers[i]->GetId())
        {
          mSlicerManagers[i]->AddLink(image2.toStdString());
        }
      if (image2.toStdString() == mSlicerManagers[i]->GetId())
        {
          mSlicerManagers[i]->AddLink(image1.toStdString());
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::RemoveLink(QString image1,QString image2) {
  for (unsigned int i = 0; i < mSlicerManagers.size();i++)
    {
      if (image1.toStdString() == mSlicerManagers[i]->GetId())
        {
          mSlicerManagers[i]->RemoveLink(image2.toStdString());
        }
      if (image2.toStdString() == mSlicerManagers[i]->GetId())
        {
          mSlicerManagers[i]->RemoveLink(image1.toStdString());
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ChangeImageWithIndexOffset(vvSlicerManager *sm, int slicer, int offset)
{
    int index = 0;
    while(sm != mSlicerManagers[index])
        index++;
    index = (index+offset) % mSlicerManagers.size();

    QTreeWidgetItem* item = GetItemFromSlicerManager(mSlicerManagers[index]);
    //CurrentImageChanged(mSlicerManagers[index]->GetId()); //select new image
    item->setData(slicer+1,Qt::CheckStateRole,2);         //change checkbox
    DisplayChanged(item,slicer+1);
}
//------------------------------------------------------------------------------

void vvMainWindow::HorizontalSliderMoved(int value,int column, int slicer_index)
{
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++)
    {
      if (DataTree->topLevelItem(i)->data(column,Qt::CheckStateRole).toInt() > 1)
        {
          for (int j = 0; j < 4; j++)
            {
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
void vvMainWindow::NOHorizontalSliderMoved() {
  HorizontalSliderMoved(NOHorizontalSlider->value(),COLUMN_UL_VIEW,0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::NEHorizontalSliderMoved() {
  HorizontalSliderMoved(NEHorizontalSlider->value(),COLUMN_UR_VIEW,1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SOHorizontalSliderMoved() {
  HorizontalSliderMoved(SOHorizontalSlider->value(),COLUMN_DL_VIEW,2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SEHorizontalSliderMoved() {
  HorizontalSliderMoved(SEHorizontalSlider->value(),COLUMN_DR_VIEW,3);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::NOVerticalSliderChanged() {
  int value = NOVerticalSlider->value();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++)
    {
      if (DataTree->topLevelItem(i)->data(COLUMN_UL_VIEW,Qt::CheckStateRole).toInt() > 1)
        {
          mSlicerManagers[i]->GetSlicer(0)->SetSlice(value);
          // mSlicerManagers[i]->UpdateSlice(0);
          // <-- DS add this. Not too much update ? YES.
          break;
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::NEVerticalSliderChanged() {
  int value = NEVerticalSlider->value();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++)
    {
      if (DataTree->topLevelItem(i)->data(COLUMN_UR_VIEW,Qt::CheckStateRole).toInt() > 1)
        {
          mSlicerManagers[i]->GetSlicer(1)->SetSlice(value);
          break;
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SOVerticalSliderChanged() {
  int value = SOVerticalSlider->value();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++)
    {
      if (DataTree->topLevelItem(i)->data(COLUMN_DL_VIEW,Qt::CheckStateRole).toInt() > 1)
        {
          mSlicerManagers[i]->GetSlicer(2)->SetSlice(value);
          break;
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SEVerticalSliderChanged() {
  int value = SEVerticalSlider->value();
  for (unsigned int i = 0; i < mSlicerManagers.size(); i++)
    {
      if (DataTree->topLevelItem(i)->data(COLUMN_DR_VIEW,Qt::CheckStateRole).toInt() > 1)
        {
          mSlicerManagers[i]->GetSlicer(3)->SetSlice(value);
          break;
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateSlice(int slicer, int slice) {
  if (slicer == 0) {
    if (slice != NOVerticalSlider->value())
      NOVerticalSlider->setValue(slice);
  }
  else if (slicer == 1)
    NEVerticalSlider->setValue(slice);
  else if (slicer == 2)
    SOVerticalSlider->setValue(slice);
  else if (slicer == 3)
    SEVerticalSlider->setValue(slice);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::UpdateTSlice(int slicer, int slice) {
  switch (slicer)
    {
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
void vvMainWindow::UpdateSliceRange(int slicer, int min, int max, int tmin, int tmax) {
  int position = int((min+max)/2);
  int tPosition = int((tmin+tmax)/2);
  if (slicer == 0)
    {
      NOVerticalSlider->setValue(position);
      NOVerticalSlider->setRange(min,max);
      NOHorizontalSlider->setRange(tmin,tmax);
      NOHorizontalSlider->setValue(tPosition);
    }
  else if (slicer == 1)
    {
      NEVerticalSlider->setValue(position);
      NEVerticalSlider->setRange(min,max);
      NEHorizontalSlider->setRange(tmin,tmax);
      NEHorizontalSlider->setValue(tPosition);
    }
  else if (slicer == 2)
    {
      SOVerticalSlider->setValue(position);
      SOVerticalSlider->setRange(min,max);
      SOHorizontalSlider->setRange(tmin,tmax);
      SOHorizontalSlider->setValue(tPosition);
    }
  else if (slicer == 3)
    {
      SEVerticalSlider->setValue(position);
      SEVerticalSlider->setRange(min,max);
      SEHorizontalSlider->setRange(tmin,tmax);
      SEHorizontalSlider->setValue(tPosition);
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveNOScreenshot() {
  vtkWindowToImageFilter *w2i = vtkWindowToImageFilter::New();
  w2i->SetInput(NOViewWidget->GetRenderWindow());
  w2i->Update();
  SaveScreenshot(w2i->GetOutput());
  w2i->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveNEScreenshot() {
  vtkWindowToImageFilter *w2i = vtkWindowToImageFilter::New();
  w2i->SetInput(NEViewWidget->GetRenderWindow());
  w2i->Update();
  SaveScreenshot(w2i->GetOutput());
  w2i->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveSOScreenshot() {
  vtkWindowToImageFilter *w2i = vtkWindowToImageFilter::New();
  w2i->SetInput(SOViewWidget->GetRenderWindow());
  w2i->Update();
  SaveScreenshot(w2i->GetOutput());
  w2i->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveSEScreenshot() {
  vtkWindowToImageFilter *w2i = vtkWindowToImageFilter::New();
  w2i->SetInput(SEViewWidget->GetRenderWindow());
  w2i->Update();
  SaveScreenshot(w2i->GetOutput());
  w2i->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SaveScreenshot(vtkImageData* image) {
  QString Extensions = "Images( *.png);;";
  Extensions += "Images( *.jpg);;";
  Extensions += "Images( *.bmp);;";
  Extensions += "Images( *.tif);;";
  Extensions += "Images( *.ppm)";
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save As"),
                                                  itksys::SystemTools::GetFilenamePath(
                                                                                       mSlicerManagers[0]->GetFileName()).c_str(),
                                                  Extensions);
  if (!fileName.isEmpty())
    {
      const char *ext = fileName.toStdString().c_str() + strlen(fileName.toStdString().c_str()) - 4;
      if (!strcmp(ext, ".bmp"))
        {
          vtkBMPWriter *bmp = vtkBMPWriter::New();
          bmp->SetInput(image);
          bmp->SetFileName(fileName.toStdString().c_str());
          bmp->Write();
          bmp->Delete();
        }
      else if (!strcmp(ext, ".tif"))
        {
          vtkTIFFWriter *tif = vtkTIFFWriter::New();
          tif->SetInput(image);
          tif->SetFileName(fileName.toStdString().c_str());
          tif->Write();
          tif->Delete();
        }
      else if (!strcmp(ext, ".ppm"))
        {
          vtkPNMWriter *pnm = vtkPNMWriter::New();
          pnm->SetInput(image);
          pnm->SetFileName(fileName.toStdString().c_str());
          pnm->Write();
          pnm->Delete();
        }
      else if (!strcmp(ext, ".png"))
        {
          vtkPNGWriter *png = vtkPNGWriter::New();
          png->SetInput(image);
          png->SetFileName(fileName.toStdString().c_str());
          png->Write();
          png->Delete();
        }
      else if (!strcmp(ext, ".jpg"))
        {
          vtkJPEGWriter *jpg = vtkJPEGWriter::New();
          jpg->SetInput(image);
          jpg->SetFileName(fileName.toStdString().c_str());
          jpg->Write();
          jpg->Delete();
        }
      else
        {
          QMessageBox::information(this,tr("Problem saving screenshot !"),tr("Cannot save image.\nPlease set a file extension !!!"));
        }
    }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::GoToCursor() {
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  for (int column = 1; column < 5; column++)
    {
      if (DataTree->selectedItems()[0]->data(column,Qt::CheckStateRole).toInt() > 1)
        {
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
void vvMainWindow::PlayPause() {
  if (playMode)
    {
      playMode = 0;
      playButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/player_play.png")));
      ImageInfoChanged();
      return;
    }
  else
    {
      int image_number=DataTree->topLevelItemCount();
      bool has_temporal;
      for (int i=0;i<image_number;i++)
        if (mSlicerManagers[i]->GetImage()->GetVTKImages().size() > 1)
          {
            has_temporal=true;
            break;
          }
      if (has_temporal)
        {
          playMode = 1;
          playButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/player_pause.png")));
          QTimer::singleShot(1000/mFrameRate, this, SLOT(PlayNext()));
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::PlayNext() {
  if (playMode && !this->isHidden())
    {
      int image_number=DataTree->topLevelItemCount();
      ///Only play one slicer per SM, and only if the SM is being displayed
      for (int i=0;i<image_number;i++)
        for (int j=0;j<4;j++)
          if (mSlicerManagers[i]->GetImage()->GetVTKImages().size() > 1 &&
              DataTree->topLevelItem(i)->data(j+1,Qt::CheckStateRole).toInt() > 0)
            {
              mSlicerManagers[i]->SetNextTSlice(j);
              break;
            }
      QTimer::singleShot(1000/mFrameRate, this, SLOT(PlayNext()));
    }
}
//------------------------------------------------------------------------------

void vvMainWindow::ShowLastImage()
{
  if (mSlicerManagers.size() > 1)
    {
      QTreeWidgetItem * item=DataTree->topLevelItem(DataTree->topLevelItemCount()-1);
      CurrentImageChanged(mSlicerManagers.back()->GetId()); //select new image
      item->setData(1,Qt::CheckStateRole,2); //show the new image in the first panel
      DisplayChanged(item,1);
    }
}

//------------------------------------------------------------------------------
void vvMainWindow::UpdateRenderWindows() {
  if (NOViewWidget->GetRenderWindow()) NOViewWidget->GetRenderWindow()->Render();
  if (NEViewWidget->GetRenderWindow()) NEViewWidget->GetRenderWindow()->Render();
  if (SOViewWidget->GetRenderWindow()) SOViewWidget->GetRenderWindow()->Render();
  if (SEViewWidget->GetRenderWindow()) SEViewWidget->GetRenderWindow()->Render();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::ResampleCurrentImage() {
  vvResamplerDialog resampler;
  int index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  resampler.SetSlicerManagers(mSlicerManagers,index);
  if (resampler.exec()) 
    AddImage(resampler.GetOutput(),resampler.GetOutputFileName());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvMainWindow::SegmentationOnCurrentImage() {
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
void vvMainWindow::AddImage(vvImage::Pointer image,std::string filename)
{
  vvSlicerManager* slicer_manager = new vvSlicerManager(4);
  slicer_manager->SetImage(image);
  slicer_manager->SetFilename(filename);
  mSlicerManagers.push_back(slicer_manager);

  //create an item in the tree with good settings
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setData(0,Qt::UserRole,slicer_manager->GetFileName().c_str());//files[i].c_str());
  item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,filename.c_str());
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
  QString id = slicer_manager->GetFileName().c_str() + QString::number(mSlicerManagers.size()-1);
  item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
  mSlicerManagers.back()->SetId(id.toStdString());

  linkPanel->addImage(filename, id.toStdString());

  connect(mSlicerManagers.back(), SIGNAL(currentImageChanged(std::string)),
          this, SLOT(CurrentImageChanged(std::string)));
  connect(mSlicerManagers.back(), SIGNAL(UpdatePosition(int, double, double, double, double, double, double, double)),
          this, SLOT(MousePositionChanged(int,double, double, double, double, double, double, double)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateVector(int, double, double, double, double)),
          this, SLOT(VectorChanged(int,double,double,double, double)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateOverlay(int, double, double)),
          this, SLOT(OverlayChanged(int,double,double)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateFusion(int, double)),
          this, SLOT(FusionChanged(int,double)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateWindows(int, int, int)),
          this,SLOT(WindowsChanged(int, int, int)));
  connect(mSlicerManagers.back(), SIGNAL(WindowLevelChanged(double, double,int, int)),
          this,SLOT(WindowLevelChanged(double, double, int, int)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateSlice(int,int)),
          this,SLOT(UpdateSlice(int,int)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateTSlice(int, int)),
          this,SLOT(UpdateTSlice(int, int)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateSliceRange(int,int,int,int,int)),
          this,SLOT(UpdateSliceRange(int,int,int,int,int)));
  connect(mSlicerManagers.back(), SIGNAL(UpdateLinkManager(std::string,int,double,double,double,int)),
          this,SLOT(UpdateLinkManager(std::string,int,double,double,double,int)));
  connect(mSlicerManagers.back(), SIGNAL(ChangeImageWithIndexOffset(vvSlicerManager*,int,int)),
          this,SLOT(ChangeImageWithIndexOffset(vvSlicerManager*,int,int)));
  connect(mSlicerManagers.back(), SIGNAL(LandmarkAdded()),landmarksPanel,SLOT(AddPoint()));
  UpdateTree();
  qApp->processEvents();
  InitSlicers();
  ShowLastImage();
  InitDisplay();
  qApp->processEvents();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMainWindow::UpdateCurrentSlicer() {
  int index = -1;
  if (DataTree->selectedItems().size() > 0) {
    index = GetSlicerIndexFromItem(DataTree->selectedItems()[0]);
  }
  mSlicerManagerCurrentIndex = index;    
}
//------------------------------------------------------------------------------

