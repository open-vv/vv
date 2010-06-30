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

#ifndef VVMAINWINDOW_H
#define VVMAINWINDOW_H

#include <iostream>
#include <vector>
#include "ui_vvMainWindow.h"
#include "vvMainWindowBase.h"
#include "vvToolManager.h"
#include "vvConstants.h"
#include "vvMesh.h"

class vvSlicerManager;
class vvHelpDialog;
class vvDocumentation;
class vtkRenderWindowInteractor;
class vtkImageData;
class vtkRenderer;
class vvDicomSeriesSelector;

//------------------------------------------------------------------------------
class vvMainWindow: public vvMainWindowBase,
                    private Ui::vvMainWindow
{
  Q_OBJECT

  public:
  vvMainWindow();
  ~vvMainWindow();
  void LoadImages(std::vector<std::string> filenames, LoadedImageType type);
  vvSlicerManager * AddImage(vvImage::Pointer image,std::string filename);
  void AddField(QString file,int index);
  void AddOverlayImage(int index, QString filename);
  void AddROI(int index, QString filename);
  ///Adds a mesh to a SlicerManager, with optional warping by vector field
  void AddContour(int image_index, vvMesh::Pointer contour, bool propagation);
  ///This is used to show an image when opened or computed
  void ShowLastImage();

  virtual void UpdateCurrentSlicer();
  virtual QTabWidget * GetTab();
  //vvMainWindowToolInfo * GetInfoForTool();
//   void AddRunningTool(vvToolCreatorBase * tool);

public slots:
  ///Allows the user to open and select various surfaces contained in a dicom-struct file
  void OpenDCStructContour();
  ///Computes the MIP of the currently selected image and displays it
  void ComputeMIP();
  ///Computes the midposition image of a 4D sequence with a VF and displays it
  void ComputeMidPosition();
  void OpenImages();
  ///Slot triggered by the dynamically-generated recent file menu actions
  void OpenRecentImage();
  void OpenImageWithTime();
  void MergeImages();
  void MergeImagesWithTime();
  void OpenDicom();
  ///Open a vtkPolyData surface mesh and display it over the current image
  void OpenVTKContour();
  void SaveAs();
  void CurrentImageChanged(std::string id);
  void ImageInfoChanged();
  void ShowHelpDialog();
  void ShowDocumentation();
  void ComputeDeformableRegistration();
  void WarpImage();
  void ChangeViewMode();
  void DisplayChanged(QTreeWidgetItem *item, int column);
  void CloseImage(QTreeWidgetItem* item, int column);
  void ReloadImage(QTreeWidgetItem* item, int column);
  void MousePositionChanged(int visibility, double x, double y, double z, double X, double Y, double Z , double value);
  void VectorChanged(int visibility, double x, double y, double z, double value);
  void OverlayChanged(int visibility, double valueOver, double valueRef);
  void FusionChanged(int visibility, double value);
  void SegmentationOnCurrentImage();
  void SurfaceViewerLaunch();

  void WindowsChanged(int window, int view, int slice);
  void WindowLevelChanged(double window, double level,int preset, int colormap);
  void UpdateSlice(int slicer, int slice);
  void UpdateTSlice(int slicer, int slice);
  void UpdateSliceRange(int slicer, int min, int max, int tmin, int tmax);
  void WindowLevelEdited();
  void UpdateColorMap();
  void UpdateWindowLevel();
  void SwitchWindowLevel();
  void UpdateLinkManager(std::string id, int slicer, double x, double y, double z, int temps);
  void UpdateLinkedNavigation(std::string id, vvSlicerManager *sm);
  void AddLink(QString image1,QString image2);
  void RemoveLink(QString image1,QString image2);
  void ChangeImageWithIndexOffset(vvSlicerManager *sm, int slicer, int offset);

  ///Generic method called when any one of the horizontal sliders is moved
  void HorizontalSliderMoved(int value,int column, int slicer_index);
  void NOHorizontalSliderMoved();
  void NEHorizontalSliderMoved();
  void SOHorizontalSliderMoved();
  void SEHorizontalSliderMoved();

  void NOVerticalSliderChanged();
  void NEVerticalSliderChanged();
  void SOVerticalSliderChanged();
  void SEVerticalSliderChanged();

  void SaveNEScreenshot();
  void SaveNOScreenshot();
  void SaveSEScreenshot();
  void SaveSOScreenshot();

  void ShowContextMenu(QPoint point);
  //  void CropImage();
  void SplitImage();
  void CloseImage();
  void ReloadImage();
  void OpenField();
  void SelectOverlayImage();
  void AddFusionImage();

  void SetVFProperty(int subsampling,int scale,int lut, int width);
  void SetOverlayProperty(int color);
  void SetFusionProperty(int opacity,int colormap,double window,double level);

  void GoToCursor();
  void PlayPause();
  void PlayNext();
  void ChangeFrameRate(int rate) {
    mFrameRate = rate;
  }
  void UpdateRenderWindows();

private:

  //variables
  // std::vector<vvSlicerManager*> mSlicerManagers;
  vvHelpDialog *help_dialog;
  vvDocumentation *documentation;
  vvDicomSeriesSelector *dicomSeriesSelector;

  bool viewMode;
  bool playMode;

  //functions
  void UpdateTree();
  ///Adds a vector field to slicer manager index
  void WarpImage(vvSlicerManager* selected_slicer,int reference_phase);
  void AddFieldEntry(QString filename,int index,bool from_disk);
  void AddField(vvImage::Pointer vf,QString file,int index);
  void InitDisplay();
  ///Sets the render window and LUT for the last SlicerManager
  void InitSlicers();
  void DisplaySliders(int slicer, int window);
  QString GetSizeInBytes(unsigned long size);
  QString GetVectorDoubleAsString(std::vector<double> vectorDouble);
  QString GetVectorIntAsString(std::vector<int> vectorInt);
  int GetSlicerIndexFromItem(QTreeWidgetItem* item);
  QTreeWidgetItem* GetItemFromSlicerManager(vvSlicerManager* sm);
  void SaveScreenshot(vtkImageData* image);

  QMenu contextMenu;
  //QMenu *AddSubImageMenu;
  std::vector<QAction*> contextActions;
  std::vector<QSlider*> horizontalSliders;
  std::vector<QSlider*> verticalSliders;
  int mFrameRate;
  
  std::string mCurrentSelectedImageId;

  // vvMainWindowToolInfo * mCurrentToolInfo;
  // std::vector<vvToolCreatorBase*> mListOfRunningTool;

  static vvMainWindow * mSingleton;

  int mCurrentTime;

};

#include "vvMainWindow.txx"

#endif
