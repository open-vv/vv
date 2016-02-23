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
#ifndef __vvSlicer_h
#define __vvSlicer_h
#include <iostream>
#include <vector>

#include <QObject>
#include <QString> //TODO delete
#include <QMessageBox>

#include "vvLandmarks.h"
#include "vvImage.h"
#include "vvMesh.h"
#include "vvMeshActor.h"
#include "vvClipPolyData.h"

#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToColors.h>
#include <vtkCaptionActor2D.h>
#include <vtkTransformPolyDataFilter.h>

class vtkActor;
class vtkActor2D;
class vtkCursor2D;
class vtkPolyDataMapper2D;
class vtkProperty2D;
class vtkClipPolyData;
class vtkImageActor;
class vvBlendImageActor;
class vtkBox;
class vtkCornerAnnotation;
class vtkExtractVOI;
class vtkPolyDataMapper2D;
class vtkPolyDataMapper;
class vtkGlyph3D;
class vvGlyph2D;
class vvGlyphSource;
class vtkCursor3D;
class vtkCutter;
class vtkAssignAttribute;
class vtkScalarBarActor;
class vtkTransform;
class vtkImageReslice;

class vvSlicer: public QObject, public vtkImageViewer2 {
  
Q_OBJECT

public:
  static vvSlicer *New();
  vtkTypeMacro(vvSlicer,vtkImageViewer2);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetImage(vvImage::Pointer inputImages);
  vvImage::Pointer GetImage() {
    return mImage;
  }

  void SetOverlay(vvImage::Pointer inputOverlay);
  vvImage::Pointer GetOverlay() {
    return mOverlay;
  }
  vtkImageMapToWindowLevelColors* GetOverlayMapper(); 
  vvBlendImageActor* GetOverlayActor() ;
  vtkImageMapToColors* GetFusionMapper() ;
  vtkImageActor* GetFusionActor() ;
  vtkActor* GetVFActor() ;
  vtkCornerAnnotation* GetAnnotation();
    
  void SetFusion(vvImage::Pointer inputFusion, int fusionSequenceCode = -1);
  vvImage::Pointer GetFusion() {
    return mFusion;
  }
  void ShowFusionLegend(bool show) { showFusionLegend = show; }

  /**Get/Set an actor's visibility ("overlay, fusion, vf, contour...")
     Overlay index is the index of the overlay by type, eg. if there are
     5 contours and we want to activate the 3rd one, pass 2 **/
  bool GetActorVisibility(const std::string& actor_type, int overlay_index);
  void SetActorVisibility(const std::string& actor_type, int overlay_index,bool vis);
  void RemoveActor(const std::string& actor_type, int overlay_index);

  void SetVF(vvImage::Pointer vf);
  vvImage *GetVF() {
    return mVF;
  }

  void SetLandmarks(vvLandmarks* landmarks);
  void SetTSlice(int t, bool updateLinkedImages = true);

  void SetFusionSequenceTSlice(int t);

  void SetSliceOrientation(int orientation);
  void AdjustResliceToSliceOrientation(vtkImageReslice *reslice);
  int GetTSlice();
  int GetFusionTSlice();
  int GetOverlayTSlice();
  int GetMaxCurrentTSlice();
  ///Reimplemented from vtkImageViewer2 to add polydata support
  void SetSlice(int s);
  int GetTMax();

  void SetOpacity(double s);
  void SetRenderWindow(int orientation, vtkRenderWindow * rw);
  void SetDisplayMode(bool i);
  void FlipHorizontalView();
  void FlipVerticalView();
  static double GetScalarComponentAsDouble(vtkImageData *image, double X, double Y, double Z, int &ix, int &iy, int &iz, int component=0);
  void Render();
  ///Sets the camera to fit the image in the window
  void ResetCamera();

