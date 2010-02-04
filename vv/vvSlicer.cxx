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
#include "vvSlicer.h"

#include "vvImage.h"
#include "vvImage.h"
#include "vvSlicerManagerCommand.h"
#include "vvGlyphSource.h"
#include "vvGlyph2D.h"
#include "vvImageMapToWLColors.h"

#include <vtkTextProperty.h>
#include <vtkTextActor.h>
#include <vtkTextSource.h>
#include <vtkActor2D.h>
#include <vtkCursor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkToolkits.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkClipPolyData.h>
#include <vtkGlyph3D.h>
#include <vtkMath.h>
#include <vtkCursor3D.h>
#include <vtkProperty.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>

#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkPolyDataMapper.h>
#include <vtkBox.h>

#include <vtkExtractVOI.h>
#include <vtkSphereSource.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkAssignAttribute.h>

vtkCxxRevisionMacro(vvSlicer, "DummyRevision");
vtkStandardNewMacro(vvSlicer);

vvSlicer::vvSlicer()
{
    mImage = NULL;
    mCurrentTSlice = 0;

    mCurrent[0] = -VTK_DOUBLE_MAX;
    mCurrent[1] = -VTK_DOUBLE_MAX;
    mCurrent[2] = -VTK_DOUBLE_MAX;

    mCursor[0] = -VTK_DOUBLE_MAX;
    mCursor[1] = -VTK_DOUBLE_MAX;
    mCursor[2] = -VTK_DOUBLE_MAX;
    mCursor[3] = -VTK_DOUBLE_MAX;

    mSubSampling = 5;
    mScale = 1;
    mVFLog = 0;

    std::string text = "F1 = sagital; F2 = coronal; F3 = axial\n";
    text += "F5 = horizontal flip; F6 = vertical flip\n\n";
    text += "0,1,2,3,4,5 : preset windowing\n";
    text += "6,7,8,9 : preset colormap\n";
    text += "r : reset view\n";
    text += "l : reload image\n";
    text += "f : fly to mouse position\n";
    text += "g : go to cross hair position\n\n";
    text += "Up,down : change slice\n";
    text += "Left,right : change tenporal slice\n\n";
    text += "Scrollbar (or w/x) : zoom in/out\n";
    text += "left button : synchronize all views\n";
    text += "middle button : grab image\n";
    text += "right button : change windowing\n";

    crossCursor = vtkCursor2D::New();
    crossCursor->AllOff();
    crossCursor->AxesOn();
    crossCursor->SetTranslationMode(1);
    crossCursor->SetRadius(2);

    pdm = vtkPolyDataMapper2D::New();
    pdm->SetInput(crossCursor->GetOutput());

    pdmA = vtkActor2D::New();
    pdmA->SetMapper(pdm);
    pdmA->GetProperty()->SetColor(255,10,212);
    pdmA->SetVisibility(0);
    pdmA->SetPickable(0);

    ca = vtkCornerAnnotation::New();
    ca->GetTextProperty()->SetColor(255,10,212);
    ca->SetVisibility(1);
    mFileName = "";

    mVF = NULL;
    mOverlay = NULL;
    mFusion = NULL;
    mLandmarks = NULL;

    legend = vtkSmartPointer<vtkScalarBarActor>::New();
    //legend->SetTitle("test!");
    legend->SetPosition(0.82,0.18);
    legend->SetWidth(0.1);
    legend->SetVisibility(0);
    legend->SetLabelFormat("%.1f");
    this->GetRenderer()->AddActor(legend);

    this->WindowLevel->Delete();
    this->WindowLevel = vvImageMapToWLColors::New();
    this->InstallPipeline();
}

vtkImageMapToWindowLevelColors* vvSlicer::GetOverlayMapper() {
    return mOverlayMapper.GetPointer();
}
vtkImageActor* vvSlicer::GetOverlayActor() {
    return mOverlayActor.GetPointer();
}
vtkImageMapToWindowLevelColors* vvSlicer::GetFusionMapper() {
    return mFusionMapper.GetPointer();
}
    
vtkImageActor* vvSlicer::GetFusionActor() {
    return mFusionActor.GetPointer();
}
vtkActor* vvSlicer::GetVFActor() {
    return mVFActor.GetPointer();
}
vtkCornerAnnotation* vvSlicer::GetAnnotation() {
    return ca.GetPointer();
}

void vvSlicer::AddContour(vvMesh::Pointer contour,bool propagate)
{

    mSurfaceCutActors.push_back(new vvMeshActor());
    if (propagate)
        mSurfaceCutActors.back()->Init(contour,mCurrentTSlice,mVF);
    else
        mSurfaceCutActors.back()->Init(contour,mCurrentTSlice);
    mSurfaceCutActors.back()->SetSlicingOrientation(SliceOrientation);
    this->GetRenderer()->AddActor(mSurfaceCutActors.back()->GetActor());

    SetContourSlice();
}

void vvSlicer::ToggleContourSuperposition()
{
    for (std::vector<vvMeshActor*>::iterator i=mSurfaceCutActors.begin();
            i!=mSurfaceCutActors.end();i++)
        (*i)->ToggleSuperposition();
}

