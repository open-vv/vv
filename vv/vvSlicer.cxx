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

#include <QMessageBox>
#include <QString>

#include "vvSlicer.h"
#include "vvImage.h"
#include "vvSlicerManagerCommand.h"
#include "vvGlyphSource.h"
#include "vvGlyph2D.h"

#include <vtkVersion.h>
#include <vtkExtentTranslator.h>
#include <vtkAlgorithm.h>
#include <vtkExecutive.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
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
#include <vtkActor2DCollection.h>
#include <vtkGlyph3D.h>
#include <vtkMath.h>
#include <vtkCursor3D.h>
#include <vtkProperty.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkScalarBarActor.h>
#include <vtkImageProperty.h>
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
#  include <vtkOpenGLImageSliceMapper.h>
#  include <vtkImageMapper3D.h>
#  include <vtkImageSliceMapper.h>
#endif

vtkStandardNewMacro(vvSlicer);
static void copyExtent(int* in, int* to){
 for(int i=0; i<6; ++i) 
 {
	to[i]=in[i];
 } 
}
//------------------------------------------------------------------------------
vvSlicer::vvSlicer()
{ 
	mFusionSequenceCode = -1;
  this->UnInstallPipeline();
  mImage = NULL;
  mReducedExtent = new int[6];
  mRegisterExtent = NULL;
  mCurrentTSlice = 0;
  mCurrentFusionTSlice = 0;
  mCurrentOverlayTSlice = 0;
  mUseReducedExtent = false;

  mCurrent[0] = -VTK_DOUBLE_MAX;
  mCurrent[1] = -VTK_DOUBLE_MAX;
  mCurrent[2] = -VTK_DOUBLE_MAX;

  mCursor[0] = 0;//-VTK_DOUBLE_MAX;
  mCursor[1] = 0;//-VTK_DOUBLE_MAX;
  mCursor[2] = 0;//-VTK_DOUBLE_MAX;
  mCursor[3] = 0;//-VTK_DOUBLE_MAX;

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
#if VTK_MAJOR_VERSION <= 5
  pdm->SetInput(crossCursor->GetOutput());
#else
  pdm->SetInputConnection(crossCursor->GetOutputPort(0));
#endif

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

  this->InstallPipeline();

  mLinkOverlayWindowLevel = true;
  mImageVisibility = true;

#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
  this->GetImageActor()->GetMapper()->BorderOn();
#endif

  mSlicingTransform = vtkSmartPointer<vtkTransform>::New();
  mConcatenatedTransform = vtkSmartPointer<vtkTransform>::New();
  mConcatenatedFusionTransform = vtkSmartPointer<vtkTransform>::New();
  mConcatenatedOverlayTransform = vtkSmartPointer<vtkTransform>::New();
  mFirstSetSliceOrientation = true;
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
void vvSlicer::SetInterpolationImageReslice(int interpolation)
{ 
  mImageReslice->SetInterpolationMode(interpolation);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSlicer::SetCurrentPosition(double x, double y, double z, int t)
{ 
  mCurrentBeforeSlicingTransform[0]=x;
  mCurrentBeforeSlicingTransform[1]=y;
  mCurrentBeforeSlicingTransform[2]=z;
  mSlicingTransform->GetInverse()->TransformPoint(mCurrentBeforeSlicingTransform,mCurrent);
  if (t>=0) SetTSlice(t);
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
    //mImageReslice->SetResliceAxes(mConcatenatedTransform->GetMatrix());
#if VTK_MAJOR_VERSION <= 5
    mImageReslice->SetInput(0, mImage->GetFirstVTKImageData());
#else
    mImageReslice->SetInputData(0, mImage->GetFirstVTKImageData());
#endif
    mImageReslice->UpdateInformation();

#if VTK_MAJOR_VERSION <= 5
    this->Superclass::SetInput(mImageReslice->GetOutput());
#else
    this->Superclass::SetInputConnection(mImageReslice->GetOutputPort());
#endif

    int extent[6];
#if VTK_MAJOR_VERSION <= 5
    this->GetInput()->GetWholeExtent(extent);
#else
    int* ext = mImageReslice->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
    copyExtent(ext, extent);
#endif

    // Prevent crash when reload -> change slice if outside extent
    if (Slice < extent[SliceOrientation*2] || Slice>=extent[SliceOrientation*2+1]) {
      Slice = (extent[SliceOrientation*2+1]+extent[SliceOrientation*2])/2.0;
    }

    // Make sure that the required part image has been computed
    extent[SliceOrientation*2] = Slice;
    extent[SliceOrientation*2+1] = Slice;    
#if VTK_MAJOR_VERSION <= 5
    mImageReslice->GetOutput()->SetUpdateExtent(extent);
    mImageReslice->GetOutput()->Update();
#else
    mImageReslice->SetUpdateExtent(extent);
    mImageReslice->Update();
#endif

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
    mOverlayVisibility = true;
    
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
#if VTK_MAJOR_VERSION <= 5
    mOverlayReslice->SetInput(0, mOverlay->GetFirstVTKImageData());
    mImageReslice->UpdateInformation();
#else
    mOverlayReslice->SetInputData(0, mOverlay->GetFirstVTKImageData());
#endif
    mOverlayReslice->Update();

    if (!mOverlayMapper)
      mOverlayMapper = vtkSmartPointer<vtkImageMapToWindowLevelColors>::New();
#if VTK_MAJOR_VERSION <= 5
    mOverlayMapper->SetInput(mOverlayReslice->GetOutput());
#else
    mOverlayMapper->SetInputConnection(mOverlayReslice->GetOutputPort(0));
#endif

    if (!mOverlayActor) {
      mOverlayActor = vtkSmartPointer<vvBlendImageActor>::New();
#if VTK_MAJOR_VERSION <= 5
      mOverlayActor->SetInput(mOverlayMapper->GetOutput());
#else
      mOverlayActor->GetMapper()->SetInputConnection(mOverlayMapper->GetOutputPort());
#endif
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
void vvSlicer::SetFusion(vvImage::Pointer fusion, int fusionSequenceCode)
{ 
	mFusionSequenceCode = fusionSequenceCode;
  if (fusion->GetVTKImages().size()) {
    mFusion = fusion;
    mFusionVisibility = true;

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
#if VTK_MAJOR_VERSION <= 5
    mFusionReslice->SetInput(0, mFusion->GetFirstVTKImageData());
    mFusionReslice->UpdateInformation();
#else
    mFusionReslice->SetInputData(0, mFusion->GetFirstVTKImageData());
#endif
    mFusionReslice->Update();

    if (!mFusionMapper)
      mFusionMapper = vtkSmartPointer<vtkImageMapToColors>::New();

    vtkSmartPointer<vtkLookupTable> lut = vtkLookupTable::New();
    lut->SetRange(0, 1);
    lut->SetValueRange(0, 1);
    lut->SetSaturationRange(0, 0);
    lut->Build();
    mFusionMapper->SetLookupTable(lut);
#if VTK_MAJOR_VERSION <= 5
    mFusionMapper->SetInput(mFusionReslice->GetOutput());
#else
    mFusionMapper->SetInputConnection(mFusionReslice->GetOutputPort(0));
#endif
    
    if (!mFusionActor) {
      mFusionActor = vtkSmartPointer<vtkImageActor>::New();
#if VTK_MAJOR_VERSION <= 5
      mFusionActor->SetInput(mFusionMapper->GetOutput());
#else
      mFusionActor->GetMapper()->SetInputConnection(mFusionMapper->GetOutputPort());
#endif
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
  if (actor_type == "image")
    vis = mImageVisibility;
  else if (actor_type == "vector")
    vis = mVFVisibility;
  else if (actor_type == "overlay")
    vis = mOverlayVisibility;
  else if ( (actor_type == "fusion") || (actor_type == "fusionSequence") )
    vis = mFusionVisibility;
  else if (actor_type == "contour")
    vis = this->mSurfaceCutActors[overlay_index]->GetActor()->GetVisibility();
  return vis;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSlicer::SetActorVisibility(const std::string& actor_type, int overlay_index ,bool vis)
{ 
  if (actor_type == "image")
    mImageVisibility = vis;
  else if (actor_type == "vector")
    mVFVisibility = vis;
  else if (actor_type == "overlay")
    mOverlayVisibility = vis;
  else if ( (actor_type == "fusion") || (actor_type == "fusionSequence") )
    mFusionVisibility = vis;
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
    mVFVisibility = true;

    if (!mAAFilter) {
      mAAFilter= vtkSmartPointer<vtkAssignAttribute>::New();
      mVOIFilter = vtkSmartPointer<vtkExtractVOI>::New();
      mVOIFilter->SetSampleRate(mSubSampling,mSubSampling,mSubSampling);
    }
#if VTK_MAJOR_VERSION <= 5
    mVOIFilter->SetInput(vf->GetFirstVTKImageData());
    mAAFilter->SetInput(mVOIFilter->GetOutput());
#else
    mVOIFilter->SetInputData(vf->GetFirstVTKImageData());
    mAAFilter->SetInputConnection(mVOIFilter->GetOutputPort());
#endif
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
#if VTK_MAJOR_VERSION <= 5
    mGlyphFilter->SetInput(mAAFilter->GetOutput());
    mGlyphFilter->SetSource(mArrow->GetOutput());
#else
    mGlyphFilter->SetInputConnection(mAAFilter->GetOutputPort());
    mGlyphFilter->SetSourceConnection(mArrow->GetOutputPort());
#endif
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
#if VTK_MAJOR_VERSION <= 5
    mVFMapper->SetInput(mGlyphFilter->GetOutput());
#else
    mVFMapper->SetInputConnection(mGlyphFilter->GetOutputPort());
#endif
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
	if (!mClipBox)
      mClipBox = vtkSmartPointer<vtkBox>::New();
    if (!mLandClipper)
      mLandClipper = vtkSmartPointer<vvClipPolyData>::New();
    if (!mLandGlyph)
      mLandGlyph = vtkSmartPointer<vtkGlyph3D>::New();
    if (!mLandMapper)
      mLandMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    if (!mLandActor)
      mLandActor = vtkSmartPointer<vtkActor>::New();

    mCross->SetFocalPoint(0.0,0.0,0.0);
    mCross->SetModelBounds(-10,10,-10,10,-10,10);
    mCross->AllOff();
    mCross->AxesOn();

    mLandClipper->SetClipFunction(mClipBox);
    mLandClipper->InsideOutOn();
#if VTK_MAJOR_VERSION <= 5
    mLandmarkTransform = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    mLandmarkTransform->SetInput(mLandmarks->GetOutput());
    mConcatenatedTransform->Identity();
    mConcatenatedTransform->Concatenate(mImage->GetTransform()[0]);
    mConcatenatedTransform->Concatenate(mSlicingTransform);
    mLandmarkTransform->SetTransform(mConcatenatedTransform->GetInverse());
    mLandClipper->SetInput(mLandmarkTransform->GetOutput());

    mLandGlyph->SetSource(mCross->GetOutput());
    mLandGlyph->SetInput(mLandClipper->GetOutput());
#else
    mLandmarkTransform = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    mLandmarkTransform->SetInputData(mLandmarks->GetOutput());
    mConcatenatedTransform->Identity();
    mConcatenatedTransform->Concatenate(mImage->GetTransform()[0]);
    mConcatenatedTransform->Concatenate(mSlicingTransform);
    mLandmarkTransform->SetTransform(mConcatenatedTransform->GetInverse());
    mLandClipper->SetInputConnection(mLandmarkTransform->GetOutputPort());

    mLandGlyph->SetSourceConnection(mCross->GetOutputPort());
    mLandGlyph->SetInputConnection(mLandClipper->GetOutputPort());
#endif
    //mLandGlyph->SetIndexModeToScalar();
    //mLandGlyph->SetRange(0,1);
    //mLandGlyph->ScalingOff();

    //mLandGlyph->SetColorModeToColorByScalar();
    
    mLandGlyph->SetScaleModeToDataScalingOff();
    mLandGlyph->SetIndexModeToOff();

    mLandMapper->SetInputConnection(mLandGlyph->GetOutputPort());
    //mLandMapper->ScalarVisibilityOff();

    mLandActor->SetMapper(mLandMapper);
    mLandActor->GetProperty()->SetOpacity(0.995);  //in order to get VTK to turn on the alpha-blending in OpenGL
    mLandActor->GetProperty()->SetColor(255,10,212);
    mLandActor->SetPickable(0);
    mLandActor->SetVisibility(true);
    this->UpdateDisplayExtent();
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
  if ( (actor_type == "fusion") || (actor_type == "fusionSequence") ) {
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
void vvSlicer::SetTSlice(int t, bool updateLinkedImages)
{ 
	if (!updateLinkedImages) {
		mCurrentTSlice = t;
#if VTK_MAJOR_VERSION <= 5
                mImageReslice->SetInput( mImage->GetVTKImages()[mCurrentTSlice] );
#else
                mImageReslice->SetInputData( mImage->GetVTKImages()[mCurrentTSlice] );
#endif
		// Update transform
		mConcatenatedTransform->Identity();
		mConcatenatedTransform->Concatenate(mImage->GetTransform()[mCurrentTSlice]);
		mConcatenatedTransform->Concatenate(mSlicingTransform);
		UpdateDisplayExtent();
		return;
	}
  if (t < 0)
    mCurrentTSlice = 0;
  else if ((unsigned int)t >= mImage->GetVTKImages().size())
    mCurrentTSlice = mImage->GetVTKImages().size() -1;
  else
    mCurrentTSlice = t;

  // Update transform
  mConcatenatedTransform->Identity();
  mConcatenatedTransform->Concatenate(mImage->GetTransform()[mCurrentTSlice]);
  mConcatenatedTransform->Concatenate(mSlicingTransform);

  // Update image data
#if VTK_MAJOR_VERSION <= 5
   mImageReslice->SetInput( mImage->GetVTKImages()[mCurrentTSlice] );
#else
  mImageReslice->SetInputData( mImage->GetVTKImages()[mCurrentTSlice] );
#endif
  if (mVF && mVFActor->GetVisibility()) {
    if (mVF->GetVTKImages().size() > (unsigned int)mCurrentTSlice)
#if VTK_MAJOR_VERSION <= 5
      mVOIFilter->SetInput(mVF->GetVTKImages()[mCurrentTSlice]);
#else
      mVOIFilter->SetInputData(mVF->GetVTKImages()[mCurrentTSlice]);
#endif
  }
  //update the overlay
  if (mOverlay && mOverlayActor->GetVisibility()) {
    if (mOverlay->GetVTKImages().size() > (unsigned int)t) {
      mCurrentOverlayTSlice = t;
#if VTK_MAJOR_VERSION <= 5
      mOverlayReslice->SetInput( mOverlay->GetVTKImages()[mCurrentOverlayTSlice] );
#else
      mOverlayReslice->SetInputData( mOverlay->GetVTKImages()[mCurrentOverlayTSlice] );
#endif
      // Update overlay transform
      mConcatenatedOverlayTransform->Identity();
      mConcatenatedOverlayTransform->Concatenate(mOverlay->GetTransform()[mCurrentOverlayTSlice]);
      mConcatenatedOverlayTransform->Concatenate(mSlicingTransform);
    }
  }
  //update the fusion ; except in case this is a fusionSequence, in which case both 'times' should be independent.
  if (mFusion && mFusionActor->GetVisibility() && (mFusionSequenceCode<0)) {
    if (mFusion->GetVTKImages().size() > (unsigned int)t) {
      mCurrentFusionTSlice = t;
#if VTK_MAJOR_VERSION <= 5
      mFusionReslice->SetInput( mFusion->GetVTKImages()[mCurrentFusionTSlice]);
#else
      mFusionReslice->SetInputData( mFusion->GetVTKImages()[mCurrentFusionTSlice]);
#endif

      // Update fusion transform
      mConcatenatedFusionTransform->Identity();
      mConcatenatedFusionTransform->Concatenate(mFusion->GetTransform()[mCurrentFusionTSlice]);
      mConcatenatedFusionTransform->Concatenate(mSlicingTransform);
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
void vvSlicer::SetFusionSequenceTSlice(int t)
{ 
  if (mFusion && mFusionActor->GetVisibility() && (mFusionSequenceCode>=0)) {
    if (mFusion->GetVTKImages().size() > (unsigned int)t) {
      mCurrentFusionTSlice = t;
#if VTK_MAJOR_VERSION <= 5
      mFusionReslice->SetInput( mFusion->GetVTKImages()[mCurrentFusionTSlice] );
#else
      mFusionReslice->SetInputData( mFusion->GetVTKImages()[mCurrentFusionTSlice] );
#endif
      // Update fusion transform
      mConcatenatedFusionTransform->Identity();
      mConcatenatedFusionTransform->Concatenate(mFusion->GetTransform()[mCurrentFusionTSlice]); //not really useful...
      mConcatenatedFusionTransform->Concatenate(mSlicingTransform);
    }
  }

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
int vvSlicer::GetMaxCurrentTSlice()
{ 
  int t = mCurrentTSlice;
  if(mOverlay && mOverlayActor->GetVisibility())
    t = std::max(t, mCurrentOverlayTSlice);
  if(mFusion&& (mFusionSequenceCode<0) && mFusionActor->GetVisibility()) //ignore fusionSequence data: for these, the times are not to be related (this way)
    t = std::max(t, mCurrentFusionTSlice);
  return t;
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
#if VTK_MAJOR_VERSION <= 5
    this->GetInput()->GetWholeExtent(extent);
#else
    int* ext = mImageReslice->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
    copyExtent(ext, extent);
#endif

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
  
  // Go to current cursor position
  // double* cursorPos = GetCursorPosition();
  // DDV(cursorPos, 3);
  // SetCurrentPosition(cursorPos[0],cursorPos[1],cursorPos[2],cursorPos[3]);

  if (mFirstSetSliceOrientation) {
    int *range = this->GetSliceRange();
    if (range)
      this->Slice = static_cast<int>((range[0] + range[1]) * 0.5);
#if VTK_MAJOR_VERSION <= 5
      mFirstSetSliceOrientation = false;
#endif
  }
  else if (this->Renderer && this->GetInput()) {
    double s = mCursor[orientation];
    double sCursor = (s - this->GetInput()->GetOrigin()[orientation])/this->GetInput()->GetSpacing()[orientation];
    this->Slice = static_cast<int>(sCursor);
  }

  this->UpdateOrientation();
  
  this->UpdateDisplayExtent();
  
  if (mFirstSetSliceOrientation) {
    mFirstSetSliceOrientation = false;
  }
  
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
#if VTK_MAJOR_VERSION <= 5
  reslice->GetOutput()->UpdateInformation();
#else
  reslice->UpdateInformation();
#endif

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

  // Step 2: round to nearest grid positionInc. This has been validated as the only
  // way to have something consistent with the thickness of a 2D slice visible on the
  // other slices. The thickness is accounted for so if the 2D slice is to thin and
  // between two slices, one will never be able to see this 2D slice (bug #1883).
  origin[this->SliceOrientation] = itk::Math::Round<double>(origin[this->SliceOrientation]);

  // Step 3: back to world coordinates
  origin[this->SliceOrientation] *= mImageReslice->GetOutput()->GetSpacing()[this->SliceOrientation];
  origin[this->SliceOrientation] += mImageReslice->GetOutput()->GetOrigin()[this->SliceOrientation];

  // Set new spacing and origin
  reslice->SetOutputOrigin(origin);
  reslice->SetOutputSpacing(spacing);
  reslice->UpdateInformation();
#if VTK_MAJOR_VERSION <= 5
  reslice->GetOutput()->UpdateInformation();
#endif
}
//------------------------------------------------------------------------------

//----------------------------------------------------------------------------
int * vvSlicer::GetExtent()
{ 
  int *w_ext;
  if (mUseReducedExtent) {
    w_ext = mReducedExtent;
  }
  else {
#if VTK_MAJOR_VERSION <= 5
    w_ext = GetInput()->GetWholeExtent();
#else
    w_ext = mImageReslice->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
#endif
  }
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
  emit UpdateDisplayExtentBegin(mSlicerNumber);
  vtkImageData *input = this->GetInput();
  if (!input || !this->ImageActor) {
    return;
  }

#if VTK_MAJOR_VERSION <= 5
  input->UpdateInformation();
#else
  mRegisterExtent = mImageReslice->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
#endif
  this->SetSlice( this->GetSlice() ); //SR: make sure the update let the slice in extents

  // Local copy of extent
  int w_ext[6];
#if VTK_MAJOR_VERSION <= 5
  int* ext = GetExtent();
#else
  int* ext = mImageReslice->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
#endif
  copyExtent(ext, w_ext);
  if (mUseReducedExtent) {
        copyExtent(mReducedExtent, w_ext);
    }
  // Set slice value

  w_ext[ this->SliceOrientation*2   ] = this->Slice;
  w_ext[ this->SliceOrientation*2+1 ] = this->Slice;
  
  // Image actor
  this->ImageActor->SetVisibility(mImageVisibility);
  this->ImageActor->SetDisplayExtent(w_ext);
  
#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
  // Fix for bug #1882
  dynamic_cast<vtkImageSliceMapper *>(this->ImageActor->GetMapper())->SetOrientation(this->GetOrientation());
#endif

  // Overlay image actor
  if (mOverlay && mOverlayVisibility) {
    AdjustResliceToSliceOrientation(mOverlayReslice);
    int overExtent[6];
#if VTK_MAJOR_VERSION <= 5
    this->ConvertImageToImageDisplayExtent(input, w_ext, mOverlayReslice->GetOutput(), overExtent);
    bool out = ClipDisplayedExtent(overExtent, mOverlayMapper->GetInput()->GetWholeExtent());
#else
    this->ConvertImageToImageDisplayExtent(mImageReslice->GetOutputInformation(0), w_ext, mOverlayReslice->GetOutput(), overExtent);
    bool out = ClipDisplayedExtent(overExtent, mOverlayMapper->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
#endif
    mOverlayActor->SetVisibility(!out);
    mOverlayActor->SetDisplayExtent( overExtent );
#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
    // Fix for bug #1882
    dynamic_cast<vtkImageSliceMapper *>(mOverlayActor->GetMapper())->SetOrientation(this->GetOrientation());
#endif
  }
  else if(mOverlay)
    mOverlayActor->SetVisibility(false);

  // Fusion image actor
  if (mFusion && mFusionVisibility) {
    AdjustResliceToSliceOrientation(mFusionReslice);
    int fusExtent[6];
#if VTK_MAJOR_VERSION <= 5
    this->ConvertImageToImageDisplayExtent(input, w_ext, mFusionReslice->GetOutput(), fusExtent);
    bool out = ClipDisplayedExtent(fusExtent, mFusionMapper->GetInput()->GetWholeExtent());
#else
    this->ConvertImageToImageDisplayExtent(mImageReslice->GetOutputInformation(0), w_ext, mFusionReslice->GetOutput(), fusExtent);
    bool out = ClipDisplayedExtent(fusExtent, mFusionMapper->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
#endif
    mFusionActor->SetVisibility(!out);
    mFusionActor->SetDisplayExtent( fusExtent );
#if VTK_MAJOR_VERSION >= 6 || (VTK_MAJOR_VERSION >= 5 && VTK_MINOR_VERSION >= 10)
    // Fix for bug #1882
    dynamic_cast<vtkImageSliceMapper *>(mFusionActor->GetMapper())->SetOrientation(this->GetOrientation());
#endif
  }
  else if(mFusion)
    mFusionActor->SetVisibility(false);
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
  
  if (mVF && mVFVisibility) {
    int vfExtent[6];
#if VTK_MAJOR_VERSION <= 5
    mVF->GetVTKImages()[0]->UpdateInformation();
    this->ConvertImageToImageDisplayExtent(input, w_ext, mVF->GetVTKImages()[0], vfExtent);
    bool out = ClipDisplayedExtent(vfExtent, mVOIFilter->GetInput()->GetWholeExtent());
#else
    mVOIFilter->Update();
    this->ConvertImageToImageDisplayExtent(mImageReslice->GetOutputInformation(0), w_ext, mVF->GetVTKImages()[0], vfExtent);
    bool out = ClipDisplayedExtent(vfExtent, mVOIFilter->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
#endif
    mVFActor->SetVisibility(!out);
    mVOIFilter->SetVOI(vfExtent);
    int orientation[3] = {1,1,1};
    orientation[this->SliceOrientation] = 0;
    mGlyphFilter->SetOrientation(orientation[0], orientation[1], orientation[2]);
    position[this->SliceOrientation] += offset;
    mVFActor->SetPosition(position);
    mVFActor->GetProperty()->SetOpacity(0.995); //in order to get VTK to turn on the alpha-blending in OpenGL
    mVFMapper->Update();

  }
  else if(mVF)
    mVFActor->SetVisibility(false);
    
    
    double boundsT [6];
      for(unsigned int i=0; i<6; i++)
        boundsT[i] = ImageActor->GetBounds()[i];
      boundsT[ this->SliceOrientation*2   ] = ImageActor->GetBounds()[ this->SliceOrientation*2  ]-fabs(this->GetInput()->GetSpacing()[this->SliceOrientation]);
      boundsT[ this->SliceOrientation*2+1 ] = ImageActor->GetBounds()[ this->SliceOrientation*2+1 ]+fabs(this->GetInput()->GetSpacing()[this->SliceOrientation]);


  // Landmarks actor
  if (mLandActor) {
    if (mClipBox) {
      RemoveLandmarks();
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
    
    if (mLandActor) {
        if (mClipBox) {
            DisplayLandmarks();
        }
    }
  }
  emit UpdateDisplayExtentEnd(mSlicerNumber);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::ConvertImageToImageDisplayExtent(vtkInformation *sourceImage, const int sourceExtent[6],
                                                vtkImageData *targetImage, int targetExtent[6])
{ 
  double dExtents[6];
  double *origin, *spacing;
  origin = sourceImage->Get(vtkDataObject::ORIGIN());
  spacing = sourceImage->Get(vtkDataObject::SPACING());
  for(unsigned int i=0; i<6; i++) {
    // From source voxel coordinates to world coordinates
    dExtents[i] = origin[i/2] + spacing[i/2] * sourceExtent[i];

    // From world coordinates to floating point target voxel coordinates
    dExtents[i] = (dExtents[i]- targetImage->GetOrigin()[i/2]) / targetImage->GetSpacing()[i/2];
    
    // Round to current slice or larger extent
    if(i/2==this->GetOrientation())
      targetExtent[i] = itk::Math::Round<double>(dExtents[i]);
    else if(i%2==1)
      targetExtent[i] = itk::Math::Ceil<double>(dExtents[i]);
    else
      targetExtent[i] = itk::Math::Floor<double>(dExtents[i]);
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
    
    // Round to current slice or larger extent
    if(i/2==this->GetOrientation())
      targetExtent[i] = itk::Math::Round<double>(dExtents[i]);
    else if(i%2==1)
      targetExtent[i] = itk::Math::Ceil<double>(dExtents[i]);
    else
      targetExtent[i] = itk::Math::Floor<double>(dExtents[i]);
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
bool vvSlicer::ClipDisplayedExtent(int extent[6], int refExtent[6])
{ 
  bool out = false;
  int maxBound = 6;

  for (int i = 0; i < maxBound; i = i+2) {
    //if we are totally outside the image
    if ( extent[i] > refExtent[i+1] || extent[i+1] < refExtent[i] ) {
      out = true;
      break;
    }
    //crop to the limit of the image
    extent[i] = std::max(extent[i], refExtent[i]);
    extent[i] = std::min(extent[i], refExtent[i+1]);;
    extent[i+1] = std::max(extent[i+1], refExtent[i]);
    extent[i+1] = std::min(extent[i+1], refExtent[i+1]);;
  }
  if (out)
    for (int i = 0; i < maxBound; i = i+2) {
      extent[i] = refExtent[i];
      extent[i+1] = refExtent[i];
    }
  return out;
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
	if (i) UpdateDisplayExtent();
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
  if(mOverlayMapper)
    mOverlayMapper->SetWindow(window);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicer::SetOverlayColorLevel(double level)
{ 
  if(mOverlayMapper)
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

#if VTK_MAJOR_VERSION > 5
    for(int j=0;j<2; j++) {
      if(iLocalExtents[i*2+j]< mImageReslice->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT())[2*i])
        iLocalExtents[i*2+j] = mImageReslice->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT())[2*i];

      if(iLocalExtents[i*2+j]> mImageReslice->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT())[2*i+1])
        iLocalExtents[i*2+j] = mImageReslice->GetInputInformation()->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT())[2*i+1];
    }
#endif
  }

  vtkSmartPointer<vtkExtractVOI> voiFilter = vtkSmartPointer<vtkExtractVOI>::New();
#if VTK_MAJOR_VERSION <= 5
  voiFilter->SetInput(image);
#else
  voiFilter->SetInputData(image);
#endif
  voiFilter->SetVOI(iLocalExtents);
  voiFilter->Update();
  if (!voiFilter->GetOutput()->GetNumberOfPoints()) {
    min = 0;
    max = 0;
    return;
  }

  vtkSmartPointer<vtkImageAccumulate> accFilter = vtkSmartPointer<vtkImageAccumulate>::New();
#if VTK_MAJOR_VERSION <= 5
  accFilter->SetInput(voiFilter->GetOutput());
#else
  accFilter->SetInputConnection(voiFilter->GetOutputPort(0));
#endif
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
#if VTK_MAJOR_VERSION <= 5
  if (ix < image->GetWholeExtent()[0] ||
      ix > image->GetWholeExtent()[1] ||
      iy < image->GetWholeExtent()[2] ||
      iy > image->GetWholeExtent()[3] ||
      iz < image->GetWholeExtent()[4] ||
      iz > image->GetWholeExtent()[5] )
    return std::numeric_limits<double>::quiet_NaN();
  image->SetUpdateExtent(ix, ix, iy, iy, iz, iz);
  image->Update();
#else
  if (ix < image->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[0] ||
      ix > image->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[1] ||
      iy < image->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[2] ||
      iy > image->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[3] ||
      iz < image->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[4] ||
      iz > image->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[5] )
    return std::numeric_limits<double>::quiet_NaN();
  //image->SetUpdateExtent(ix, ix, iy, iy, iz, iz);
  //image->Update();
#endif
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
    std::stringstream worldPos(" ");
    double pt[3];
    mConcatenatedTransform->TransformPoint(mCurrent, pt);
    double X = (pt[0] - mImage->GetVTKImages()[mCurrentTSlice]->GetOrigin()[0])/mImage->GetVTKImages()[mCurrentTSlice]->GetSpacing()[0];
    double Y = (pt[1] - mImage->GetVTKImages()[mCurrentTSlice]->GetOrigin()[1])/mImage->GetVTKImages()[mCurrentTSlice]->GetSpacing()[1];
    double Z = (pt[2] - mImage->GetVTKImages()[mCurrentTSlice]->GetOrigin()[2])/mImage->GetVTKImages()[mCurrentTSlice]->GetSpacing()[2];
#if VTK_MAJOR_VERSION <= 5
    if (X >= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[0]-0.5 &&
        X <= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[1]+0.5 &&
        Y >= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[2]-0.5 &&
        Y <= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[3]+0.5 &&
        Z >= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[4]-0.5 &&
        Z <= mImage->GetVTKImages()[mCurrentTSlice]->GetWholeExtent()[5]+0.5) {
#else
    if (X >= mImage->GetVTKImages()[mCurrentTSlice]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[0]-0.5 &&
        X <= mImage->GetVTKImages()[mCurrentTSlice]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[1]+0.5 &&
        Y >= mImage->GetVTKImages()[mCurrentTSlice]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[2]-0.5 &&
        Y <= mImage->GetVTKImages()[mCurrentTSlice]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[3]+0.5 &&
        Z >= mImage->GetVTKImages()[mCurrentTSlice]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[4]-0.5 &&
        Z <= mImage->GetVTKImages()[mCurrentTSlice]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[5]+0.5) {
#endif
      int ix, iy, iz;
      double value = this->GetScalarComponentAsDouble(mImage->GetVTKImages()[mCurrentTSlice], X, Y, Z, ix, iy, iz);
      if(ImageActor->GetVisibility())
        worldPos << "data value : " << value << std::endl;
      worldPos << "mm : " << lrint(mCurrent[0]) << ' '
                          << lrint(mCurrent[1]) << ' '
                          << lrint(mCurrent[2]) << ' '
                          << mCurrentTSlice
                          << std::endl;
      worldPos << "pixel : " << ix << ' '
                             << iy << ' '
                             << iz << ' '
                             << mCurrentTSlice
                             << std::endl;
    
    }
    ca->SetText(1,worldPos.str().c_str());

    std::stringstream slicePos;
    slicePos << "Slice: " << this->GetSlice();
    ca->SetText(2, slicePos.str().c_str());
  }

  if (pdmA->GetVisibility()) {
    double x = mCursor[0];
    double y = mCursor[1];
    double z = mCursor[2];
    double xCursor = (x - this->GetInput()->GetOrigin()[0])/this->GetInput()->GetSpacing()[0];
    double yCursor = (y - this->GetInput()->GetOrigin()[1])/this->GetInput()->GetSpacing()[1];
    double zCursor = (z - this->GetInput()->GetOrigin()[2])/this->GetInput()->GetSpacing()[2];
#if VTK_MAJOR_VERSION <= 5    
    if (xCursor >= this->GetImageActor()->GetDisplayExtent()[0]-0.5 &&
        xCursor < this->GetImageActor()->GetDisplayExtent()[1]+0.5 &&
        yCursor >= this->GetImageActor()->GetDisplayExtent()[2]-0.5 &&
        yCursor < this->GetImageActor()->GetDisplayExtent()[3]+0.5 &&
        zCursor >= this->GetImageActor()->GetDisplayExtent()[4]-0.5 &&
        zCursor < this->GetImageActor()->GetDisplayExtent()[5]+0.5 ) {
      vtkRenderer * renderer = this->Renderer;

      renderer->WorldToView(x,y,z);
      renderer->ViewToNormalizedViewport(x,y,z);
      renderer->NormalizedViewportToViewport(x,y);
      renderer->ViewportToNormalizedDisplay(x,y);
      renderer->NormalizedDisplayToDisplay(x,y);
      crossCursor->SetFocalPoint(x,y,z);
    } else
      crossCursor->SetFocalPoint(-1,-1,z);
    crossCursor->Update();
  }
#else
    vtkSmartPointer<vtkOpenGLImageSliceMapper> mapperOpenGL= vtkSmartPointer<vtkOpenGLImageSliceMapper>::New();
    try {
        mapperOpenGL = dynamic_cast<vtkOpenGLImageSliceMapper*>(GetImageActor()->GetMapper());
    } catch (const std::bad_cast& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Conversion error" << std::endl;
		return;
	}

    if (xCursor >= mapperOpenGL->GetCroppingRegion()[0]-0.5 &&
        xCursor < mapperOpenGL->GetCroppingRegion()[1]+0.5 &&
        yCursor >= mapperOpenGL->GetCroppingRegion()[2]-0.5 &&
        yCursor < mapperOpenGL->GetCroppingRegion()[3]+0.5 &&
        zCursor >= mapperOpenGL->GetCroppingRegion()[4]-0.5 &&
        zCursor < mapperOpenGL->GetCroppingRegion()[5]+0.5 ) {
      vtkRenderer * renderer = this->Renderer;

      renderer->WorldToView(x,y,z);
      renderer->ViewToNormalizedViewport(x,y,z);
      renderer->NormalizedViewportToViewport(x,y);
      renderer->ViewportToNormalizedDisplay(x,y);
      renderer->NormalizedDisplayToDisplay(x,y);
      crossCursor->SetFocalPoint(x,y,z);
    } else
      crossCursor->SetFocalPoint(-1,-1,z);
    crossCursor->Update();
  }
#endif

  if (mOverlay && mOverlayActor->GetVisibility()) {
    if(mLinkOverlayWindowLevel) {
      mOverlayMapper->SetWindow(this->GetColorWindow());
      mOverlayMapper->SetLevel(this->GetColorLevel());
    }
#if VTK_MAJOR_VERSION <= 5
    mOverlayMapper->GetOutput()->SetUpdateExtent(mOverlayActor->GetDisplayExtent());
#else
    mOverlayMapper->SetUpdateExtent(mOverlayActor->GetDisplayExtent());
#endif
    mOverlayMapper->Update();
  }
  if (mFusion && mFusionActor->GetVisibility()) {
#if VTK_MAJOR_VERSION <= 5
    mFusionMapper->GetOutput()->SetUpdateExtent(mFusionActor->GetDisplayExtent());
#else
    mFusionMapper->SetUpdateExtent(mFusionActor->GetDisplayExtent());
#endif
    mFusionMapper->Update();
  }
  if (mLandMapper) {
    RemoveLandmarks();
    DisplayLandmarks();
  }

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
void vvSlicer::RemoveLandmarks()
{ 
  vtkPolyData *pd = static_cast<vtkPolyData*>(mLandmarks->GetOutput());
  if (pd->GetPoints()) {

    //First remove all captions:
    for(unsigned int i=0;i<mLandLabelActors.size();i++) {
	this->Renderer->RemoveActor2D(mLandLabelActors[i]);
	//allActors2D->Remove (mLandLabelActors[i]);
    }
    mLandLabelActors.clear();
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::DisplayLandmarks()
{ 

  double bounds [6];
  for(unsigned int i=0; i<6; i++)
    bounds[i] = ImageActor->GetBounds()[i];
  bounds[ this->SliceOrientation*2   ] = ImageActor->GetBounds()[ this->SliceOrientation*2  ]-fabs(this->GetInput()->GetSpacing()[this->SliceOrientation]);
  bounds[ this->SliceOrientation*2+1 ] = ImageActor->GetBounds()[ this->SliceOrientation*2+1 ]+fabs(this->GetInput()->GetSpacing()[this->SliceOrientation]);
  mClipBox->SetBounds(bounds);


  vtkPolyData *pd = static_cast<vtkPolyData*>(mLandmarks->GetOutput());
  if (pd->GetPoints()) {
    this->GetRenderer()->AddActor(mLandActor);
    //mLandGlyph->SetRange(0,1);
    //mLandGlyph->Modified();
    //mLandGlyph->Update();
    mClipBox->Modified();
    mLandClipper->Update();
    mLandMapper->Update();
    //Next add the captions to the displayed points
    for (vtkIdType id=0; id<mLandClipper->GetOutput()->GetNumberOfPoints(); id++) {
	  double *position = mLandClipper->GetOutput()->GetPoint(id);
      vtkStdString label = static_cast<vtkStringArray*>(mLandClipper->GetOutput()->GetPointData()->GetAbstractArray("labels"))->GetValue(id);
      vtkSmartPointer<vtkCaptionActor2D> label_actor = vtkSmartPointer<vtkCaptionActor2D>::New();
      label_actor->SetCaption(label);
      label_actor->SetAttachmentPoint(position);
      label_actor->GetCaptionTextProperty()->SetColor(1,0,0);
      label_actor->GetCaptionTextProperty()->SetOrientation(33.333333);
      label_actor->GetCaptionTextProperty()->SetFontFamilyToTimes();
      label_actor->GetCaptionTextProperty()->SetBold(0);
      label_actor->GetCaptionTextProperty()->SetFontSize(6);
      label_actor->BorderOff();
      label_actor->LeaderOff();
      label_actor->ThreeDimensionalLeaderOff();
      mLandLabelActors.push_back(label_actor);
      this->Renderer->AddActor2D(mLandLabelActors[id]);
     }
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
int vvSlicer::GetTMax() 
{ 
  int tmax = (int)mImage->GetVTKImages().size() - 1;
  if(mOverlay)
    tmax = std::max(tmax, (int)mOverlay->GetVTKImages().size()-1);
  return tmax;
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
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::SetRegisterExtent(int ext[6])
{ 
    copyExtent(ext, mRegisterExtent);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicer::GetRegisterExtent(int ext[6])
{ 
    copyExtent(mRegisterExtent, ext);
}
//----------------------------------------------------------------------------

