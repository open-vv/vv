/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
#ifndef vvSlicerManager_h
#define vvSlicerManager_h

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
#include "vvConstants.h"
#include "clitkCommon.h"
#include "vvImage.h"
#include "vvMesh.h"
class vvSlicer;
class vvInteractorStyleNavigator;
class vvImageReader;
class vvImageReader;
class vvLandmarks;

class vvSlicerManager : public QObject {
    Q_OBJECT

public:
    vvSlicerManager(int numberOfSlicers);
    ~vvSlicerManager();

    std::string GetLastError() {
        return mLastError;
    }

    bool SetImage(std::string filename,LoadedImageType type);
    void SetImage(vvImage::Pointer image);
    void SetExtractedImage(std::string filename, vvImage::Pointer image, int slice);
    bool SetImages(std::vector<std::string> filenames,LoadedImageType type);

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

    std::string GetFileName() {
        return mFileName;
    }
    std::string GetVFName() {
        return mVFName;
    }
    std::string GetOverlayName() {
        return mOverlayName;
    }
    std::string GetFusionName() {
        return mFusionName;
    }

    ///Switch between nearest neighbor and linear interpolation
    void ToggleInterpolation();
    vvSlicer* GetSlicer(int i);
    void UpdateSlicer(int num, bool state);
    void SetSlicerWindow(int i, vtkRenderWindow* RW);
    void SetInteractorStyleNavigator(int i,vtkInteractorStyle* style);
    int NumberOfSlicers() {
        return mSlicers.size();
    }
    vvImage::Pointer GetImage() {
        return mImage;
    }
    vvImage::Pointer GetVF() {
        return mVF;
    }
    int GetDimension() {
        if (mImage) return mImage->GetNumberOfDimensions();
        else return -1;
    }

    int GetType() {
        return mType;
    }

    void SetId(std::string id) {
        mId = id;
    }
    std::string GetId() {
        return mId;
    }

    void SetFilename(std::string f) {
        mFileName = f;
    }

    void SetTSlice(int slice);
    void SetNextTSlice(int originating_slicer);
    void SetPreviousTSlice(int originating_slicer);
    void SetTSliceInSlicer(int tslice, int slicer);

    void GenerateDefaultLookupTable();
    void SetColorWindow(double s);
    void SetColorLevel(double s);
    void SetOpacity(int i, double factor);
    void SetColorMap();
    void SetColorMap(int colormap);
    void SetPreset(int preset);
    void SetOverlayColor(int color) {
        mOverlayColor = color;
    }
    void SetFusionOpacity(int opacity) {
        mFusionOpacity = opacity;
    }
    void SetFusionColorMap(int colorMap) {
        mFusionColorMap = colorMap;
    }
    void SetFusionWindow(int window) {
        mFusionWindow = window;
    }
    void SetFusionLevel(int level) {
        mFusionLevel = level;
    }

    double GetColorWindow();
    double GetColorLevel();
    int GetColorMap() {
        return mColorMap;
    }
    int GetPreset() {
        return mPreset;
    }
    int GetOverlayColor() {
        return mOverlayColor;
    }

    int GetFusionOpacity() {
        return mFusionOpacity;
    }
    int GetFusionColorMap() {
        return mFusionColorMap;
    }
    double GetFusionWindow() {
        return mFusionWindow;
    }
    double GetFusionLevel() {
        return mFusionLevel;
    }

    void SetCursorVisibility(int s);
    void UpdateViews(int current, int slicer);
    void UpdateLinked(int slicer);
    void Render();

    void AddLink(std::string newId) {
        mLinkedId.push_back(newId);
    }
    void RemoveLink(std::string oldId) {
        mLinkedId.remove(oldId);
    }

    ///Remove the actor defined by its type and index (example: 3rd contour)
    void RemoveActor(const std::string& actor_type, int overlay_index);
    void RemoveActors();
    void Reload();
    void ReloadOverlay();
    void ReloadFusion();
    void ReloadVF();

    void Activated();
    void UpdateInfoOnCursorPosition(int slicer);
    void UpdateWindowLevel();
    void UpdateSlice(int slicer);
    void UpdateTSlice(int slicer);
    void UpdateSliceRange(int slicer);

    vvLandmarks *GetLandmarks();
    void AddLandmark(float x,float y,float z,float t);

signals :
    void currentImageChanged(std::string id);
    void UpdatePosition(int visibility,double x, double y, double z, double X, double Y, double Z, double value);
    void UpdateVector(int display, double x, double y, double z, double value);
    void UpdateOverlay(int display, double valueOver, double valueRef);
    void UpdateFusion(int display, double valueFus);
    void UpdateWindows(int slicer, int view, int slice);
    void UpdateSlice(int slicer, int slice);
    void UpdateTSlice(int slicer, int slice);
    void UpdateSliceRange(int slice, int min, int max, int tmin, int tmax);
    void WindowLevelChanged(double window, double level, int preset, int colormap);
    void UpdateLinkManager(std::string, int slicer, double x, double y, double z, int temps);
    void LandmarkAdded();

protected:
    std::vector<vvSlicer*> mSlicers;
    vvImageReader* mReader;
    vvImageReader* mOverlayReader;
    vvImageReader* mFusionReader;
    vvImage::Pointer mImage;
    vvImage::Pointer mVF;
    vvImageReader* mVectorReader;
    int mColorMap;
    int mOverlayColor;

    int mFusionOpacity;
    int mFusionColorMap;
    double mFusionWindow;
    double mFusionLevel;

    int mPreset;
    LoadedImageType mType;
    std::string mVFComponent;
    std::string mOverlayComponent;
    std::string mFusionComponent;
    std::string mFileName;
    std::string mId;
    std::string mVFName;
    std::string mOverlayName;
    std::string mFusionName;
    std::string mVFId;
    std::string mLastError;
    std::list<std::string> mLinkedId;

    vvLandmarks* mLandmarks;
};

#endif