void vvSlicer::SetCursorColor(int r,int g, int b)
{
    pdmA->GetProperty()->SetColor(r,g,b);
}
void vvSlicer::SetCursorVisibility(bool s)
{
    pdmA->SetVisibility(s);
}
bool vvSlicer::GetCursorVisibility()
{
    return pdmA->GetVisibility();
}

vvSlicer::~vvSlicer()
{
    for (std::vector<vvMeshActor*>::iterator i=mSurfaceCutActors.begin();
            i!=mSurfaceCutActors.end();i++)
        delete (*i);
}

void vvSlicer::SetCurrentPosition(double x, double y, double z, int t)
{
    mCurrent[0] = x;
    mCurrent[1] = y;
    mCurrent[2] = z;
    mCurrentTSlice = t;
}

void vvSlicer::SetImage(vvImage::Pointer image)
{
    if (image->GetVTKImages().size())
    {
        mImage = image;
        this->Superclass::SetInput(image->GetVTKImages()[0]);
        this->UpdateDisplayExtent();
        mCurrentTSlice = 0;
        ca->SetText(0,mFileName.c_str());
    }
}

void vvSlicer::SetOverlay(vvImage::Pointer overlay)
{
    if (overlay->GetVTKImages().size())
    {
        mOverlay = overlay;

        if (!mOverlayMapper)
            mOverlayMapper = vtkImageMapToWindowLevelColors::New();
        mOverlayMapper->SetInput(overlay->GetVTKImages()[0]);

        if (!mOverlayActor)
        {
            mOverlayActor = vtkImageActor::New();
            mOverlayActor->SetInput(mOverlayMapper->GetOutput());
            mOverlayActor->SetPickable(0);
            mOverlayActor->SetVisibility(false);
            mOverlayActor->SetOpacity(0.5);
            this->UpdateDisplayExtent();
        }

        //stupid but necessary : the Overlay need to be rendered before fusion
        if (mFusionActor)
        {
            this->GetRenderer()->RemoveActor(mFusionActor);
            this->GetRenderer()->AddActor(mOverlayActor);
            this->GetRenderer()->AddActor(mFusionActor);
        }
        else
            this->GetRenderer()->AddActor(mOverlayActor);

        //Synchronize slice
        SetTSlice(mCurrentTSlice);
    }
}

void vvSlicer::SetFusion(vvImage::Pointer fusion)
{
    if (fusion->GetVTKImages().size())
    {
        mFusion = fusion;

        if (!mFusionMapper)
            mFusionMapper = vtkImageMapToWindowLevelColors::New();
        mFusionMapper->SetInput(fusion->GetVTKImages()[0]);

        if (!mFusionActor)
        {
            mFusionActor = vtkImageActor::New();
            mFusionActor->SetInput(mFusionMapper->GetOutput());
            mFusionActor->SetPickable(0);
            mFusionActor->SetVisibility(false);
            mFusionActor->SetOpacity(0.7);
            this->UpdateDisplayExtent();
            this->GetRenderer()->AddActor(mFusionActor);
        }

        //Synchronize slice
        SetTSlice(mCurrentTSlice);
    }
}

void vvSlicer::SetActorVisibility(const std::string& actor_type, int overlay_index ,bool vis)
{
    if (actor_type == "vector")
    {
        this->mVFActor->SetVisibility(vis);
    }
    if (actor_type == "overlay")
    {
        this->mOverlayActor->SetVisibility(vis);
    }
    if (actor_type == "fusion")
    {
        this->mFusionActor->SetVisibility(vis);
    }
    if (actor_type == "contour")
        this->mSurfaceCutActors[overlay_index]->GetActor()->SetVisibility(vis);
    UpdateDisplayExtent();
}

