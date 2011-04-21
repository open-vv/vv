
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

// clitk
#include "clitkTestFilter_ggo.h"
#include "clitkImageCommon.h"
#include "clitkBooleanOperatorLabelImageFilter.h"
#include "clitkAutoCropFilter.h"
//#include "clitkRelativePositionConstraintLabelImageFilter.h"
#include "clitkResampleImageWithOptionsFilter.h"
#include "clitkAddRelativePositionConstraintToLabelImageFilter.h"
#include "clitkExtractLungFilter.h"
#include "clitkExtractPatientFilter.h"
#include "clitkExtractMediastinumFilter.h"
//#include "clitkTestStation7.h"
#include "clitkSegmentationUtils.h"
#include "clitkMorphoMathFilter.h"

// ITK ENST
#include "RelativePositionPropImageFilter.h"

// VTK
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPolyDataWriter.h>
#include <vtkImageData.h>
#include <vtkMetaImageWriter.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkImageStencil.h>

#include <algorithm>


template<class PointType>
class comparePointsX {
public:
  bool operator() (PointType i, PointType j) { return (i[0]<j[0]); }
};

template<int Dim>
void HypercubeCorners(std::vector<itk::Point<double, Dim> > & out) {
  std::vector<itk::Point<double, Dim-1> > previous;
  HypercubeCorners<Dim-1>(previous);
  out.resize(previous.size()*2);
  for(uint i=0; i<out.size(); i++) {
    itk::Point<double, Dim> p;
    if (i<previous.size()) p[0] = 0; 
    else p[0] = 1;
    for(int j=0; j<Dim; j++) p[j+1] = previous[i%previous.size()][j]; //NON i p
    out[i] = p;
  }
}

template<>
void HypercubeCorners<1>(std::vector<itk::Point<double, 1> > & out) {
  out.resize(2);
  out[0][0] = 0;
  out[1][0] = 1;
}