  void SetVFSubSampling(int sub);
  int GetVFSubSampling() {
    return mSubSampling;
  }
  void SetVFScale(int scale);
  void SetVFWidth(int width);
  int GetVFScale() {
    return mScale;
  }
  void SetVFLog(int log);
  int GetVFLog() {
    return mVFLog;
  }

  void SetFileName(std::string filename) {
    mFileName = filename;
  }
  std::string GetFileName() {
    return mFileName;
  }

  double* GetCursorPosition() {
    return mCursor;
  }
  vtkTransform * GetSlicingTransform() { return mSlicingTransform; }
  vtkTransform * GetConcatenatedTransform() { return mConcatenatedTransform; }
  vtkTransform * GetConcatenatedFusionTransform() { return mConcatenatedFusionTransform; }
  vtkTransform * GetConcatenatedOverlayTransform() { return mConcatenatedOverlayTransform; }

  void SetCurrentPosition(double x, double y, double z, int t);
  double* GetCurrentPosition();

  void UpdateCursorPosition();
  void SetCursorVisibility(bool s);

  bool GetCursorVisibility();
  void SetCursorColor(int r,int g, int b);

  void SetCornerAnnotationVisibility(bool s);
  bool GetCornerAnnotationVisibility();

  void GetExtremasAroundMousePointer(double & min, double & max, vtkImageData *image, vtkTransform *transform);

  void RemoveLandmarks();
  void DisplayLandmarks();
  void ForceUpdateDisplayExtent();

  int* GetDisplayExtent();
  /**Add a polydata to be displayed as a contour over the image
   ** the contour can be propagated to a time sequence using a motion field */
  void AddContour(vvMesh::Pointer contours,bool propagate);
  ///Toggle temporal superposition of contours
  void ToggleContourSuperposition();

  virtual void SetColorWindow(double s);
  virtual void SetColorLevel(double s);

  double GetOverlayColorWindow();
  double GetOverlayColorLevel();
  bool GetLinkOverlayWindowLevel() { return mLinkOverlayWindowLevel; }

  void SetOverlayColorWindow(double s);
  void SetOverlayColorLevel(double s);
  void SetLinkOverlayWindowLevel(bool b) { mLinkOverlayWindowLevel = b; }

  /**
   * When it is enabled, beware of a call to GetExtent.
   * we must have setted mReducedExtent otherwhise random values
   * are returned by GetExtent
   * */
  void EnableReducedExtent(bool b);
  void SetReducedExtent(int * ext);

  bool ClipDisplayedExtent(int extent[6], int refExtent[6]);
  int GetOrientation();
  int * GetExtent();

  double* GetVFColor() {
    return mVFColor;
  }
  void SetVFColor(double r, double g, double b);
  
  //necessary to flag the secondary sequence
  void SetFusionSequenceCode(int code) {mFusionSequenceCode=code;}
  void SetRegisterExtent(int [6]);
  void GetRegisterExtent(int [6]);
  
  void SetSlicerNumber(const int nbSlicer) {mSlicerNumber = nbSlicer;}
  int GetSlicerNumber() const {return mSlicerNumber;}
  
signals:
  void UpdateDisplayExtentBegin(int);
  void UpdateDisplayExtentEnd(int);
  
protected:
  vvSlicer();
  ~vvSlicer();

  std::string mFileName;
  vvImage::Pointer mImage;
  vvImage::Pointer mOverlay;
  vvImage::Pointer mFusion;
  vvImage::Pointer mVF;

  vvLandmarks* mLandmarks;

  int mFusionSequenceCode; //-1: not involved in a fusion sequence, 0: main sequence (CT), 1: secondary sequence (US)

  //                         __________ Image coordinates accounting for spacing and origin
  //                            Λ  Λ
  //                            |  | vvImage.GetTransform()
  //                            |  |
  // GetConcatenatedTransform() | _|___ VV world coordinates (mm) (displayed in VV)             mCurrentBeforeSlicingTransform
  //                            |  Λ
  //                            |  | GetSlicingTransform()
  //                            |  |
  //                         ___|__|___ VTK world coordinates (mm) (never displayed)            mCurrent