void vvSlicer::SetVF(vvImage::Pointer vf)
{
    if (vf->GetVTKImages().size())
    {
        mVF = vf;

        if (!mAAFilter)
        {
            mAAFilter=vtkAssignAttribute::New();
            mVOIFilter = vtkExtractVOI::New();
            mVOIFilter->SetSampleRate(mSubSampling,mSubSampling,mSubSampling);
        }
        mVOIFilter->SetInput(vf->GetVTKImages()[0]);
        mAAFilter->SetInput(mVOIFilter->GetOutput());
        ///This tells VTK to use the scalar (pixel) data of the image to draw the little arrows
        mAAFilter->Assign(vtkDataSetAttributes::SCALARS, vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

        if (!mArrow)
            mArrow = vvGlyphSource::New();
        mArrow->SetGlyphTypeToSpecificArrow();
        mArrow->SetScale(mScale);
        mArrow->FilledOff();

        // Glyph the gradient vector (with arrows)
        if (!mGlyphFilter)
            mGlyphFilter = vvGlyph2D::New();
        mGlyphFilter->SetInput(mAAFilter->GetOutput());
        mGlyphFilter->SetSource(mArrow->GetOutput());
        mGlyphFilter->ScalingOn();
        mGlyphFilter->SetScaleModeToScaleByVector();
        mGlyphFilter->OrientOn();
        mGlyphFilter->SetVectorModeToUseVector();
        mGlyphFilter->SetColorModeToColorByVector();

        if (!mVFMapper)
            mVFMapper = vtkPolyDataMapper::New();
        //mVFMapper->SetInputConnection(mGlyphFilter->GetOutputPort());
        mVFMapper->SetInput(mGlyphFilter->GetOutput());
        mVFMapper->ImmediateModeRenderingOn();

        if (!mVFActor)
            mVFActor = vtkActor::New();
        mVFActor->SetMapper(mVFMapper);
        mVFActor->SetPickable(0);
        this->UpdateDisplayExtent();
        this->GetRenderer()->AddActor(mVFActor);

        //Synchronize slice
        SetTSlice(mCurrentTSlice);
    }
}

void vvSlicer::SetLandmarks(vvLandmarks* landmarks)
{
    mLandmarks = landmarks;
    if (landmarks)
    {

        if (!mCross)
            mCross = vtkCursor3D::New();
        mCross->SetFocalPoint(0.0,0.0,0.0);
        mCross->SetModelBounds(-10,10,-10,10,-10,10);
        mCross->AllOff();
        mCross->AxesOn();

        if (!mLandGlyph)
            mLandGlyph = vtkGlyph3D::New();
        mLandGlyph->SetSource(mCross->GetOutput());
        mLandGlyph->SetInput(landmarks->GetOutput());
        //mLandGlyph->SetIndexModeToScalar();
        mLandGlyph->SetRange(0,1);
        mLandGlyph->ScalingOff();

        mLandGlyph->SetColorModeToColorByScalar();

        if (!mClipBox)
            mClipBox = vtkBox::New();
        if (!mLandClipper)
            mLandClipper = vtkClipPolyData::New();
        mLandClipper->InsideOutOn();
        mLandClipper->SetInput(mLandGlyph->GetOutput());
        mLandClipper->SetClipFunction(mClipBox);

        if (!mLandMapper)
            mLandMapper = vtkPolyDataMapper::New();
        mLandMapper->SetInputConnection(mLandClipper->GetOutputPort());
        //mLandMapper->ScalarVisibilityOff();

        if (!mLandActor)
            mLandActor = vtkActor::New();
        mLandActor->SetMapper(mLandMapper);
        mLandActor->GetProperty()->SetColor(255,10,212);
        mLandActor->SetPickable(0);
        mLandActor->SetVisibility(true);
        this->UpdateDisplayExtent();
        this->GetRenderer()->AddActor(mLandActor);
    }
}

//FIXME: this function leaks memory, we should fix it someday :)
void vvSlicer::RemoveActor(const std::string& actor_type, int overlay_index)
{
    if (actor_type == "vector")
    {
        Renderer->RemoveActor(mVFActor);
        mGlyphFilter=NULL;
        mVF = NULL;
        mArrow = NULL;
        mAAFilter=NULL;
        mVOIFilter = NULL;
        mVFMapper = NULL;
        mVFActor = NULL;
    }
    if (actor_type == "overlay")
    {
        Renderer->RemoveActor(mOverlayActor);
        mOverlay = NULL;
        mOverlayActor = NULL;
        mOverlayMapper = NULL;
    }
    if (actor_type == "fusion")
    {
        Renderer->RemoveActor(mFusionActor);
        mFusion = NULL;
        mFusionActor = NULL;
        mFusionMapper = NULL;
    }
    if (actor_type == "contour")
    {
        Renderer->RemoveActor(this->mSurfaceCutActors[overlay_index]->GetActor());
        mSurfaceCutActors.erase(mSurfaceCutActors.begin()+overlay_index);
    }
}

void vvSlicer::SetVFSubSampling(int sub)
{
    if (mVOIFilter)
    {
        mVOIFilter->SetSampleRate(mSubSampling,mSubSampling,mSubSampling);
        mSubSampling = sub;
    }
    UpdateDisplayExtent();
    Render();
}

void vvSlicer::SetVFScale(int scale)
{
    mScale = scale;
    if (mArrow)
        mArrow->SetScale(mScale);
    UpdateDisplayExtent();
    Render();
}

void vvSlicer::SetVFLog(int log)
{
    mVFLog = log;
    if (mGlyphFilter)
    {
        mGlyphFilter->SetUseLog(mVFLog);
        mGlyphFilter->Modified();
    }
    UpdateDisplayExtent();
    Render();
}

void vvSlicer::SetTSlice(int t)
{
    if (t < 0)
        t = 0;
    else if ((unsigned int)t >= mImage->GetVTKImages().size())
        t = mImage->GetVTKImages().size() -1;
    mCurrentTSlice = t;
    this->SetInput(mImage->GetVTKImages()[t]);
    if (mVF && mVFActor->GetVisibility())
    {
        if (mVF->GetVTKImages().size() > (unsigned int)mCurrentTSlice)
            mVOIFilter->SetInput(mVF->GetVTKImages()[mCurrentTSlice]);
    }
    if (mOverlay && mOverlayActor->GetVisibility())
    {
        if (mOverlay->GetVTKImages().size() > (unsigned int)mCurrentTSlice)
            mOverlayMapper->SetInput(mOverlay->GetVTKImages()[mCurrentTSlice]);
    }
    if (mFusion && mFusionActor->GetVisibility())
    {
        if (mFusion->GetVTKImages().size() > (unsigned int)mCurrentTSlice)
            mFusionMapper->SetInput(mFusion->GetVTKImages()[mCurrentTSlice]);
    }
    if (mSurfaceCutActors.size() > 0)
        for (std::vector<vvMeshActor*>::iterator i=mSurfaceCutActors.begin();
                i!=mSurfaceCutActors.end();i++)
            (*i)->SetTimeSlice(mCurrentTSlice);
    UpdateDisplayExtent();
}

int vvSlicer::GetTSlice()
{
    return mCurrentTSlice;
}

void vvSlicer::SetSliceOrientation(int orientation)
{
    //if 2D image, force to watch in Axial View
    int extent[6];
    this->GetInput()->GetWholeExtent(extent);
    if (extent[5]-extent[4] <= 2)
        orientation=2;

    if (orientation < vtkImageViewer2::SLICE_ORIENTATION_YZ ||
            orientation > vtkImageViewer2::SLICE_ORIENTATION_XY)
    {
        vtkErrorMacro("Error - invalid slice orientation " << orientation);
        return;
    }

    this->SliceOrientation = orientation;
    
    // Update the viewer
    int *range = this->GetSliceRange();
    if (range)
        this->Slice = static_cast<int>((range[0] + range[1]) * 0.5);

    this->UpdateOrientation();
    this->UpdateDisplayExtent();

    if (this->Renderer && this->GetInput())
    {
        double scale = this->Renderer->GetActiveCamera()->GetParallelScale();
        this->Renderer->ResetCamera();
        this->Renderer->GetActiveCamera()->SetParallelScale(scale);
    }

    SetContourSlice();
}

//----------------------------------------------------------------------------
void vvSlicer::UpdateDisplayExtent()
{
    vtkImageData *input = this->GetInput();
    if (!input || !this->ImageActor)
    {
        return;
    }
    input->UpdateInformation();
    int *w_ext = input->GetWholeExtent();

    switch (this->SliceOrientation)
    {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            this->ImageActor->SetDisplayExtent(
                    w_ext[0], w_ext[1], w_ext[2], w_ext[3], this->Slice, this->Slice);
            if (mVF && mVFActor->GetVisibility())
            {
                int vfExtent[6];
                ComputeVFDisplayedExtent(w_ext[0], w_ext[1], w_ext[2], w_ext[3], this->Slice, this->Slice,vfExtent);
                mVOIFilter->SetVOI(vfExtent);
                mGlyphFilter->SetOrientation(1,1,0);
                mVFMapper->Update();
                // put the vector field between the image and the camera
                if (Renderer->GetActiveCamera()->GetPosition()[2] > this->Slice)
                    mVFActor->SetPosition(0,0,ImageActor->GetBounds()[5]+2);
                else
                    mVFActor->SetPosition(0,0,ImageActor->GetBounds()[4]-2);
            }
            if (mOverlay && mOverlayActor->GetVisibility())
            {
                int overExtent[6];
                ComputeOverlayDisplayedExtent(w_ext[0], w_ext[1], w_ext[2], w_ext[3], this->Slice, this->Slice,overExtent);
                mOverlayActor->SetDisplayExtent(overExtent);
                if (Renderer->GetActiveCamera()->GetPosition()[2] > this->Slice)
                    mOverlayActor->SetPosition(0,0,1);
                else
                    mOverlayActor->SetPosition(0,0,-1);
            }
            if (mFusion && mFusionActor->GetVisibility())
            {
                int fusExtent[6];
                ComputeFusionDisplayedExtent(w_ext[0], w_ext[1], w_ext[2], w_ext[3], this->Slice, this->Slice,fusExtent);
                mFusionActor->SetDisplayExtent(fusExtent);
                if (Renderer->GetActiveCamera()->GetPosition()[2] > this->Slice)
                    mFusionActor->SetPosition(0,0,1.5);
                else
                    mFusionActor->SetPosition(0,0,-1.5);
            }
            if (mLandActor)
            {
                if (mClipBox)
                {
                    double bounds [6];
                    bounds[0] = ImageActor->GetBounds()[0];
                    bounds[1] = ImageActor->GetBounds()[1];
                    bounds[2] = ImageActor->GetBounds()[2];
                    bounds[3] = ImageActor->GetBounds()[3];
                    bounds[4] = ImageActor->GetBounds()[4]-(0.9/this->GetInput()->GetSpacing()[2]);
                    bounds[5] = ImageActor->GetBounds()[5]+(0.9/this->GetInput()->GetSpacing()[2]);
                    mClipBox->SetBounds(bounds);
                    UpdateLandmarks();
                }
                if (Renderer->GetActiveCamera()->GetPosition()[2] > this->Slice)
                    mLandActor->SetPosition(0,0,1.5);
                else
                    mLandActor->SetPosition(0,0,-1.5);
            }
            break;

        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            this->ImageActor->SetDisplayExtent(
                    w_ext[0], w_ext[1], this->Slice, this->Slice, w_ext[4], w_ext[5]);
            if (mVF && mVFActor->GetVisibility())
            {
                int vfExtent[6];
                ComputeVFDisplayedExtent(w_ext[0], w_ext[1], this->Slice, this->Slice, w_ext[4], w_ext[5],vfExtent);
                mVOIFilter->SetVOI(vfExtent);
                mGlyphFilter->SetOrientation(1,0,1);
                mVFMapper->Update();
                // put the vector field between the image aSpacingnd the camera
                if (Renderer->GetActiveCamera()->GetPosition()[1] > this->Slice)
                    mVFActor->SetPosition(0,ImageActor->GetBounds()[3]+2,0);
                else
                    mVFActor->SetPosition(0,ImageActor->GetBounds()[2]-2,0);
            }
            if (mOverlay && mOverlayActor->GetVisibility())
            {
                int overExtent[6];
                ComputeOverlayDisplayedExtent(w_ext[0], w_ext[1], this->Slice, this->Slice, w_ext[4], w_ext[5],overExtent);
                mOverlayActor->SetDisplayExtent(overExtent);
                if (Renderer->GetActiveCamera()->GetPosition()[1] > this->Slice)
                    mOverlayActor->SetPosition(0,1,0);
                else
                    mOverlayActor->SetPosition(0,-1,0);
            }
            if (mFusion && mFusionActor->GetVisibility())
            {
                int fusExtent[6];
                ComputeFusionDisplayedExtent(w_ext[0], w_ext[1], this->Slice, this->Slice, w_ext[4], w_ext[5],fusExtent);
                mFusionActor->SetDisplayExtent(fusExtent);
                if (Renderer->GetActiveCamera()->GetPosition()[1] > this->Slice)
                    mFusionActor->SetPosition(0,1.5,0);
                else
                    mFusionActor->SetPosition(0,-1.5,0);
            }
            if (mLandActor)
            {
                if (mClipBox)
                {
                    double bounds [6];
                    bounds[0] = ImageActor->GetBounds()[0];
                    bounds[1] = ImageActor->GetBounds()[1];
                    bounds[2] = ImageActor->GetBounds()[2]-(0.5/this->GetInput()->GetSpacing()[1]);
                    bounds[3] = ImageActor->GetBounds()[3]+(0.5/this->GetInput()->GetSpacing()[1]);
                    bounds[4] = ImageActor->GetBounds()[4];
                    bounds[5] = ImageActor->GetBounds()[5];
                    mClipBox->SetBounds(bounds);
                    UpdateLandmarks();
                }
                if (Renderer->GetActiveCamera()->GetPosition()[1] > this->Slice)
                    mLandActor->SetPosition(0,1.5,0);
                else
                    mLandActor->SetPosition(0,-1.5,0);
            }
            break;

        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            this->ImageActor->SetDisplayExtent(
                    this->Slice, this->Slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
            if (mVF && mVFActor->GetVisibility())
            {
                int vfExtent[6];
                ComputeVFDisplayedExtent(this->Slice, this->Slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5],vfExtent);
                mVOIFilter->SetVOI(vfExtent);
                mGlyphFilter->SetOrientation(0,1,1);
                mVFMapper->Update();
                // put the vector field between the image and the camera
                if (Renderer->GetActiveCamera()->GetPosition()[0] > this->Slice)
                    mVFActor->SetPosition(ImageActor->GetBounds()[1]+2,0,0);
                else
                    mVFActor->SetPosition(ImageActor->GetBounds()[0]-2,0,0);
            }
            if (mOverlay && mOverlayActor->GetVisibility())
            {
                int overExtent[6];
                ComputeOverlayDisplayedExtent(this->Slice, this->Slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5],overExtent);
                mOverlayActor->SetDisplayExtent(overExtent);
                if (Renderer->GetActiveCamera()->GetPosition()[0] > this->Slice)
                    mOverlayActor->SetPosition(1,0,0);
                else
                    mOverlayActor->SetPosition(-1,0,0);
            }
            if (mFusion && mFusionActor->GetVisibility())
            {
                int fusExtent[6];
                ComputeFusionDisplayedExtent(this->Slice, this->Slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5],fusExtent);
                mFusionActor->SetDisplayExtent(fusExtent);
                if (Renderer->GetActiveCamera()->GetPosition()[0] > this->Slice)
                    mFusionActor->SetPosition(1.5,0,0);
                else
                    mFusionActor->SetPosition(-1.5,0,0);
            }
            if (mLandActor)
            {
                if (mClipBox)
                {
                    double bounds [6];
                    bounds[0] = ImageActor->GetBounds()[0]-(0.5/this->GetInput()->GetSpacing()[0]);
                    bounds[1] = ImageActor->GetBounds()[1]+(0.5/this->GetInput()->GetSpacing()[0]);
                    bounds[2] = ImageActor->GetBounds()[2];
                    bounds[3] = ImageActor->GetBounds()[3];
                    bounds[4] = ImageActor->GetBounds()[4];
                    bounds[5] = ImageActor->GetBounds()[5];
                    mClipBox->SetBounds(bounds);
                    UpdateLandmarks();
                }
                if (Renderer->GetActiveCamera()->GetPosition()[0] > this->Slice)
                    mLandActor->SetPosition(1.5,0,0);
                else
                    mLandActor->SetPosition(-1.5,0,0);
            }
            break;
    }

    // Figure out the correct clipping range

    if (this->Renderer)
    {
        if (this->InteractorStyle &&
                this->InteractorStyle->GetAutoAdjustCameraClippingRange())
        {
            this->Renderer->ResetCameraClippingRange();
        }
        else
        {
            vtkCamera *cam = this->Renderer->GetActiveCamera();
            if (cam)
            {
                double bounds[6];
                this->ImageActor->GetBounds(bounds);
                double spos = (double)bounds[this->SliceOrientation * 2];
                double cpos = (double)cam->GetPosition()[this->SliceOrientation];
                double range = fabs(spos - cpos);
                double *spacing = input->GetSpacing();
                double avg_spacing =
                    ((double)spacing[0] + (double)spacing[1] + (double)spacing[2]) / 3.0;
                cam->SetClippingRange(
                        range - avg_spacing * 3.0, range + avg_spacing * 3.0);
            }
        }
    }
}

