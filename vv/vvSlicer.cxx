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

#include "vvSlicer.h"
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
#include <vvBlendImageActor.h>
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
#include <vtkAssignAttribute.h>
#include <vtkImageAccumulate.h>
#include <vtkImageReslice.h>
#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
#  include <vtkImageMapper3D.h>
#endif

vtkCxxRevisionMacro(vvSlicer, "DummyRevision");
vtkStandardNewMacro(vvSlicer);
static void copyExtent(int* in, int* to){
 for(int i=0; i<6; ++i) to[i]=in[i]; 
}
//------------------------------------------------------------------------------
vvSlicer::vvSlicer()
{
  this->UnInstallPipeline();
  mImage = NULL;
  mReducedExtent = new int[6];
  mCurrentTSlice = 0;
  mCurrentFusionTSlice = 0;
  mCurrentOverlayTSlice = 0;
  mUseReducedExtent = false;

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
  mVFWidth = 1;
  mVFColor[0] = 0;
  mVFColor[1] = 1;
  mVFColor[2] = 0;

  crossCursor = vtkSmartPointer<vtkCursor2D>::New();
  crossCursor->AllOff();
  crossCursor->AxesOn();
  crossCursor->SetTranslationMode(1);
  crossCursor->SetRadius(2);

  pdm = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  pdm->SetInput(crossCursor->GetOutput());

  pdmA = vtkSmartPointer<vtkActor2D>::New();
  pdmA->SetMapper(pdm);
  pdmA->GetProperty()->SetColor(255,10,212);
  pdmA->SetVisibility(0);
  pdmA->SetPickable(0);

  ca = vtkSmartPointer<vtkCornerAnnotation>::New();
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
  showFusionLegend = false;

  this->WindowLevel->Delete();
  this->WindowLevel = vvImageMapToWLColors::New();

  this->InstallPipeline();

  mLinkOverlayWindowLevel = true;

#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
  this->GetImageActor()->GetMapper()->BorderOn();
#endif

  mSlicingTransform = vtkSmartPointer<vtkTransform>::New();
  mConcatenatedTransform = vtkSmartPointer<vtkTransform>::New();
  mConcatenatedFusionTransform = vtkSmartPointer<vtkTransform>::New();
  mConcatenatedOverlayTransform = vtkSmartPointer<vtkTransform>::New();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vtkImageMapToWindowLevelColors* vvSlicer::GetOverlayMapper()
{
  return mOverlayMapper.GetPointer();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvBlendImageActor* vvSlicer::GetOverlayActor()
{
  return mOverlayActor.GetPointer();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vtkImageMapToColors* vvSlicer::GetFusionMapper()
{
  return mFusionMapper.GetPointer();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vtkImageActor* vvSlicer::GetFusionActor()
{
  return mFusionActor.GetPointer();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vtkActor* vvSlicer::GetVFActor()
{
  return mVFActor.GetPointer();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vtkCornerAnnotation* vvSlicer::GetAnnotation()
{
  return ca.GetPointer();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::EnableReducedExtent(bool b)
{
  mUseReducedExtent = b;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetReducedExtent(int * ext)
{
  copyExtent(ext, mReducedExtent);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::ToggleContourSuperposition()
{
  for (std::vector<vvMeshActor*>::iterator i=mSurfaceCutActors.begin();
       i!=mSurfaceCutActors.end(); i++)
    (*i)->ToggleSuperposition();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetCursorColor(int r,int g, int b)
{
  pdmA->GetProperty()->SetColor(r,g,b);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetCursorVisibility(bool s)
{
  pdmA->SetVisibility(s);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvSlicer::GetCursorVisibility()
{
  return pdmA->GetVisibility();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetCornerAnnotationVisibility(bool s)
{
  ca->SetVisibility(s);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvSlicer::GetCornerAnnotationVisibility()
{
  return ca->GetVisibility();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvSlicer::~vvSlicer()
{
  for (std::vector<vvMeshActor*>::iterator i=mSurfaceCutActors.begin();
       i!=mSurfaceCutActors.end(); i++)
    delete (*i);
  delete [] mReducedExtent;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
double* vvSlicer::GetCurrentPosition()
{
  return mCurrentBeforeSlicingTransform;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSlicer::SetCurrentPosition(double x, double y, double z, int t)
{
  mCurrentBeforeSlicingTransform[0]=x;
  mCurrentBeforeSlicingTransform[1]=y;
  mCurrentBeforeSlicingTransform[2]=z;
  mSlicingTransform->GetInverse()->TransformPoint(mCurrentBeforeSlicingTransform,mCurrent);
  SetTSlice(t);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetImage(vvImage::Pointer image)
{
  if (image->GetVTKImages().size()) {
    mImage = image;

    if (!mImageReslice) {
      mImageReslice = vtkSmartPointer<vtkImageReslice>::New();
      mImageReslice->SetInterpolationModeToLinear();
      mImageReslice->AutoCropOutputOn();
      mImageReslice->SetBackgroundColor(-1000,-1000,-1000,1);
    }

    mConcatenatedTransform->Identity();
    mConcatenatedTransform->Concatenate(mImage->GetTransform()[0]);
    mConcatenatedTransform->Concatenate(mSlicingTransform);
    mImageReslice->SetResliceTransform(mConcatenatedTransform);
    mImageReslice->SetInput(0, mImage->GetFirstVTKImageData());
    mImageReslice->UpdateInformation();

    this->Superclass::SetInput(mImageReslice->GetOutput());

    int extent[6];
    this->GetInput()->GetWholeExtent(extent);

    // Prevent crash when reload -> change slice if outside extent
    if (Slice < extent[SliceOrientation*2] || Slice>=extent[SliceOrientation*2+1]) {
      Slice = (extent[SliceOrientation*2+1]+extent[SliceOrientation*2])/2.0;
    }

    // Make sure that the required part image has been computed
    extent[SliceOrientation*2] = Slice;
    extent[SliceOrientation*2+1] = Slice;
    mImageReslice->GetOutput()->SetUpdateExtent(extent);
    mImageReslice->GetOutput()->Update();

    this->UpdateDisplayExtent();

    mCurrentTSlice = 0;
    ca->SetText(0,mFileName.c_str());
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetOverlay(vvImage::Pointer overlay)
{
  if (overlay->GetVTKImages().size()) {
    mOverlay = overlay;
    
    if (!mOverlayReslice) {
      mOverlayReslice = vtkSmartPointer<vtkImageReslice>::New();
      mOverlayReslice->SetInterpolationModeToLinear();
      mOverlayReslice->AutoCropOutputOn();
      mOverlayReslice->SetBackgroundColor(-1000,-1000,-1000,1);
    }

    mConcatenatedOverlayTransform->Identity();
    mConcatenatedOverlayTransform->Concatenate(mOverlay->GetTransform()[0]);
    mConcatenatedOverlayTransform->Concatenate(mSlicingTransform);
    mOverlayReslice->SetResliceTransform(mConcatenatedOverlayTransform);
    mOverlayReslice->SetInput(0, mOverlay->GetFirstVTKImageData());
    mImageReslice->UpdateInformation();

    if (!mOverlayMapper)
      mOverlayMapper = vtkSmartPointer<vtkImageMapToWindowLevelColors>::New();
    mOverlayMapper->SetInput(mOverlayReslice->GetOutput());

    if (!mOverlayActor) {
      mOverlayActor = vtkSmartPointer<vvBlendImageActor>::New();
      mOverlayActor->SetInput(mOverlayMapper->GetOutput());
      mOverlayActor->SetPickable(0);
      mOverlayActor->SetVisibility(true);
      mOverlayActor->SetOpacity(0.5);
#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
      mOverlayActor->GetMapper()->BorderOn();
#endif
      }

    //stupid but necessary : the Overlay need to be rendered before fusion
    if (mFusionActor) {
      this->GetRenderer()->RemoveActor(mFusionActor);
      this->GetRenderer()->AddActor(mOverlayActor);
      this->GetRenderer()->AddActor(mFusionActor);
    } else
      this->GetRenderer()->AddActor(mOverlayActor);

    //Synchronize orientation and slice
    AdjustResliceToSliceOrientation(mOverlayReslice);
    this->UpdateDisplayExtent();
    this->SetTSlice(mCurrentTSlice);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetFusion(vvImage::Pointer fusion)
{
  if (fusion->GetVTKImages().size()) {
    mFusion = fusion;

    if (!mFusionReslice) {
      mFusionReslice = vtkSmartPointer<vtkImageReslice>::New();
      mFusionReslice->SetInterpolationModeToLinear();
      mFusionReslice->AutoCropOutputOn();
      mFusionReslice->SetBackgroundColor(-1000,-1000,-1000,1);
    }

    mConcatenatedFusionTransform->Identity();
    mConcatenatedFusionTransform->Concatenate(mFusion->GetTransform()[0]);
    mConcatenatedFusionTransform->Concatenate(mSlicingTransform);
    mFusionReslice->SetResliceTransform(mConcatenatedFusionTransform);
    mFusionReslice->SetInput(0, mFusion->GetFirstVTKImageData());
    mFusionReslice->UpdateInformation();

    if (!mFusionMapper)
      mFusionMapper = vtkSmartPointer<vtkImageMapToColors>::New();

    vtkSmartPointer<vtkLookupTable> lut = vtkLookupTable::New();
    lut->SetRange(0, 1);
    lut->SetValueRange(0, 1);
    lut->SetSaturationRange(0, 0);
    lut->Build();
    mFusionMapper->SetLookupTable(lut);
    mFusionMapper->SetInput(mFusionReslice->GetOutput());

    if (!mFusionActor) {
      mFusionActor = vtkSmartPointer<vtkImageActor>::New();
      mFusionActor->SetInput(mFusionMapper->GetOutput());
      mFusionActor->SetPickable(0);
      mFusionActor->SetVisibility(true);
      mFusionActor->SetOpacity(0.7);
#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
      mFusionActor->GetMapper()->BorderOn();
#endif
      this->GetRenderer()->AddActor(mFusionActor);
    }

    //Synchronize orientation and slice
    AdjustResliceToSliceOrientation(mFusionReslice);
    this->UpdateDisplayExtent();
    this->SetTSlice(mCurrentTSlice);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvSlicer::GetActorVisibility(const std::string& actor_type, int overlay_index)
{
  bool vis = false;
  if (actor_type == "image") {
    vis = this->ImageActor->GetVisibility();
  }
  else if (actor_type == "vector") {
    vis = this->mVFActor->GetVisibility();
  }
  else if (actor_type == "overlay") {
    vis = this->mOverlayActor->GetVisibility();
  }
  else if (actor_type == "fusion") {
    vis = this->mFusionActor->GetVisibility();
  }
  else if (actor_type == "contour")
    vis = this->mSurfaceCutActors[overlay_index]->GetActor()->GetVisibility();

  return vis;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSlicer::SetActorVisibility(const std::string& actor_type, int overlay_index ,bool vis)
{
  if (actor_type == "image") {
    this->ImageActor->SetVisibility(vis);
  }
  else if (actor_type == "vector") {
    this->mVFActor->SetVisibility(vis);
  }
  else if (actor_type == "overlay") {
    this->mOverlayActor->SetVisibility(vis);
  }
  else if (actor_type == "fusion") {
    this->mFusionActor->SetVisibility(vis);
  }
  else if (actor_type == "contour")
    this->mSurfaceCutActors[overlay_index]->GetActor()->SetVisibility(vis);
  UpdateDisplayExtent();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSlicer::SetVF(vvImage::Pointer vf)
{
  if (vf->GetVTKImages().size()) {
    mVF = vf;

    if (!mAAFilter) {
      mAAFilter= vtkSmartPointer<vtkAssignAttribute>::New();
      mVOIFilter = vtkSmartPointer<vtkExtractVOI>::New();
      mVOIFilter->SetSampleRate(mSubSampling,mSubSampling,mSubSampling);
    }
    mVOIFilter->SetInput(vf->GetFirstVTKImageData());
    mAAFilter->SetInput(mVOIFilter->GetOutput());
    ///This tells VTK to use the scalar (pixel) data of the image to draw the little arrows
    mAAFilter->Assign(vtkDataSetAttributes::SCALARS, vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);

    if (!mArrow)
      mArrow = vtkSmartPointer<vvGlyphSource>::New();
    mArrow->SetGlyphTypeToSpecificArrow();
    mArrow->SetScale(mScale);
    mArrow->FilledOff();

    // Glyph the gradient vector (with arrows)
    if (!mGlyphFilter)
      mGlyphFilter = vtkSmartPointer<vvGlyph2D>::New();
    mGlyphFilter->SetInput(mAAFilter->GetOutput());
    mGlyphFilter->SetSource(mArrow->GetOutput());
    mGlyphFilter->ScalingOn();
    mGlyphFilter->SetScaleModeToScaleByVector();
    mGlyphFilter->OrientOn();
    mGlyphFilter->SetVectorModeToUseVector();
    mGlyphFilter->SetColorModeToColorByVector();

    if (!mVFColorLUT)
      mVFColorLUT = vtkSmartPointer<vtkLookupTable>::New();

    double mVFColorHSV[3];
    vtkMath::RGBToHSV(mVFColor, mVFColorHSV);
    mVFColorLUT->SetHueRange(mVFColorHSV[0], mVFColorHSV[0]);
    mVFColorLUT->SetSaturationRange(mVFColorHSV[1],mVFColorHSV[1]);
    mVFColorLUT->SetValueRange(mVFColorHSV[2], mVFColorHSV[2]);

    if (!mVFMapper)
      mVFMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mVFMapper->SetInput(mGlyphFilter->GetOutput());
    mVFMapper->ImmediateModeRenderingOn();
    mVFMapper->SetLookupTable(mVFColorLUT);

    if (!mVFActor)
      mVFActor = vtkSmartPointer<vtkActor>::New();
    mVFActor->SetMapper(mVFMapper);
    mVFActor->SetPickable(0);
    mVFActor->GetProperty()->SetLineWidth(mVFWidth);
    this->UpdateDisplayExtent();
    this->GetRenderer()->AddActor(mVFActor);

    //Synchronize slice
    SetTSlice(mCurrentTSlice);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetLandmarks(vvLandmarks* landmarks)
{
  mLandmarks = landmarks;
  if (landmarks) {

    if (!mCross)
      mCross = vtkSmartPointer<vtkCursor3D>::New();
    mCross->SetFocalPoint(0.0,0.0,0.0);
    mCross->SetModelBounds(-10,10,-10,10,-10,10);
    mCross->AllOff();
    mCross->AxesOn();

    if (!mLandGlyph)
      mLandGlyph = vtkSmartPointer<vtkGlyph3D>::New();
    mLandGlyph->SetSource(mCross->GetOutput());
    mLandGlyph->SetInput(landmarks->GetOutput());
    //mLandGlyph->SetIndexModeToScalar();
    mLandGlyph->SetRange(0,1);
    mLandGlyph->ScalingOff();

    mLandGlyph->SetColorModeToColorByScalar();

    if (!mClipBox)
      mClipBox = vtkSmartPointer<vtkBox>::New();
    if (!mLandClipper)
      mLandClipper = vtkSmartPointer<vtkClipPolyData>::New();
    mLandClipper->InsideOutOn();
    mLandClipper->SetInput(mLandGlyph->GetOutput());
    mLandClipper->SetClipFunction(mClipBox);

    if (!mLandMapper)
      mLandMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mLandMapper->SetInputConnection(mLandClipper->GetOutputPort());
    //mLandMapper->ScalarVisibilityOff();

    if (!mLandActor)
      mLandActor = vtkSmartPointer<vtkActor>::New();
    mLandActor->SetMapper(mLandMapper);
    mLandActor->GetProperty()->SetColor(255,10,212);
    mLandActor->SetPickable(0);
    mLandActor->SetVisibility(true);
    this->UpdateDisplayExtent();
    this->GetRenderer()->AddActor(mLandActor);
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//FIXME: this function leaks memory, we should fix it someday :)
void vvSlicer::RemoveActor(const std::string& actor_type, int overlay_index)
{
  if (actor_type == "vector") {
    Renderer->RemoveActor(mVFActor);
    mGlyphFilter=NULL;
    mVF = NULL;
    mArrow = NULL;
    mAAFilter=NULL;
    mVOIFilter = NULL;
    mVFMapper = NULL;
    mVFActor = NULL;
  }
  if (actor_type == "overlay") {
    Renderer->RemoveActor(mOverlayActor);
    mOverlay = NULL;
    mOverlayActor = NULL;
    mOverlayMapper = NULL;
  }
  if (actor_type == "fusion") {
    Renderer->RemoveActor(mFusionActor);
    mFusion = NULL;
    mFusionActor = NULL;
    mFusionMapper = NULL;
  }
  if (actor_type == "contour") {
    Renderer->RemoveActor(this->mSurfaceCutActors[overlay_index]->GetActor());
    mSurfaceCutActors.erase(mSurfaceCutActors.begin()+overlay_index);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetVFSubSampling(int sub)
{
  if (mVOIFilter) {
    mVOIFilter->SetSampleRate(mSubSampling,mSubSampling,mSubSampling);
    mSubSampling = sub;
  }
  UpdateDisplayExtent();
  Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetVFScale(int scale)
{
  mScale = scale;
  if (mArrow)
    mArrow->SetScale(mScale);
  UpdateDisplayExtent();
  Render();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSlicer::SetVFWidth(int width)
{
  mVFWidth = width;
  if (mVFActor)
    mVFActor->GetProperty()->SetLineWidth(mVFWidth);
  UpdateDisplayExtent();
  Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetVFLog(int log)
{
  mVFLog = log;
  if (mGlyphFilter) {
    mGlyphFilter->SetUseLog(mVFLog);
    mGlyphFilter->Modified();
  }
  UpdateDisplayExtent();
  Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicer::SetTSlice(int t)
{
  if (t < 0)
    t = 0;
  else if ((unsigned int)t >= mImage->GetVTKImages().size())
    t = mImage->GetVTKImages().size() -1;

  // Update transform
  mConcatenatedTransform->Identity();
  mConcatenatedTransform->Concatenate(mImage->GetTransform()[t]);
  mConcatenatedTransform->Concatenate(mSlicingTransform);

  // Update image data
  mCurrentTSlice = t;
  mImageReslice->SetInput( mImage->GetVTKImages()[mCurrentTSlice] );
  if (mVF && mVFActor->GetVisibility()) {
    if (mVF->GetVTKImages().size() > (unsigned int)mCurrentTSlice)
      mVOIFilter->SetInput(mVF->GetVTKImages()[mCurrentTSlice]);
  }
  if (mOverlay && mOverlayActor->GetVisibility()) {
    if (mOverlay->GetVTKImages().size() > (unsigned int)mCurrentTSlice) {
      mCurrentOverlayTSlice = mCurrentTSlice;
      mOverlayReslice->SetInput( mOverlay->GetVTKImages()[mCurrentOverlayTSlice] );
    }
  }
  if (mFusion && mFusionActor->GetVisibility()) {
    if (mFusion->GetVTKImages().size() > (unsigned int)mCurrentTSlice) {
      mCurrentFusionTSlice = mCurrentTSlice;
      mFusionReslice->SetInput( mFusion->GetVTKImages()[mCurrentFusionTSlice]);
    }
  }
  if (mSurfaceCutActors.size() > 0)
    for (std::vector<vvMeshActor*>::iterator i=mSurfaceCutActors.begin();
         i!=mSurfaceCutActors.end(); i++)
      (*i)->SetTimeSlice(mCurrentTSlice);
  UpdateDisplayExtent();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int vvSlicer::GetTSlice()
{
  return mCurrentTSlice;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int vvSlicer::GetFusionTSlice()
{
  return mCurrentFusionTSlice;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int vvSlicer::GetOverlayTSlice()
{
  return mCurrentOverlayTSlice;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSlicer::SetSliceOrientation(int orientation)
{
  //if 2D image, force to watch in Axial View
  int extent[6];
  this->GetInput()->GetWholeExtent(extent);
  if (extent[5]-extent[4] <= 2)
    orientation = vtkImageViewer2::SLICE_ORIENTATION_XY;

  if (orientation < vtkImageViewer2::SLICE_ORIENTATION_YZ ||
      orientation > vtkImageViewer2::SLICE_ORIENTATION_XY) {
    vtkErrorMacro("Error - invalid slice orientation " << orientation);
    return;
  }
  
  this->SliceOrientation = orientation;

  if(mFusion)
    AdjustResliceToSliceOrientation(mFusionReslice);

  if(mOverlay)
    AdjustResliceToSliceOrientation(mOverlayReslice);

  // Update the viewer
  int *range = this->GetSliceRange();
  if (range)
    this->Slice = static_cast<int>((range[0] + range[1]) * 0.5);

  // Go to current cursor position
  // double* cursorPos = GetCursorPosition();
  // DDV(cursorPos, 3);
  // SetCurrentPosition(cursorPos[0],cursorPos[1],cursorPos[2],cursorPos[3]);

  this->UpdateOrientation();
  this->UpdateDisplayExtent();

  if (this->Renderer && this->GetInput()) {
    double scale = this->Renderer->GetActiveCamera()->GetParallelScale();
    this->Renderer->ResetCamera();
    this->Renderer->GetActiveCamera()->SetParallelScale(scale);
  }

  SetContourSlice();
}
//----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// This function ensures that we sample the slices of a vtkImageReslice filter
// in the direction of the slicer (SliceOrientation) similarly as mImageReslice.
// In other words, we change the grid of the reslice in the same way as the grid
// of the displayed image in the slicing direction.
void vvSlicer::AdjustResliceToSliceOrientation(vtkImageReslice *reslice)
{
  // Reset autocrop and update output information
  reslice->SetOutputOriginToDefault();
  reslice->SetOutputSpacingToDefault();
  reslice->GetOutput()->UpdateInformation();

  // Ge new origin / spacing
  double origin[3];
  double spacing[3];
  reslice->GetOutput()->GetOrigin(origin);
  reslice->GetOutput()->GetSpacing(spacing);

  // Use similar spacing as the image in the direction SliceOrientation
  spacing[this->SliceOrientation] = mImageReslice->GetOutput()->GetSpacing()[this->SliceOrientation];

  // Modify origin to be on the image grid in the direction SliceOrientation in 3 steps
  // Step 1: from world coordinates to image coordinates
  origin[this->SliceOrientation] -= mImageReslice->GetOutput()->GetOrigin()[this->SliceOrientation];
  origin[this->SliceOrientation] /= mImageReslice->GetOutput()->GetSpacing()[this->SliceOrientation];
  // Step 2: round to superior grid positionInc
  origin[this->SliceOrientation] = itk::Math::Ceil<double>(origin[this->SliceOrientation]);
  // Step 3: back to world coordinates
  origin[this->SliceOrientation] *= mImageReslice->GetOutput()->GetSpacing()[this->SliceOrientation];
  origin[this->SliceOrientation] += mImageReslice->GetOutput()->GetOrigin()[this->SliceOrientation];

  // Set new spacing and origin
  reslice->SetOutputOrigin(origin);
  reslice->SetOutputSpacing(spacing);
  reslice->UpdateInformation();
  reslice->GetOutput()->UpdateInformation();
}
//------------------------------------------------------------------------------

//----------------------------------------------------------------------------
int * vvSlicer::GetExtent(){
  int *w_ext;
  if (mUseReducedExtent) {
    w_ext = mReducedExtent;
  } else w_ext = GetInput()->GetWholeExtent();
  return w_ext;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
int vvSlicer::GetOrientation()
{
  return this->SliceOrientation;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::UpdateDisplayExtent()
{
  vtkImageData *input = this->GetInput();
  if (!input || !this->ImageActor) {
    return;
  }
  input->UpdateInformation();

  // Local copy of extent
  int w_ext[6];
  int* ext = GetExtent();
  copyExtent(ext, w_ext);
  // Set slice value
  int s = this->Slice > ext[this->SliceOrientation*2+1] ? ext[this->SliceOrientation*2 + 1] : this->Slice;
  w_ext[ this->SliceOrientation*2   ] = s;
  w_ext[ this->SliceOrientation*2+1 ] = s;
  
  // Image actor
  this->ImageActor->SetDisplayExtent(w_ext);
  
  // Overlay image actor
  if (mOverlay && mOverlayActor->GetVisibility()) {
    AdjustResliceToSliceOrientation(mOverlayReslice);
    int overExtent[6];
    this->ConvertImageToImageDisplayExtent(input, w_ext, mOverlayReslice->GetOutput(), overExtent);
    ClipDisplayedExtent(overExtent, mOverlayMapper->GetInput()->GetWholeExtent());
    mOverlayActor->SetDisplayExtent( overExtent );
  }

  // Fusion image actor
  if (mFusion && mFusionActor->GetVisibility()) {
    AdjustResliceToSliceOrientation(mFusionReslice);
    int fusExtent[6];
    this->ConvertImageToImageDisplayExtent(input, w_ext, mFusionReslice->GetOutput(), fusExtent);
    ClipDisplayedExtent(fusExtent, mFusionMapper->GetInput()->GetWholeExtent());
    mFusionActor->SetDisplayExtent(fusExtent);
  }

  // Vector field actor
  double* camera = Renderer->GetActiveCamera()->GetPosition();
  double* image_bounds = ImageActor->GetBounds();
  double position[3] = {0, 0, 0};
  position[this->SliceOrientation] = image_bounds[this->SliceOrientation*2]; 

  //print_vector<double, 6>("camera", camera);
  //print_vector<double, 6>("image_bounds", image_bounds);
  //print_vector<double, 3>("position", position);

  // find where to place the VF actor. to deal with
  // z-buffer issues, the VF is placed right in front of the image,
  // subject to a small offset. the position actually depends on the
  // the location of the camera relative to the image. 
  double offset = 1;
  if (camera[this->SliceOrientation] < image_bounds[this->SliceOrientation*2])
    offset = -1;
  
  if (mVF && mVFActor->GetVisibility()) {
    int vfExtent[6];
    mVF->GetVTKImages()[0]->UpdateInformation();
    this->ConvertImageToImageDisplayExtent(input, w_ext, mVF->GetVTKImages()[0], vfExtent);
    ClipDisplayedExtent(vfExtent, mVOIFilter->GetInput()->GetWholeExtent());
    mVOIFilter->SetVOI(vfExtent);
    int orientation[3] = {1,1,1};
    orientation[this->SliceOrientation] = 0;
    mGlyphFilter->SetOrientation(orientation[0], orientation[1], orientation[2]);
    mVFMapper->Update();

    position[this->SliceOrientation] += offset;
    mVFActor->SetPosition(position);
  }
  
  // Landmarks actor
  if (mLandActor) {
    if (mClipBox) {
      double bounds [6];
      for(unsigned int i=0; i<6; i++)
        bounds[i] = ImageActor->GetBounds()[i];
      bounds[ this->SliceOrientation*2   ] = ImageActor->GetBounds()[ this->SliceOrientation*2  ]-fabs(this->GetInput()->GetSpacing()[this->SliceOrientation]);
      bounds[ this->SliceOrientation*2+1 ] = ImageActor->GetBounds()[ this->SliceOrientation*2+1 ]+fabs(this->GetInput()->GetSpacing()[this->SliceOrientation]);
      mClipBox->SetBounds(bounds);
      UpdateLandmarks();
    }
    
    position[this->SliceOrientation] = offset;
    mLandActor->SetPosition(position);
  }

  // Figure out the correct clipping range
  if (this->Renderer) {
    if (this->InteractorStyle &&
        this->InteractorStyle->GetAutoAdjustCameraClippingRange()) {
      this->Renderer->ResetCameraClippingRange();
    } else {
      vtkCamera *cam = this->Renderer->GetActiveCamera();
      if (cam) {
        double bounds[6];
        this->ImageActor->GetBounds(bounds);
        double spos = (double)bounds[this->SliceOrientation * 2];
        double cpos = (double)cam->GetPosition()[this->SliceOrientation];
        double range = fabs(spos - cpos);
        double *spacing = input->GetSpacing();
        double sumSpacing = spacing[0] + spacing[1] + spacing[2];
        cam->SetClippingRange(range - sumSpacing, range + sumSpacing);
      }
    }
  }
  
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::ConvertImageToImageDisplayExtent(vtkImageData *sourceImage, const int sourceExtent[6],
                                                vtkImageData *targetImage, int targetExtent[6])
{
  double dExtents[6];
  for(unsigned int i=0; i<6; i++) {
    // From source voxel coordinates to world coordinates
    dExtents[i] = sourceImage->GetOrigin()[i/2] + sourceImage->GetSpacing()[i/2] * sourceExtent[i];

    // From world coordinates to floating point target voxel coordinates
    dExtents[i] = (dExtents[i]- targetImage->GetOrigin()[i/2]) / targetImage->GetSpacing()[i/2];
    
    // Round to nearest
    //targetExtent[i] = itk::Math::Round<double>(dExtents[i]);
    targetExtent[i] = itk::Math::Floor<double>(dExtents[i]);
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::ClipDisplayedExtent(int extent[6], int refExtent[6])
{
  bool out = false;
  int maxBound = 6;

  //2D overlay on 3D image specific case
  if (refExtent[4] == refExtent[5]) {
    maxBound = 4;
    extent[4] = refExtent[4];
    extent[5] = refExtent[5];
  }

  for (int i = 0; i < maxBound; i = i+2) {
    //if we are totally outside the image
    if ( extent[i] > refExtent[i+1] || extent[i+1] < refExtent[i] ) {
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
    for (int i = 0; i < maxBound; i = i+2) {
      extent[i] = refExtent[i];
      extent[i+1] = refExtent[i];
    }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::UpdateOrientation()
{
  // Set the camera position
  vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (cam) {
    switch (this->SliceOrientation) {
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
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::SetOpacity(double s)
{
  this->GetImageActor()->SetOpacity(s);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::ResetCamera()
{
  this->GetRenderer()->ResetCamera();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::SetDisplayMode(bool i)
{
  this->GetRenderer()->SetDraw(i);
  if (i)
    UpdateDisplayExtent();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::FlipHorizontalView()
{
  vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (cam) {
    double *position = cam->GetPosition();
    double factor[3] = {1, 1, 1};
    factor[this->SliceOrientation] = -1;
    cam->SetPosition(factor[0]*position[0],factor[1]*position[1],factor[2]*position[2]);
    
/*    switch (this->SliceOrientation) {
    case vtkImageViewer2::SLICE_ORIENTATION_XY:
      cam->SetPosition(position[0],position[1],-position[2]);
      break;

    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
      cam->SetPosition(position[0],-position[1],position[2]);
      break;

    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
      cam->SetPosition(-position[0],position[1],position[2]);
      break;
    }*/

    this->Renderer->ResetCameraClippingRange();
    this->UpdateDisplayExtent();
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::FlipVerticalView()
{
  vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (cam) {
    FlipHorizontalView();
    double *viewup = cam->GetViewUp();
    cam->SetViewUp(-viewup[0],-viewup[1],-viewup[2]);
    this->UpdateDisplayExtent();
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::SetColorWindow(double window)
{
  vtkLookupTable* LUT = static_cast<vtkLookupTable*>(this->GetWindowLevel()->GetLookupTable());
  if ( LUT ) {
    double level = this->GetWindowLevel()->GetLevel();
    LUT->SetTableRange(level-fabs(window)/2,level+fabs(window)/2);
    LUT->Build();
  }
  this->vtkImageViewer2::SetColorWindow(window);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::SetColorLevel(double level)
{
  vtkLookupTable* LUT = static_cast<vtkLookupTable*>(this->GetWindowLevel()->GetLookupTable());
  if ( LUT ) {
    double window = this->GetWindowLevel()->GetWindow();
    LUT->SetTableRange(level-fabs(window)/2,level+fabs(window)/2);
    LUT->Build();
  }
  this->vtkImageViewer2::SetColorLevel(level);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
double vvSlicer::GetOverlayColorWindow()
{
  if(mOverlayMapper)
    return mOverlayMapper->GetWindow();
  else
    return 0.;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
double vvSlicer::GetOverlayColorLevel()
{
  if(mOverlayMapper)
    return mOverlayMapper->GetLevel();
  else
    return 0.;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::SetOverlayColorWindow(double window)
{
  mOverlayMapper->SetWindow(window);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::SetOverlayColorLevel(double level)
{
  mOverlayMapper->SetLevel(level);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Returns the min an the max value in a 20%x20% region around the mouse pointer
void vvSlicer::GetExtremasAroundMousePointer(double & min, double & max, vtkImageData *image, vtkTransform *transform)
{
  //Get mouse pointer position in view coordinates
  double corner1[3];
  double corner2[3];
  for(int i=0; i<3; i++) {
    corner1[i] = mCurrent[i];
    corner2[i] = mCurrent[i];
  }

  this->Renderer->WorldToView(corner1[0], corner1[1], corner1[2]);
  this->Renderer->WorldToView(corner2[0], corner2[1], corner2[2]);

  // In view coordinates, x is the slicer width and y is the slicer height are the in-plane axis
  int w, h;
  this->Renderer->GetTiledSize(&w, &h);
  corner1[0] -= 0.2*h/(double)w;
  corner2[0] += 0.2*h/(double)w;
  corner1[1] -= 0.2;
  corner2[1] += 0.2;
  this->Renderer->ViewToWorld(corner1[0], corner1[1], corner1[2]);
  this->Renderer->ViewToWorld(corner2[0], corner2[1], corner2[2]);

  //Convert to image pixel coordinates (rounded)
  transform->TransformPoint(corner1, corner1);
  transform->TransformPoint(corner2, corner2);
  int iLocalExtents[6];
  for(int i=0; i<3; i++) {
    corner1[i] = (corner1[i] - image->GetOrigin()[i])/image->GetSpacing()[i];
    corner2[i] = (corner2[i] - image->GetOrigin()[i])/image->GetSpacing()[i];

    iLocalExtents[i*2  ] = lrint(corner1[i]);
    iLocalExtents[i*2+1] = lrint(corner2[i]);

    if(iLocalExtents[i*2  ]>iLocalExtents[i*2+1])
      std::swap(iLocalExtents[i*2], iLocalExtents[i*2+1]);
  }

  vtkSmartPointer<vtkExtractVOI> voiFilter = vtkSmartPointer<vtkExtractVOI>::New();
  voiFilter->SetInput(image);
  voiFilter->SetVOI(iLocalExtents);
  voiFilter->Update();
  if (!voiFilter->GetOutput()->GetNumberOfPoints()) {
    min = 0;
    max = 0;
    return;
  }

  vtkSmartPointer<vtkImageAccumulate> accFilter = vtkSmartPointer<vtkImageAccumulate>::New();
  accFilter->SetInput(voiFilter->GetOutput());
  accFilter->Update();

  min = *(accFilter->GetMin());
  max = *(accFilter->GetMax());
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
double vvSlicer::GetScalarComponentAsDouble(vtkImageData *image, double X, double Y, double Z, int &ix, int &iy, int &iz, int component)
{
  ix = lrint(X);
  iy = lrint(Y);
  iz = lrint(Z);
  if (ix < image->GetWholeExtent()[0] ||
      ix > image->GetWholeExtent()[1] ||
      iy < image->GetWholeExtent()[2] ||
      iy > image->GetWholeExtent()[3] ||
      iz < image->GetWholeExtent()[4] ||
      iz > image->GetWholeExtent()[5] )
    return std::numeric_limits<double>::quiet_NaN();

  image->SetUpdateExtent(ix, ix, iy, iy, iz, iz);
  image->Update();
  return image->GetScalarComponentAsDouble(ix, iy, iz, component);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::Render()
{
  if (this->mFusion && mFusionActor->GetVisibility() && showFusionLegend) {
    legend->SetLookupTable(this->GetFusionMapper()->GetLookupTable());
    legend->UseOpacityOn();
    legend->SetVisibility(1);
  }
  else if (this->GetWindowLevel()->GetLookupTable() && !this->mOverlay)  {
    legend->SetLookupTable(this->GetWindowLevel()->GetLookupTable());
    legend->UseOpacityOff();
    legend->SetVisibility(1);
  } else legend->SetVisibility(0);

  if (ca->GetVisibility()) {
    std::stringstream worldPos;
    double pt[3];
    mConcatenatedTransform->TransformPoint(mCurrent, pt);
    double X = (pt[0] - mImage->GetVTKImages()[mCurrentTSlice]->GetOrigin()[0])/mImage->GetVTKImages()[mCurrentTSlice]->GetSpacing()[0];
    double Y = (pt[1] - mImage->GetVTKImages()[mCurrentTSlice]->GetOrigin()[1])/mImage->GetVTKImages()[mCurrentTSlice]->GetSpacing()[1];
    double Z = (pt[2] - mImage->GetVTKImages()[mCurrentTSlice]->GetOrigin()[2])/mImage->GetVTKImages()[mCurrentTSlice]->GetSpacing()[2];

    if (X >= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[0]-0.5 &&
        X <= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[1]+0.5 &&
        Y >= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[2]-0.5 &&
        Y <= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[3]+0.5 &&
        Z >= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[4]-0.5 &&
        Z <= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[5]+0.5) {

      
      int ix, iy, iz;
      double value = this->GetScalarComponentAsDouble(mImage->GetVTKImages()[mCurrentTSlice], X, Y, Z, ix, iy, iz);

      if(ImageActor->GetVisibility())
        worldPos << "data value : " << value << std::endl;

      worldPos << "mm : " << lrint(mCurrentBeforeSlicingTransform[0]) << ' '
                          << lrint(mCurrentBeforeSlicingTransform[1]) << ' '
                          << lrint(mCurrentBeforeSlicingTransform[2]) << ' '
                          << mCurrentTSlice
                          << std::endl;
      worldPos << "pixel : " << ix << ' '
                             << iy << ' '
                             << iz << ' '
                             << mCurrentTSlice
                             << std::endl;
    }
    ca->SetText(1,worldPos.str().c_str());
  }

  if (pdmA->GetVisibility()) {
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
        zCursor < this->GetImageActor()->GetDisplayExtent()[5]+1 ) {
      vtkRenderer * renderer = this->Renderer;

      renderer->WorldToView(x,y,z);
      renderer->ViewToNormalizedViewport(x,y,z);
      renderer->NormalizedViewportToViewport(x,y);
      renderer->ViewportToNormalizedDisplay(x,y);
      renderer->NormalizedDisplayToDisplay(x,y);
      crossCursor->SetFocalPoint(x,y,z);
    } else
      crossCursor->SetFocalPoint(-1,-1,z);
  }


  if (mOverlay && mOverlayActor->GetVisibility()) {
    if(mLinkOverlayWindowLevel) {
      mOverlayMapper->SetWindow(this->GetColorWindow());
      mOverlayMapper->SetLevel(this->GetColorLevel());
    }
    mOverlayMapper->GetOutput()->SetUpdateExtent(mOverlayActor->GetDisplayExtent());
    mOverlayMapper->GetOutput()->Update();
    mOverlayMapper->Update();
  }
  if (mLandMapper)
    UpdateLandmarks();

  this->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::UpdateCursorPosition()
{
  pdmA->SetVisibility(true);
  mCursor[0] = mCurrent[0];
  mCursor[1] = mCurrent[1];
  mCursor[2] = mCurrent[2];
  mCursor[3] = mCurrentTSlice;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::UpdateLandmarks()
{
  vtkPolyData *pd = static_cast<vtkPolyData*>(mLandClipper->GetInput());
  if (pd->GetPoints()) {
    mLandGlyph->SetRange(0,1);
    mLandGlyph->Modified();
    mLandGlyph->Update();

    mClipBox->Modified();
    mLandClipper->Update();
    mLandMapper->Update();
  }

}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::SetSlice(int slice)
{
  int *range = this->GetSliceRange();
  if (range) {
    if (slice < range[0]) {
      slice = range[0];
    } else if (slice > range[1]) {
      slice = range[1];
    }
  }

  if (this->Slice == slice) {
    return;
  }

  this->Slice = slice;
  SetContourSlice();
  this->Modified();
  this->UpdateDisplayExtent();

  // Seems to work without this line
  //this->Render();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::SetContourSlice()
{
  if (mSurfaceCutActors.size() > 0)
    for (std::vector<vvMeshActor*>::iterator i=mSurfaceCutActors.begin();
         i!=mSurfaceCutActors.end(); i++) {
         
      (*i)->SetSlicingOrientation(this->SliceOrientation);
      (*i)->SetCutSlice((this->Slice)*this->GetImage()->GetSpacing()[this->SliceOrientation]+
                        this->GetImage()->GetOrigin()[this->SliceOrientation]);
    }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::ForceUpdateDisplayExtent()
{
  this->UpdateDisplayExtent();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
int* vvSlicer::GetDisplayExtent()
{
  return this->GetImageActor()->GetDisplayExtent();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::SetVFColor(double r, double g, double b)
{
  double mVFColorHSV[3];
  mVFColor[0] = r;
  mVFColor[1] = g;
  mVFColor[2] = b;

  vtkMath::RGBToHSV(mVFColor, mVFColorHSV);
  mVFColorLUT->SetHueRange(mVFColorHSV[0], mVFColorHSV[0]);
  mVFColorLUT->SetSaturationRange(mVFColorHSV[1],mVFColorHSV[1]);
  mVFColorLUT->SetValueRange(mVFColorHSV[2], mVFColorHSV[2]);

  this->Render();
}  

