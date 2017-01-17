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

#ifndef VVMAINWINDOW_H
#define VVMAINWINDOW_H

#include <iostream>
#include <vector>
#include "ui_vvMainWindow.h"
#include "vvMainWindowBase.h"
#include "vvToolManager.h"
#include "vvImageReader.h"
#include "vvMesh.h"
#include "vvWipeImage.h"
#include "clitkMemoryUsage.h"

#include "vtkSmartPointer.h"


class vvSlicerManager;
class vvHelpDialog;
class vvDocumentation;
class vtkRenderWindowInteractor;
class vtkImageData;
class vtkRenderer;
class vtkMatrix4x4;
class vvDicomSeriesSelector;
#if CLITK_USE_PACS_CONNECTION
class vvQPacsConnection;
#endif
class vvSlicer;
class QTreeWidget;

//------------------------------------------------------------------------------
class vvMainWindow: public vvMainWindowBase,
                    private Ui::vvMainWindow
{
  Q_OBJECT

  public:
  vvMainWindow();
  ~vvMainWindow();
  void LoadImages(std::vector<std::string> filenames, vvImageReader::LoadedImageType type);
  vvSlicerManager * AddImage(vvImage::Pointer image,std::string filename);
  void AddField(QString file,int index);
  bool CheckAddedImage(int index, QString imageType);
  void AddOverlayImage(int index, std::vector<std::string> fileNames, vvImageReader::LoadedImageType type);
  void AddFusionImage(int index, std::vector<std::string> fileNames, vvImageReader::LoadedImageType type);
  void AddROI(int index, QString filename);

  //Process the sequence for fusion:
  void AddFusionSequence(int index, std::vector<std::string> fileNames, vvImageReader::LoadedImageType type);

  void AddLandmarks(int index, std::vector<std::string>);
///Adds a mesh to a SlicerManager, with optional warping by vector field
  void AddContour(int image_index, vvMesh::Pointer contour, bool propagation);
  ///This is used to show an image when opened or computed
  void ShowLastImage();
  void SaveCurrentStateAs(const std::string& stateFile);
  void ReadSavedStateFile(const std::string& stateFile);
	void LinkAllImages();

  virtual void UpdateCurrentSlicer();
  virtual QTabWidget * GetTab();
  QTreeWidget* GetTree() { return DataTree; }
  //vvMainWindowToolInfo * GetInfoForTool();
//   void AddRunningTool(vvToolCreatorBase * tool);

public slots:
  ///Allows the user to open and select various surfaces contained in a dicom-struct file
  void AddDCStructContour(int index, QString file);
  void OpenDCStructContour();
  ///Computes the midposition image of a 4D sequence with a VF and displays it
  void ComputeMidPosition();
  void OpenImages();
  ///Slot triggered by the dynamically-generated recent file menu actions
  void OpenRecentImage();
  void OpenImageWithTime();
  void MergeImages();
  void SliceImages();
  void MergeImagesWithTime();
  void OpenDicom();
#if CLITK_USE_PACS_CONNECTION
  void ConnectPacs();
#endif
  ///Open a vtkPolyData surface mesh and display it over the current image
  void OpenVTKContour();
  void SaveAs();
  void SaveCurrentState();
  void ReadSavedState();
  void CurrentImageChanged(std::string id);
  void CurrentPickedImageChanged(std::string id);
  void ImageInfoChanged();
  void ShowHelpDialog();
  void ShowDocumentation();
  void PopupRegisterForm(bool checkCanPush=false);
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
  //void FusionSequenceChanged(int visibility, double value);
  void SegmentationOnCurrentImage();
  void SurfaceViewerLaunch();

  void WindowLevelChanged();
  void UpdateSlice(int slicer, int slice);
  void UpdateTSlice(int slicer, int slice, int code=-1);
  void UpdateSliceRange(int slicer, int min, int max, int tmin, int tmax);
  void WindowLevelEdited();
  void SetWindowLevel(double w, double l);
  void UpdateColorMap();
  void UpdateWindowLevel();
  void UpdateSlicingPreset();
  void SwitchWindowLevel();
  void ApplyWindowLevelToAllImages();
  void ApplyWindowToSetOfImages(double window, unsigned int indexMin, unsigned int indexMax);
  void ApplyLevelToSetOfImages(double level, unsigned int indexMin, unsigned int indexMax);
  void UpdateLinkManager(std::string id, int slicer, double x, double y, double z, int temps);
  void UpdateLinkedNavigation(std::string id, vvSlicerManager *sm, vvSlicer* refSlicer);
  void AddLink(QString image1,QString image2,bool fromPanel = true);
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
  void SaveScreenshotAllSlices();

  void ShowContextMenu(QPoint point);
  void CloseImage();
  void ReloadImage();
  void OpenField();
  void SelectOverlayImage();
  void SelectWipeImage();
  void SelectFusionImage();
  //select the file(s) from the disk containing the image sequence to fuse
  void SelectFusionSequence();
  void SelectFusionSequenceCorrespondances();

  void ResetTransformationToIdentity();

  void SetVFProperty(int subsampling,int scale,int lut, int width, double r, double g, double b);
  void SetOverlayProperty(int color, int linked, double window, double level);
  void SetFusionProperty(int opacity, int tresOpacity, int colormap,double window,double level, bool showLegend);
  void SetFusionSequenceProperty(int fusionSequenceFrameIndex, bool spatialSyncFlag, unsigned int fusionSequenceNbFrames, bool temporalSyncFlag);

  void GoToCursor();
  void GoToLandmark();
  void PlayPause();
  void PlayNext();
  void ChangeFrameRate(int rate) {
    mFrameRate = rate;
  }
  void UpdateRenderWindows();
  void UpdateMemoryUsage();
  void show();
  
  static vvMainWindow* Instance(){return mSingleton;}
  
protected:
  
  void createRecentlyOpenedFilesMenu();
  void updateRecentlyOpenedFilesMenu(const std::list<std::string> &files);
  
private:

  //variables
  // std::vector<vvSlicerManager*> mSlicerManagers;
  vvHelpDialog *help_dialog;
  vvDocumentation *documentation;
  vvDicomSeriesSelector *dicomSeriesSelector;
  
  vvWipeImage::Pointer mWipeImage;

#if CLITK_USE_PACS_CONNECTION
  vvQPacsConnection *PacsConnection;
#endif

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
  int GetSlicerIndexFromItem(QTreeWidgetItem* item); //this actually returns the SlicerManager index TODO: rename it to GetSlicerManagerIndexFromItem
  QTreeWidgetItem* GetItemFromSlicerManager(vvSlicerManager* sm);
  void SaveScreenshot(QVTKWidget *widget);
  int GetImageDuplicateFilenameNumber(std::string filename);

  QMenu contextMenu;
  QMenu* recentlyOpenedFilesMenu;
  //QMenu *AddSubImageMenu;
  std::vector<QAction*> contextActions;
  std::vector<QSlider*> horizontalSliders;
  std::vector<QSlider*> verticalSliders;
  int mFrameRate;
  
  std::string mCurrentSelectedImageId;
  std::string mCurrentPickedImageId;
  unsigned int mCurrentPickedImageIndex;

  // vvMainWindowToolInfo * mCurrentToolInfo;
  // std::vector<vvToolCreatorBase*> mListOfRunningTool;

  static vvMainWindow* mSingleton;

  int mCurrentTime;
  
};

#endif