void vvSlicer::ComputeVFDisplayedExtent(int x1,int x2,int y1,int y2,int z1,int z2,int vfExtent[6])
{
    vtkImageData* image=this->GetInput();
    vfExtent[0] = (( image->GetOrigin()[0] + x1*image->GetSpacing()[0] ) - mVF->GetOrigin()[0]) /
                mVF->GetSpacing()[0];
    vfExtent[1] = (( image->GetOrigin()[0] + x2*image->GetSpacing()[0] ) - mVF->GetOrigin()[0]) /
                mVF->GetSpacing()[0];
    vfExtent[2] = (( image->GetOrigin()[1] + y1*image->GetSpacing()[1] ) - mVF->GetOrigin()[1]) /
                mVF->GetSpacing()[1];
    vfExtent[3] = (( image->GetOrigin()[1] + y2*image->GetSpacing()[1] ) - mVF->GetOrigin()[1]) /
                mVF->GetSpacing()[1];
    vfExtent[4] = (( image->GetOrigin()[2] + z1*image->GetSpacing()[2] ) - mVF->GetOrigin()[2]) /
                mVF->GetSpacing()[2];
    vfExtent[5] = (( image->GetOrigin()[2] + z2*image->GetSpacing()[2] ) - mVF->GetOrigin()[2]) /
                mVF->GetSpacing()[2];

    ClipDisplayedExtent(vfExtent,mVOIFilter->GetInput()->GetWholeExtent());
}