  vtkSmartPointer<vtkTransform> mSlicingTransform;
  vtkSmartPointer<vtkTransformPolyDataFilter> mLandmarkTransform;
  vtkSmartPointer<vtkImageReslice> mImageReslice;
  vtkSmartPointer<vtkTransform> mConcatenatedTransform;
  vtkSmartPointer<vtkImageReslice> mOverlayReslice;
  vtkSmartPointer<vtkTransform> mConcatenatedOverlayTransform;
  vtkSmartPointer<vtkImageMapToWindowLevelColors> mOverlayMapper;
  vtkSmartPointer<vvBlendImageActor> mOverlayActor;
  vtkSmartPointer<vtkImageReslice> mFusionReslice;
  vtkSmartPointer<vtkTransform> mConcatenatedFusionTransform;
  vtkSmartPointer<vtkImageMapToColors> mFusionMapper;
  vtkSmartPointer<vtkImageActor> mFusionActor;
  vtkSmartPointer<vtkCornerAnnotation> ca;
  vtkSmartPointer<vtkCursor2D> crossCursor;
  vtkSmartPointer<vtkPolyDataMapper2D> pdm;
  vtkSmartPointer<vtkActor2D> pdmA;
  vtkSmartPointer<vvGlyphSource> mArrow;
  vtkSmartPointer<vtkAssignAttribute> mAAFilter;
  vtkSmartPointer<vtkExtractVOI> mVOIFilter;
  vtkSmartPointer<vvGlyph2D> mGlyphFilter;
  vtkSmartPointer<vtkPolyDataMapper> mVFMapper;
  vtkSmartPointer<vtkLookupTable> mVFColorLUT;
  vtkSmartPointer<vtkActor> mVFActor;
  vtkSmartPointer<vtkGlyph3D> mLandGlyph;
  vtkSmartPointer<vtkCursor3D> mCross;
  vtkSmartPointer<vvClipPolyData> mLandClipper;
  vtkSmartPointer<vtkPolyDataMapper> mLandMapper;
  vtkSmartPointer<vtkActor> mLandActor;
  std::vector<vtkSmartPointer<vtkCaptionActor2D> > mLandLabelActors;
  vtkSmartPointer<vtkBox> mClipBox;
  vtkSmartPointer<vtkScalarBarActor> legend;
  std::vector<vvMeshActor*> mSurfaceCutActors;

  int mSlicerNumber;
  int mCurrentTSlice;
  int mCurrentFusionTSlice;
  int mCurrentOverlayTSlice;
  double mCurrent[3];
  double mCurrentBeforeSlicingTransform[3];
  double mCursor[4];
  int mSubSampling;
  int mScale;
  int mVFLog;
  int mVFWidth;
  double mVFColor[3];
  bool mUseReducedExtent;
  int * mReducedExtent;
  int * mRegisterExtent;
  bool mLinkOverlayWindowLevel;
  bool showFusionLegend;

private:
  void UpdateOrientation();
  void UpdateDisplayExtent();
  void ConvertImageToImageDisplayExtent(vtkInformation *sourceImage, const int sourceExtent[6],
                                        vtkImageData *targetImage, int targetExtent[6]);
  void ConvertImageToImageDisplayExtent(vtkImageData *sourceImage, const int sourceExtent[6],
                                        vtkImageData *targetImage, int targetExtent[6]);
  ///Sets the surfaces to be cut on the image slice: update the vtkCutter
  void SetContourSlice();

  // Visibility of the different elements that can be set from outside the object.
  // Note that vvSlicer also check if the element is to be displayed according to
  // the extent of the displayed object.
  // These members have been introduced to fix Bug #1883.
  bool mImageVisibility;
  bool mOverlayVisibility;
  bool mFusionVisibility;
  bool mVFVisibility;
  bool mFirstSetSliceOrientation;
};
#endif
