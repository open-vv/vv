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
#ifndef __vvSlicer_h
#define __vvSlicer_h
#include <iostream>
#include <vector>

#include "vvLandmarks.h"
#include "vvImage.h"
#include "vtkImageViewer2.h"
#include "vvMesh.h"
#include <vvMeshActor.h>
#include <vtkSmartPointer.h>

class vtkActor;
class vtkActor2D;
class vtkCursor2D;
class vtkPolyDataMapper2D;
class vtkProperty2D;
class vtkClipPolyData;
class vtkImageActor;
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

class vvSlicer: public vtkImageViewer2
{
public:
    static vvSlicer *New();
    vtkTypeRevisionMacro(vvSlicer,vtkImageViewer2);
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
    vtkImageActor* GetOverlayActor() ;
    vtkImageMapToWindowLevelColors* GetFusionMapper() ;
    vtkImageActor* GetFusionActor() ;
    vtkActor* GetVFActor() ;
    vtkCornerAnnotation* GetAnnotation();
    
    void SetFusion(vvImage::Pointer inputFusion);
    vvImage::Pointer GetFusion() {
        return mFusion;
    }

    /**Set an actor's visibility ("overlay, fusion, vf, contour...")
       Overlay index is the index of the overlay by type, eg. if there are
       5 contours and we want to activate the 3rd one, pass 2 **/
    void SetActorVisibility(const std::string& actor_type, int overlay_index,bool vis);
    void RemoveActor(const std::string& actor_type, int overlay_index);

    void SetVF(vvImage::Pointer vf);
    vvImage *GetVF() {
        return mVF;
    }

    void SetLandmarks(vvLandmarks* landmarks);
    void SetTSlice(int t);
    void SetSliceOrientation(int orientation);
    int GetTSlice();
    ///Reimplemented from vtkImageViewer2 to add polydata support
    void SetSlice(int s);
    int GetTMax() {
        return mImage->GetVTKImages().size() - 1;
    }

    void SetOpacity(double s);
    void SetRenderWindow(int orientation, vtkRenderWindow * rw);
    void SetDisplayMode(bool i);
    void FlipHorizontalView();
    void FlipVerticalView();
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

    void SetCurrentPosition(double x, double y, double z, int t);
    double* GetCurrentPosition() {
        return mCurrent;
    }

    void UpdateCursorPosition();
    void SetCursorVisibility(bool s);
    bool GetCursorVisibility();
    void SetCursorColor(int r,int g, int b);

    void GetExtremasAroundMousePointer(double & min, double & max);

    void UpdateLandmarks();
    void ForceUpdateDisplayExtent();

    int* GetDisplayExtent();
    /**Add a polydata to be displayed as a contour over the image
    ** the contour can be propagated to a time sequence using a motion field */
    void AddContour(vvMesh::Pointer contours,bool propagate);
    ///Toggle temporal superposition of contours
    void ToggleContourSuperposition();

    virtual void SetColorWindow(double s);
    virtual void SetColorLevel(double s);

    
    void EnableReducedExtent(bool b);
    void SetReducedExtent(int * ext);

    void ClipDisplayedExtent(int extent[6], int refExtent[6]);
    int GetOrientation();
    int * GetExtent();

protected:
    vvSlicer();
    ~vvSlicer();

    std::string mFileName;
    vvImage::Pointer mImage;
    vvImage::Pointer mOverlay;
    vvImage::Pointer mFusion;
    vvImage::Pointer mVF;

    vvLandmarks* mLandmarks;

    vtkSmartPointer<vtkImageMapToWindowLevelColors> mOverlayMapper;
    vtkSmartPointer<vtkImageActor> mOverlayActor;
    vtkSmartPointer<vtkImageMapToWindowLevelColors> mFusionMapper;
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
    vtkSmartPointer<vtkActor> mVFActor;
    vtkSmartPointer<vtkGlyph3D> mLandGlyph;
    vtkSmartPointer<vtkCursor3D> mCross;
    vtkSmartPointer<vtkClipPolyData> mLandClipper;
    vtkSmartPointer<vtkPolyDataMapper> mLandMapper;
    vtkSmartPointer<vtkActor> mLandActor;
    vtkSmartPointer<vtkBox> mClipBox;
    vtkSmartPointer<vtkScalarBarActor> legend;
    std::vector<vvMeshActor*> mSurfaceCutActors;

    int mCurrentTSlice;
    double mCurrent[3];
    double mCursor[4];
    int mSubSampling;
    int mScale;
    int mVFLog;
    int mVFWidth;
    bool mUseReducedExtent;
    int * mReducedExtent;
    int * mInitialExtent;

private:
    void UpdateOrientation();
    void UpdateDisplayExtent();
    void ComputeVFDisplayedExtent(int x1,int x2,int y1,int y2,int z1,int z2,int extent[6]);
    void ComputeOverlayDisplayedExtent(int x1,int x2,int y1,int y2,int z1,int z2,int overExtent[6]);
    void ComputeFusionDisplayedExtent(int x1,int x2,int y1,int y2,int z1,int z2,int overExtent[6]);
    ///Sets the surfaces to be cut on the image slice: update the vtkCutter
    void SetContourSlice();


};
#endif