void vvSlicer::ComputeOverlayDisplayedExtent(int x1,int x2,int y1,int y2,int z1,int z2,int overExtent[6])
{
    vtkImageData* image=this->GetInput();
    overExtent[0] = (( image->GetOrigin()[0] + x1*image->GetSpacing()[0] ) - mOverlay->GetOrigin()[0]) /
                mOverlay->GetSpacing()[0];
    overExtent[1] = (( image->GetOrigin()[0] + x2*image->GetSpacing()[0] ) - mOverlay->GetOrigin()[0]) /
                mOverlay->GetSpacing()[0];
    overExtent[2] = (( image->GetOrigin()[1] + y1*image->GetSpacing()[1] ) - mOverlay->GetOrigin()[1]) /
                mOverlay->GetSpacing()[1];
    overExtent[3] = (( image->GetOrigin()[1] + y2*image->GetSpacing()[1] ) - mOverlay->GetOrigin()[1]) /
                mOverlay->GetSpacing()[1];
    overExtent[4] = (( image->GetOrigin()[2] + z1*image->GetSpacing()[2] ) - mOverlay->GetOrigin()[2]) /
                mOverlay->GetSpacing()[2];
    overExtent[5] = (( image->GetOrigin()[2] + z2*image->GetSpacing()[2] ) - mOverlay->GetOrigin()[2]) /
                mOverlay->GetSpacing()[2];
    ClipDisplayedExtent(overExtent, mOverlayMapper->GetInput()->GetWholeExtent());
}

