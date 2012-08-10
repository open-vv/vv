
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

#ifndef VVSLICERMANAGER_H
#define VVSLICERMANAGER_H

// std
#include <iostream>
#include <vector>
#include <string>

// QT
#include <QObject>

// VTK
#include <vtksys/SystemTools.hxx>
class vtkImageData;
class vtkInteractorStyle;
class vtkRenderWindow;
class vtkPolyData;

// VV
#include "clitkCommon.h"
#include "vvImage.h"
#include "vvMesh.h"
#include "vvImageReader.h"

class vvSlicer;
class vvInteractorStyleNavigator;
class vvImageReader;
class vvImageReader;
class vvLandmarks;

//------------------------------------------------------------------------------
class vvSlicerManager : public QObject {
  Q_OBJECT

  public:
  typedef enum {WORLD_SLICING, VOXELS_SLICING} SlicingPresetType;
  vvSlicerManager(int numberOfSlicers);
  ~vvSlicerManager();

  std::string GetLastError() {
    return mLastError;
  }

  bool SetImage(std::string filename, vvImageReader::LoadedImageType type, int n=0, unsigned int slice=0);
  void SetImage(vvImage::Pointer image);
  bool SetImages(std::vector<std::string> filenames, vvImageReader::LoadedImageType type, int n=0);

  bool SetOverlay(std::string filename, int dim, std::string component);
  bool SetFusion(std::string filename, int dim, std::string component);
  ///Set a VF by loading it from the disk
  bool SetVF(std::string filename);
  ///Set a VF from memory
  bool SetVF(vvImage::Pointer vf,std::string filename);
  ///Add a mesh to the slicers, with optional propagation using a vector field
  void AddContour(vvMesh::Pointer ,bool propagate=false);
  ///Toggle temporal superposition of contours
  void ToggleContourSuperposition();

  std::string GetFileName()      { return mFileName; }
  std::string GetBaseFileName()  { return mBaseFileName; }
  int GetBaseFileNameNumber()    { return mBaseFileNameNumber; }
  std::string GetVFName()        { return mVFName; }
  std::string GetOverlayName()   { return mOverlayName; }
  std::string GetFusionName()    { return mFusionName; }
  std::string GetListOfAbsoluteFilePathInOneString(const std::string &actorType);

  ///Switch between nearest neighbor and linear interpolation
  void ToggleInterpolation();
  vvSlicer* GetSlicer(int i);
  void UpdateSlicer(int num, bool state);
  void SetSlicerWindow(int i, vtkRenderWindow* RW);
  void SetInteractorStyleNavigator(int i,vtkInteractorStyle* style);

  int GetNumberOfSlicers()        { return mSlicers.size(); }
  vvImage::Pointer GetImage()  { return mImage; }
  vvImage::Pointer GetVF()     { return mVF; }
  int GetType()                { return mType; }
  void SetId(std::string id)   { mId = id; }
  std::string GetId()          { return mId; }
  int GetDimension() {
    if (mImage) return mImage->GetNumberOfDimensions();
    else return -1;
  }

  void SetFilename(std::string f, int number=0);

  void SetSliceOrientation(int slicer, int orientation);
  int GetTSlice();
  void SetTSlice(int slice);
  void SetNextTSlice(int originating_slicer);
  void SetPreviousTSlice(int originating_slicer);
  void SetTSliceInSlicer(int tslice, int slicer);

  void GenerateDefaultLookupTable();
  void SetColorWindow(double s);
  void SetColorLevel(double s);
  void SetOverlayColorWindow(double s);
  void SetOverlayColorLevel(double s);
  void SetLinkOverlayWindowLevel(bool b);
  void SetLocalColorWindowing(const int slicer, const bool bCtrlKey);
  void SetOpacity(int i, double factor);
  void SetColorMap(int colormap);
  void SetPreset(int preset);
  void SetOverlayColor(int color) {
    mOverlayColor = (color/60)*60; //SR: new vvBlendImageActor needs 0 or 255 per component
  }
  void SetFusionOpacity(int opacity) {
    mFusionOpacity = opacity;
  }
  void SetFusionThresholdOpacity(int thresOpacity) {
    mFusionThresOpacity = thresOpacity;
  }
  void SetFusionColorMap(int colorMap) {
    mFusionColorMap = colorMap;
  }
  void SetFusionWindow(double window) {
    mFusionWindow = window;
  }
  void SetFusionLevel(double level) {
    mFusionLevel = level;
  }
  void SetFusionShowLegend(int show) {
    mFusionShowLegend = show;
  }

