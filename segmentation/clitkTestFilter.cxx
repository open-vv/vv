
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

// ITK ENST
#include "RelativePositionPropImageFilter.h"

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
  input1 = clitk::readImage<InputImageType>(args_info.input1_arg, true);
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
  if (1) {
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
  }

  //--------------------------------------------------------------------
  // Filter test ExtractLungsFilter
  if (0) {
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
  }

  //--------------------------------------------------------------------
  // Filter test ExtractMediastinumFilter
  if (0) {
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
  }

  // This is the end my friend
  return EXIT_SUCCESS;
}// end main
//--------------------------------------------------------------------