void vvSlicer::ComputeFusionDisplayedExtent(int x1,int x2,int y1,int y2,int z1,int z2,int fusExtent[6])
{
    vtkImageData* image=this->GetInput();
    fusExtent[0] = (( image->GetOrigin()[0] + x1*image->GetSpacing()[0] ) - mFusion->GetOrigin()[0]) /
                mFusion->GetSpacing()[0];
    fusExtent[1] = (( image->GetOrigin()[0] + x2*image->GetSpacing()[0] ) - mFusion->GetOrigin()[0]) /
                mFusion->GetSpacing()[0];
    fusExtent[2] = (( image->GetOrigin()[1] + y1*image->GetSpacing()[1] ) - mFusion->GetOrigin()[1]) /
                mFusion->GetSpacing()[1];
    fusExtent[3] = (( image->GetOrigin()[1] + y2*image->GetSpacing()[1] ) - mFusion->GetOrigin()[1]) /
                mFusion->GetSpacing()[1];
    fusExtent[4] = (( image->GetOrigin()[2] + z1*image->GetSpacing()[2] ) - mFusion->GetOrigin()[2]) /
                mFusion->GetSpacing()[2];
    fusExtent[5] = (( image->GetOrigin()[2] + z2*image->GetSpacing()[2] ) - mFusion->GetOrigin()[2]) /
                mFusion->GetSpacing()[2];
    ClipDisplayedExtent(fusExtent, mFusionMapper->GetInput()->GetWholeExtent());
}