  double GetColorWindow();
  double GetColorLevel();
  double GetOverlayColorWindow() const;
  double GetOverlayColorLevel() const;
  bool GetLinkOverlayWindowLevel() const;
  int GetColorMap() {
    return mColorMap;
  }
  int GetPreset() {
    return mPreset;
  }
  SlicingPresetType GetSlicingPreset() {
    return mSlicingPreset;
  }
  int GetOverlayColor() const {
    return mOverlayColor;
  }
  int GetFusionOpacity() const {
    return mFusionOpacity;
  }
  int GetFusionThresholdOpacity() const {
    return mFusionThresOpacity;
  }
  int GetFusionColorMap() const {
    return mFusionColorMap;
  }
  double GetFusionWindow() const {
    return mFusionWindow;
  }
  double GetFusionLevel() const {
    return mFusionLevel;
  }

  void SetCursorAndCornerAnnotationVisibility(int s);
  void UpdateViews(int current, int slicer);
  void UpdateLinked(int slicer);
  void UpdateLinkedNavigation(vvSlicer *slicer, bool bPropagate=false);
  void ResetTransformationToIdentity(const std::string actorType);
  void Render();

  void AddLink(std::string newId) {
    mLinkedId.push_back(newId);
  }
  void RemoveLink(std::string oldId) {
    mLinkedId.remove(oldId); 
  }
  
  bool IsLinked() {
    return mLinkedId.size() > 0;
  }

  ///Remove the actor defined by its type and index (example: 3rd contour)
  void RemoveActor(const std::string& actor_type, int overlay_index);
  void RemoveActors();
  void Reload();
  void ReloadOverlay();
  void ReloadFusion();
  void ReloadVF();

  void Activated();
  void Picked();
  void UpdateInfoOnCursorPosition(int slicer);
  void UpdateWindowLevel();
  void UpdateSlice(int slicer);
  void UpdateTSlice(int slicer);
  void UpdateSliceRange(int slicer);
  void SetSlicingPreset(SlicingPresetType preset);

  vvLandmarks *GetLandmarks();
  void AddLandmark(float x,float y,float z,float t);
  
  void NextImage(int slicer);
  void PrevImage(int slicer);
  void LeftButtonReleaseEvent(int slicer);
  void VerticalSliderHasChanged(int slicer, int slice);
  double GetScalarComponentAsDouble(vtkImageData *image, double X, double Y, double Z, int component=0);

signals :
  void currentImageChanged(std::string id);
  void currentPickedImageChanged(std::string id);
  void UpdatePosition(int visibility,double x, double y, double z, double X, double Y, double Z, double value);
  void UpdateVector(int display, double x, double y, double z, double value);
  void UpdateOverlay(int display, double valueOver, double valueRef);
  void UpdateFusion(int display, double valueFus);
  void UpdateOrientation(int slicer, int orientation);
  void UpdateSlice(int slicer, int slice);
  void UpdateTSlice(int slicer, int slice);
  void UpdateSliceRange(int slice, int min, int max, int tmin, int tmax);
  void WindowLevelChanged();
  void UpdateLinkManager(std::string, int slicer, double x, double y, double z, int temps);
  void UpdateLinkedNavigation(std::string, vvSlicerManager*, vvSlicer*);
  void LandmarkAdded();
  void ChangeImageWithIndexOffset(vvSlicerManager *sm, int slicer, int offset);
  void LeftButtonReleaseSignal(int slicer);
  void AVerticalSliderHasChanged(int slicer, int slice);

protected:
  std::vector< vtkSmartPointer<vvSlicer> > mSlicers;
  vvImageReader::Pointer mReader;
  vvImageReader::Pointer mOverlayReader;
  vvImageReader::Pointer mFusionReader;
  vvImageReader::Pointer mVectorReader;
  vvImage::Pointer mImage;
  vvImage::Pointer mVF;
  int mColorMap;
  int mOverlayColor;

  int mFusionOpacity;
  int mFusionThresOpacity;
  int mFusionColorMap;
  double mFusionWindow;
  double mFusionLevel;
  bool mFusionShowLegend;

  int mPreset;
  SlicingPresetType mSlicingPreset;
  vvImageReader::LoadedImageType mType;
  std::string mVFComponent;
  std::string mOverlayComponent;
  std::string mFusionComponent;
  std::string mFileName;
  std::string mBaseFileName;
  int mBaseFileNameNumber;
  std::string mId;
  std::string mVFName;
  std::string mOverlayName;
  std::string mFusionName;
  std::string mVFId;
  std::string mLastError;
  std::list<std::string> mLinkedId;

  vvLandmarks* mLandmarks;
  
  std::vector<int> mPreviousSlice;
  std::vector<int> mPreviousTSlice;
};

#endif