//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkTestFilter, args_info);

  // Image types
  //typedef unsigned char PixelType;
  typedef short PixelType;
  static const int Dim=3;
  typedef itk::Image<PixelType, Dim> InputImageType;

  // Read inputs
  InputImageType::Pointer input1;
  InputImageType::Pointer input2;
  InputImageType::Pointer input3;
  if (args_info.input1_given) input1 = clitk::readImage<InputImageType>(args_info.input1_arg, true);
  if (args_info.input2_given) input2 = clitk::readImage<InputImageType>(args_info.input2_arg, true);
  if (args_info.input3_given) input3 = clitk::readImage<InputImageType>(args_info.input3_arg, true);
  
  // Declare output
  InputImageType::Pointer output;

  //--------------------------------------------------------------------
  // Filter test BooleanOperatorLabelImageFilter
  if (0) {
    typedef clitk::BooleanOperatorLabelImageFilter<InputImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput1(input1);
    filter->SetInput2(input2);
    output = clitk::NewImageLike<InputImageType>(input1);
    filter->GraftOutput(output);  /// TO VERIFY !!!
    filter->Update();  
    filter->SetInput2(input3);
    filter->Update();    
    output = filter->GetOutput();
    clitk::writeImage<InputImageType>(output, args_info.output_arg);
  }
  
  //--------------------------------------------------------------------
  // Filter test AutoCropLabelImageFilter
  if (0) {
    typedef clitk::AutoCropFilter<InputImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(input1);
    filter->Update();    
    output = filter->GetOutput();
    clitk::writeImage<InputImageType>(output, args_info.output_arg);
  }

  //--------------------------------------------------------------------
  // Filter test RelativePositionPropImageFilter
  if (0) {
    typedef itk::Image<float, Dim> OutputImageType;
    OutputImageType::Pointer outputf;
    typedef itk::RelativePositionPropImageFilter<InputImageType, OutputImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(input1);

    filter->SetAlpha1(clitk::deg2rad(args_info.angle1_arg)); // xy plane
    filter->SetAlpha2(clitk::deg2rad(args_info.angle2_arg));
    filter->SetK1(M_PI/2.0); // Opening parameter, default = pi/2
    filter->SetFast(true);
    filter->SetRadius(2);
    filter->SetVerboseProgress(true);
    
    /*         A1   A2
               Left      0    0
               Right   180    0
               Ant      90    0
               Post    -90    0
               Inf       0   90
               Sup       0  -90
    */

    filter->Update();    
    clitk::writeImage<OutputImageType>(filter->GetOutput(), args_info.output_arg);
  }

  //--------------------------------------------------------------------
  // Filter test 
  if (0) {
    typedef itk::Image<short, Dim> OutputImageType;
    typedef clitk::ResampleImageWithOptionsFilter<InputImageType, OutputImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(input1); 
    filter->SetOutputIsoSpacing(1);
    //filter->SetNumberOfThreads(4); // auto
    filter->SetGaussianFilteringEnabled(false);
    filter->Update();    
    clitk::writeImage<OutputImageType>(filter->GetOutput(), args_info.output_arg);
  }

  //--------------------------------------------------------------------
  // Filter AddRelativePositionConstraintToLabelImageFilter test 
  if (0) {
    /*
    typedef clitk::AddRelativePositionConstraintToLabelImageFilter<InputImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(input1); 
    filter->SetInputObject(input2); 
    filter->SetOrientationType(FilterType::LeftTo);
    filter->SetIntermediateSpacing(5);
    filter->SetFuzzyThreshold(0.5);
    filter->VerboseStepOn();
    filter->WriteStepOff();
    filter->Update();

    filter->SetInput(filter->GetOutput()); 
    filter->SetInputObject(input2); 
    filter->SetOrientationType(FilterType::RightTo);
    filter->SetIntermediateSpacing(5);
    filter->SetFuzzyThreshold(0.5);
    filter->Update();   

    clitk::writeImage<InputImageType>(filter->GetOutput(), args_info.output_arg);
    */
  }

  //--------------------------------------------------------------------
  // Filter test ExtractPatientFilter
  if (0) {
    /*
    typedef itk::Image<char, Dim> OutputImageType;
    typedef clitk::ExtractPatientFilter<InputImageType, OutputImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(input1);
    filter->VerboseStepOn();
    filter->WriteStepOn();    
    // options (default)
    filter->SetUpperThreshold(-300);
    filter->FinalOpenCloseOff(); // default=on (not rezally needed ?)
    filter->Update();    
    OutputImageType::Pointer output = filter->GetOutput();
    clitk::writeImage<OutputImageType>(output, args_info.output_arg);
    */
  }

  //--------------------------------------------------------------------
  // Filter test ExtractLungsFilter
  if (0) {
    /*
    typedef itk::Image<PixelType, Dim> OutputImageType; // to change into char
    typedef clitk::ExtractLungFilter<InputImageType, OutputImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    // DD(filter->GetNumberOfSteps());
    filter->SetInput(input1); // CT
    filter->SetInputPatientMask(input2, 0); // Patient mask and BG value
    filter->VerboseStepOn();
    filter->WriteStepOn();    
    // options (default)
    //filter->SetBackgroundValue(0);
    filter->SetUpperThreshold(-300);
    // filter->SetMinimumComponentSize(100);

    filter->Update();    
    OutputImageType::Pointer output = filter->GetOutput();
    clitk::writeImage<OutputImageType>(output, args_info.output_arg);
    */
  }

  //--------------------------------------------------------------------
  // Filter test ExtractMediastinumFilter
  if (0) {
    /*
    typedef clitk::ExtractMediastinumFilter<InputImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInputPatientLabelImage(input1);
    filter->SetInputLungLabelImage(input2, 0, 1, 2); // BG, FG Left Lung, FG Right Lung
    filter->SetInputBonesLabelImage(input3);
    filter->VerboseStepOn();
    filter->WriteStepOn();    
    filter->Update();    
    output = filter->GetOutput();
    clitk::writeImage<InputImageType>(output, args_info.output_arg);
    */
  }

  //--------------------------------------------------------------------
  // Test for auto register sub-task in a segmentation process
  if (0) {
    // ExtractLymphStation_7 * s7 = new ExtractLymphStation_7;
    //    s7->SetArgsInfo<args_info_clitkTestFilter>(args_info);
    // GetParent->SetArgsInfo<>
    //s7->StartSegmentation();
  }

  //--------------------------------------------------------------------
  // Test for biinary image from a contour set
  if (0) {
    DD("here");

    // Type of a slice
    typedef itk::Image<InputImageType::PixelType, InputImageType::ImageDimension-1> SliceType;
    
    // Build the list of slices
    std::vector<SliceType::Pointer> slices;
    clitk::ExtractSlices<InputImageType>(input1, 2, slices);
    DD(slices.size());

    // HOW TO DO SEVERAL BY SLICE !!! not a map ?

    // Compute centroids 3D centroids by slices
    int BG = 0;
    std::map<int, std::vector<InputImageType::PointType> > centroids3D;    
    for(uint i=0; i<slices.size(); i++) {
      // Labelize
      slices[i] = clitk::Labelize<SliceType>(slices[i], BG, true, 1);
      // ComputeCentroids
      std::vector<SliceType::PointType> temp;
      clitk::ComputeCentroids<SliceType>(slices[i], BG, temp);
      for(uint j=1; j<temp.size(); j++) {
        InputImageType::PointType a;
        clitk::PointsUtils<InputImageType>::Convert2DTo3D(temp[j], input1, i, a);
        centroids3D[i].push_back(a);
      }
    }

    // REPRENDRE POUR TOUT FAIRE BY SLICE (pas de i);
    
    // Take a given slice i=29
    int index=29;
    SliceType::Pointer slice = slices[index];
    std::vector<InputImageType::PointType> points = centroids3D[index];
    
    // Sort points from R to L
    std::sort(points.begin(), points.end(), 
              comparePointsX<InputImageType::PointType>());
    
    // Slice corners (quel sens ?)

    // Compute min and max coordinates
    const uint dim=3;
    typedef InputImageType::PointType PointType;
    typedef InputImageType::IndexType IndexType;
    PointType min_c, max_c;
    IndexType min_i, max_i;
    min_i = input1->GetLargestPossibleRegion().GetIndex();
    for(uint i=0; i<dim; i++) max_i[i] = input1->GetLargestPossibleRegion().GetSize()[i] + min_i[i];
    input1->TransformIndexToPhysicalPoint(min_i, min_c);
    input1->TransformIndexToPhysicalPoint(max_i, max_c);

    // Compute the corners coordinates
    std::vector<itk::Point<double, dim> > l;
    HypercubeCorners<dim>(l);
    for(uint i=0; i<l.size(); i++) {
      for(uint j=0; j<dim; j++) {
        if (l[i][j] == 0) l[i][j] = min_c[j];
        if (l[i][j] == 1) l[i][j] = max_c[j];
      }
    }
    DDV(l, 8);


    // Add first/last point, horizontally to the image boundaries

    double sz = points[0][2]; // slice Z
    double margin = 2; // needed (if not forget to remove the first line)
    // Corners 1
    InputImageType::PointType p = min_c;
    p[0] -= margin; // margins
    p[1] -= margin; // margins
    p[2] = sz;
    points.insert(points.begin(), p);
    // vertical p
    p = points[0];
    p[0] = min_c[0] - margin; //margin
    points.insert(points.begin(), p);
    // last H point
    p = points.back();
    p[0] = max_c[0];
    points.push_back(p);
    // last corners
    p[0] = max_c[0];
    p[1] = min_c[1]-margin;
    p[2] = sz;
    points.push_back(p);
    // Same first point
    p = points[0];
    points.push_back(p);

    DDV(points, points.size());

    // create a contour, polydata. 
    vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();
    mesh->Allocate(); //for cell structures
    mesh->SetPoints(vtkPoints::New());
    vtkIdType ids[2];
    int point_number = points.size();
    for (unsigned int i=0; i<points.size(); i++) {
      mesh->GetPoints()->InsertNextPoint(points[i][0],points[i][1],points[i][2]);
      ids[0]=i;
      ids[1]=(ids[0]+1)%point_number; //0-1,1-2,...,n-1-0
      mesh->GetLines()->InsertNextCell(2,ids);
    }

    vtkSmartPointer<vtkPolyDataWriter> w = vtkSmartPointer<vtkPolyDataWriter>::New();
    w->SetInput(mesh);
    w->SetFileName("bidon.vtk");
    w->Write();    

    // binarize
    DD("binarize");
    double *bounds=mesh->GetBounds();
    DDV(bounds, 6);

    DD("create image");
    vtkSmartPointer<vtkImageData> binary_image=vtkSmartPointer<vtkImageData>::New();
    binary_image->SetScalarTypeToUnsignedChar();
    ///Use the smallest mask in which the mesh fits
    // Add two voxels on each side to make sure the mesh fits
    //    double * samp_origin=
    InputImageType::PointType samp_origin = input1->GetOrigin();
    //    double * 
    InputImageType::SpacingType spacing=input1->GetSpacing();
    double * spacing2 = new double[3];
    spacing2[0] = spacing[0];
    spacing2[1] = spacing[1];
    spacing2[2] = spacing[2];
    DD(spacing2[0]);
    binary_image->SetSpacing(spacing2);
    /// Put the origin on a voxel to avoid small skips
    DD(floor((bounds[0]-samp_origin[0])/spacing[0]-2)*spacing[0]);
    DD(bounds[0]);
    DD(samp_origin[0]);
    DD(spacing[0]);
    binary_image->SetOrigin(//samp_origin[0], samp_origin[1], samp_origin[2]);
                            floor((bounds[0]-samp_origin[0])/spacing[0]-2)*spacing[0]+samp_origin[0],
                            floor((bounds[2]-samp_origin[1])/spacing[1]-2)*spacing[1]+samp_origin[1],
                            floor((bounds[4]-samp_origin[2])/spacing[2]-2)*spacing[2]+samp_origin[2]);
    double * origin=binary_image->GetOrigin();
    binary_image->SetExtent(0,ceil((bounds[1]-origin[0])/spacing[0]+4), // Joel used +4 here (?)
                            0,ceil((bounds[3]-origin[1])/spacing[1]+4),
                            0,ceil((bounds[5]-origin[2])/spacing[2]+4));
    binary_image->AllocateScalars();
    memset(binary_image->GetScalarPointer(),0,
           binary_image->GetDimensions()[0]*binary_image->GetDimensions()[1]*binary_image->GetDimensions()[2]*sizeof(unsigned char));

    vtkSmartPointer<vtkPolyDataToImageStencil> sts=vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    //The following line is extremely important
    //http://www.nabble.com/Bug-in-vtkPolyDataToImageStencil--td23368312.html#a23370933
    sts->SetTolerance(0);
    sts->SetInformationInput(binary_image);
    
    bool extrude=true;
    if (extrude) {
      vtkSmartPointer<vtkLinearExtrusionFilter> extrude=vtkSmartPointer<vtkLinearExtrusionFilter>::New();
      extrude->SetInput(mesh);
      
      /// NO ????We extrude in the -slice_spacing direction to respect the FOCAL convention ???

      extrude->SetVector(0, 0, input1->GetSpacing()[2]);//slice_spacing); // 2* ? yes use a single one
      sts->SetInput(extrude->GetOutput());
    } else
      sts->SetInput(mesh);

    DD("stencil");
    vtkSmartPointer<vtkImageStencil> stencil=vtkSmartPointer<vtkImageStencil>::New();
    stencil->SetStencil(sts->GetOutput());
    stencil->SetInput(binary_image);
    stencil->Update();    
    
    DD("write");
    vtkSmartPointer<vtkMetaImageWriter> ww = vtkSmartPointer<vtkMetaImageWriter>::New();
    ww->SetInput(stencil->GetOutput());
    ww->SetFileName("binary.mhd");
    ww->Write();
  }
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  // Test for vessels ReconstructionByDilatation
  if (1) {
    // Read input CT (already croped)
    // treshold 3D
    // erode n times (or in 2D ?)
    // slices extract
    // SBS
    //    - CCL (keep mask)
    //    - for all CCL, ReconstructionByDilatation in initial mask
    // joint for output

    // input1
    DD("binarize")
    int BG = 0;
    int FG = 1;
    typedef itk::Image<unsigned char, InputImageType::ImageDimension> MaskImageType;
    typedef itk::BinaryThresholdImageFilter<InputImageType, MaskImageType> BinarizeFilterType; 
    BinarizeFilterType::Pointer binarizeFilter = BinarizeFilterType::New();
    binarizeFilter->SetInput(input1);
    binarizeFilter->SetLowerThreshold(150);
    binarizeFilter->SetInsideValue(FG);
    binarizeFilter->SetOutsideValue(BG);
    binarizeFilter->Update();
    MaskImageType::Pointer mask = binarizeFilter->GetOutput();
    clitk::writeImage<MaskImageType>(mask, "m.mhd");
    
    // Extract slices
    typedef itk::Image<MaskImageType::PixelType, MaskImageType::ImageDimension-1> SliceType;
    std::vector<SliceType> slices_mask;
    clitk::ExtractSlices<MaskImageType>(mask, 2, slices_mask);
    DD(slices_mask.size());

    std::vector<SliceType> debug_eroded;
    std::vector<SliceType> debug_labeled;
    
    // Loop
    for(uint i=0; i<slices_mask.size(); i++) {
      DD(i);
      // erode
      DD("erosion");
      clitk::MorphoMathFilter<SliceType>::Pointer f = clitk::MorphoMathFilter<SliceType>::New();
      f->SetInput(slices_mask[i]);
      f->SetBackgroundValue(BG);
      f->SetForegroundValue(FG);
      f->SetRadius(1);
      f->SetOperationType(0); // Erode
      f->Update();
      SliceType::Pointer eroded = f->GetOutput();
      debug_eroded.push_back(eroded);
      
      // CCL
      DD("CCL");
      SliceType::Pointer labeled = Labelize<MaskSliceType>(slices_mask[i], 0, false, 10);
      debug_labeled.push_back(labeled);
      
      // ReconstructionByDilatation 
      
      
    } // end loop

    MaskImageType::Pointer eroded = clitk::JoinSlices<MaskImageType>(debug_eroded, mask, 2);
    clitk::writeImage<MaskImageType>(eroded, "eroded.mhd");

    MaskImageType::Pointer labeled = clitk::JoinSlices<MaskImageType>(debug_labeled, mask, 2);
    clitk::writeImage<MaskImageType>(labeled, "labeled.mhd");

  }
  //--------------------------------------------------------------------

  // This is the end my friend
  return EXIT_SUCCESS;
}// end main
//--------------------------------------------------------------------