void vvSlicer::ClipDisplayedExtent(int extent[6], int refExtent[6])
{
    bool out = false;
    int maxBound = 6;

    //2D overlay on 3D image specific case
    if (refExtent[4] == refExtent[5])
    {
        maxBound = 4;
        extent[4] = refExtent[4];
        extent[5] = refExtent[5];
    }

    for (int i = 0; i < maxBound; i = i+2)
    {
        //if we are totally outside the image
        if ( extent[i] > refExtent[i+1] or extent[i+1] < refExtent[i] )
        {
            out = true;
            break;
        }
        //crop to the limit of the image
        extent[i] = (extent[i] > refExtent[i]) ? extent[i] : refExtent[i];
        extent[i] = (extent[i] < refExtent[i+1]) ? extent[i] : refExtent[i+1];
        extent[i+1] = (extent[i+1] > refExtent[i]) ? extent[i+1] : refExtent[i];
        extent[i+1] = (extent[i+1] < refExtent[i+1]) ? extent[i+1] : refExtent[i+1];
    }
    if (out)
        for (int i = 0; i < maxBound; i = i+2)
        {
            extent[i] = refExtent[i];
            extent[i+1] = refExtent[i];
        }
}

void vvSlicer::UpdateOrientation()
{
    // Set the camera position
    vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
    if (cam)
    {
        switch (this->SliceOrientation)
        {
            case vtkImageViewer2::SLICE_ORIENTATION_XY:
                cam->SetFocalPoint(0,0,0);
                cam->SetPosition(0,0,-1); // -1 if medical ?
                cam->SetViewUp(0,-1,0);
                break;

            case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                cam->SetFocalPoint(0,0,0);
                cam->SetPosition(0,-1,0); // 1 if medical ?
                cam->SetViewUp(0,0,1);
                break;

            case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                cam->SetFocalPoint(0,0,0);
                cam->SetPosition(-1,0,0); // -1 if medical ?
                cam->SetViewUp(0,0,1);
                break;
        }
    }
}

void vvSlicer::SetOpacity(double s)
{
    this->GetImageActor()->SetOpacity(s);
}

void vvSlicer::SetRenderWindow(int orientation, vtkRenderWindow * rw)
{
    this->Superclass::SetRenderWindow(rw);
    this->SetupInteractor(rw->GetInteractor());
    ca->SetImageActor(this->GetImageActor());
    ca->SetWindowLevel(this->GetWindowLevel());
    ca->SetText(2, "<slice>");
    ca->SetText(3, "<window>\n<level>");

    double bounds[6];
    double max = 65000;

    bounds[0] = -max;
    bounds[1] = max;
    bounds[2] = -max;
    bounds[3] = max;
    bounds[4] = -max;
    bounds[5] = max;

    crossCursor->SetModelBounds(bounds);
    this->GetRenderer()->AddActor(pdmA);
    this->GetRenderer()->AddActor(ca);
    this->GetRenderer()->ResetCamera();

    //this is just a mapping between the labeling of the orientations presented to the user and
    //the one used by vtk
    SetSliceOrientation(2-(orientation%3));
    ResetCamera();
}

void vvSlicer::ResetCamera()
{
    if (this->GetInput())
    {
        double* input_bounds=this->GetInput()->GetBounds();
        double bmax=input_bounds[1]-input_bounds[0];
        if (bmax < input_bounds[3]-input_bounds[2]) bmax=input_bounds[3]-input_bounds[2];
        if (bmax < input_bounds[5]-input_bounds[4]) bmax=input_bounds[5]-input_bounds[4];
        this->GetRenderer()->ResetCamera();
        this->GetRenderer()->GetActiveCamera()->SetParallelScale(bmax/2);
    }
}

void vvSlicer::SetDisplayMode(bool i)
{
    this->GetImageActor()->SetVisibility(i);
    this->GetAnnotation()->SetVisibility(i);
    this->GetRenderer()->SetDraw(i);
    if (mLandActor)
        mLandActor->SetVisibility(i);
    pdmA->SetVisibility(i);
    if (i)
        UpdateDisplayExtent();
}

void vvSlicer::FlipHorizontalView()
{
    vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
    if (cam)
    {
        double *position = cam->GetPosition();
        switch (this->SliceOrientation)
        {
            case vtkImageViewer2::SLICE_ORIENTATION_XY:
                cam->SetPosition(position[0],position[1],-position[2]);
                break;

            case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                cam->SetPosition(position[0],-position[1],position[2]);
                break;

            case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                cam->SetPosition(-position[0],position[1],position[2]);
                break;
        }
        this->Renderer->ResetCameraClippingRange();
        this->UpdateDisplayExtent();
    }
}

void vvSlicer::FlipVerticalView()
{
    vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
    if (cam)
    {
        FlipHorizontalView();
        double *viewup = cam->GetViewUp();
        cam->SetViewUp(-viewup[0],-viewup[1],-viewup[2]);
        this->UpdateDisplayExtent();
    }
}

void vvSlicer::Render()
{
    if (this->GetWindowLevel()->GetLookupTable() and not this->mOverlay and not
            this->mFusion)
    {
        legend->SetLookupTable(this->GetWindowLevel()->GetLookupTable());
        legend->SetVisibility(1);
    }
    else legend->SetVisibility(0);

    if (ca->GetVisibility())
    {
        std::string worldPos = "";
        std::stringstream world1;
        std::stringstream world2;
        std::stringstream world3;
        world1 << (int)mCurrent[0];
        world2 << (int)mCurrent[1];
        world3 << (int)mCurrent[2];
        double X = (mCurrent[0] - this->GetInput()->GetOrigin()[0])/this->GetInput()->GetSpacing()[0];
        double Y = (mCurrent[1] - this->GetInput()->GetOrigin()[1])/this->GetInput()->GetSpacing()[1];
        double Z = (mCurrent[2] - this->GetInput()->GetOrigin()[2])/this->GetInput()->GetSpacing()[2];

        if (pdmA->GetVisibility())
        {
            double x = mCursor[0];
            double y = mCursor[1];
            double z = mCursor[2];
            double xCursor = (x - this->GetInput()->GetOrigin()[0])/this->GetInput()->GetSpacing()[0];
            double yCursor = (y - this->GetInput()->GetOrigin()[1])/this->GetInput()->GetSpacing()[1];
            double zCursor = (z - this->GetInput()->GetOrigin()[2])/this->GetInput()->GetSpacing()[2];

            if (xCursor >= this->GetImageActor()->GetDisplayExtent()[0] &&
                    xCursor < this->GetImageActor()->GetDisplayExtent()[1]+1 &&
                    yCursor >= this->GetImageActor()->GetDisplayExtent()[2] &&
                    yCursor < this->GetImageActor()->GetDisplayExtent()[3]+1 &&
                    zCursor >= this->GetImageActor()->GetDisplayExtent()[4] &&
                    zCursor < this->GetImageActor()->GetDisplayExtent()[5]+1 )
            {
                vtkRenderer * renderer = this->Renderer;

                renderer->WorldToView(x,y,z);
                renderer->ViewToNormalizedViewport(x,y,z);
                renderer->NormalizedViewportToViewport(x,y);
                renderer->ViewportToNormalizedDisplay(x,y);
                renderer->NormalizedDisplayToDisplay(x,y);
                crossCursor->SetFocalPoint(x,y,z);
            }
            else
                crossCursor->SetFocalPoint(-1,-1,z);
        }

        if (X >= this->GetInput()->GetWholeExtent()[0] &&
                X <= this->GetInput()->GetWholeExtent()[1] &&
                Y >= this->GetInput()->GetWholeExtent()[2] &&
                Y <= this->GetInput()->GetWholeExtent()[3] &&
                Z >= this->GetInput()->GetWholeExtent()[4] &&
                Z <= this->GetInput()->GetWholeExtent()[5])
        {
            std::stringstream pixel1;
            std::stringstream pixel2;
            std::stringstream pixel3;
            std::stringstream temps;
            pixel1 << (int)X;
            pixel2 << (int)Y;
            pixel3 << (int)Z;
            temps << mCurrentTSlice;
            double value = this->GetInput()->GetScalarComponentAsDouble(
                    (int)X,
                    (int)Y,
                    (int)Z,0);

            std::stringstream val;
            val << value;
            worldPos += "data value : " + val.str();
            worldPos += "\n mm : " + world1.str() + " " + world2.str() + " " + world3.str() + " " + temps.str();
            worldPos += "\n pixel : " + pixel1.str() + " " + pixel2.str() + " " + pixel3.str() + " " + temps.str();
        }
        ca->SetText(1,worldPos.c_str());
    }
    if (mOverlay && mOverlayActor->GetVisibility())
    {
        mOverlayMapper->SetWindow(this->GetColorWindow());
        mOverlayMapper->SetLevel(this->GetColorLevel());
        mOverlayMapper->Update();
    }
    if (mLandMapper)
        UpdateLandmarks();
    //this->Superclass::Render();
    this->GetRenderWindow()->Render();
}

void vvSlicer::UpdateCursorPosition()
{
    if (this->GetImageActor()->GetVisibility())
    {
        pdmA->SetVisibility(true);
        mCursor[0] = mCurrent[0];
        mCursor[1] = mCurrent[1];
        mCursor[2] = mCurrent[2];
        mCursor[3] = mCurrentTSlice;
    }
}

void vvSlicer::UpdateLandmarks()
{
    vtkPolyData *pd = static_cast<vtkPolyData*>(mLandClipper->GetInput());
    if (pd->GetPoints())
    {
        mLandGlyph->SetRange(0,1);
        mLandGlyph->Modified();
        mLandGlyph->Update();

        mClipBox->Modified();
        mLandClipper->Update();
        mLandMapper->Update();
    }

}

//----------------------------------------------------------------------------
void vvSlicer::SetSlice(int slice)
{
    int *range = this->GetSliceRange();
    if (range)
    {
        if (slice < range[0])
        {
            slice = range[0];
        }
        else if (slice > range[1])
        {
            slice = range[1];
        }
    }

    if (this->Slice == slice)
    {
        return;
    }

    this->Slice = slice;
    SetContourSlice();
    this->Modified();
    this->UpdateDisplayExtent();
    this->Render();
}

void vvSlicer::SetContourSlice()
{
    if (mSurfaceCutActors.size() > 0)
        for (std::vector<vvMeshActor*>::iterator i=mSurfaceCutActors.begin();
                i!=mSurfaceCutActors.end();i++)
            (*i)->SetCutSlice((this->Slice)*this->GetImage()->GetSpacing()[this->SliceOrientation]+
                    this->GetImage()->GetOrigin()[this->SliceOrientation]);
}

void vvSlicer::ForceUpdateDisplayExtent()
{
    this->UpdateDisplayExtent();
}

int* vvSlicer::GetDisplayExtent()
{
    return this->GetImageActor()->GetDisplayExtent();
}

void vvSlicer::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}
