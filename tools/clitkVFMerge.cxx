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
#ifndef CLITKVFMERGE_CXX
#define CLITKVFMERGE_CXX

/**
 * @file   clitkVFMerge.cxx
 * @author Jef Vandemeulebroucke <jefvdmb@gmail.com>
 * @date   June 15  10:14:53 2007
 *
 * @brief  Read in one VF (ex mhd, vf)  and write to another.  Transforming from mm to voxels needed for the vf format is implemented in clitkVfImageIO.cxx .
 *
 *
 */

// clitk include
#include "clitkVFMerge_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"

// itk include
#include "itkImageFileWriter.h"
#include <iostream>
#include "itkImageFileReader.h"

int main( int argc, char *argv[] )
{

  // Init command line
  GGO(clitkVFMerge, args_info);
  CLITK_INIT;

  const unsigned int SpaceDimension = 3;
  const unsigned int ModelDimension = 4;
  typedef itk::Vector< float, SpaceDimension > Displacement;
  typedef itk::Image< Displacement, SpaceDimension > DeformationFieldType;
  typedef itk::Image< Displacement, ModelDimension > DynamicDeformationFieldType;
  typedef itk::ImageFileReader< DeformationFieldType > DeformationFieldReaderType;
  typedef itk::ImageFileWriter< DynamicDeformationFieldType > DynamicDeformationFieldWriterType;


//declare the dynamic
  DynamicDeformationFieldType::Pointer  dynamicDeformationField=DynamicDeformationFieldType::New();


//declare their iterators
  typedef itk::ImageRegionIterator< DynamicDeformationFieldType> DynamicDeformationFieldIteratorType;
  DynamicDeformationFieldIteratorType *dynamicIteratorPointer= new DynamicDeformationFieldIteratorType;

  for (unsigned int i=0 ; i< args_info.inputs_num ; i ++) {
    //read in the deformation field i
    DeformationFieldReaderType::Pointer deformationFieldReader = DeformationFieldReaderType::New();
    deformationFieldReader->SetFileName( args_info.inputs[i]);
    if (args_info.verbose_flag) std::cout<<"Reading VF number "<< i+1 << std::endl;
    deformationFieldReader->Update();
    DeformationFieldType::Pointer currentDeformationField = deformationFieldReader->GetOutput();

    //create an iterator for the current deformation field
    typedef itk::ImageRegionIterator<DeformationFieldType> FieldIteratorType;
    FieldIteratorType fieldIterator(currentDeformationField, currentDeformationField->GetLargestPossibleRegion());

    //Allocate memory for the dynamic components
    if (i==0) {
      DynamicDeformationFieldType::RegionType dynamicDeformationFieldRegion;
      DynamicDeformationFieldType::RegionType::SizeType dynamicDeformationFieldSize;
      DeformationFieldType::RegionType::SizeType deformationFieldSize;
      deformationFieldSize= currentDeformationField->GetLargestPossibleRegion().GetSize();
      dynamicDeformationFieldSize[0]=deformationFieldSize[0];
      dynamicDeformationFieldSize[1]=deformationFieldSize[1];
      dynamicDeformationFieldSize[2]=deformationFieldSize[2];
      dynamicDeformationFieldSize[3]=args_info.inputs_num;
      dynamicDeformationFieldRegion.SetSize(dynamicDeformationFieldSize);
      DynamicDeformationFieldType::IndexType start;
      start.Fill(0);
      dynamicDeformationFieldRegion.SetIndex(start);
      dynamicDeformationField->SetRegions(dynamicDeformationFieldRegion);
      dynamicDeformationField->Allocate();


      //Set the spacing
      DeformationFieldType::SpacingType spacing= currentDeformationField->GetSpacing();
      DynamicDeformationFieldType::SpacingType dynamicSpacing;
      dynamicSpacing[0]=spacing[0];
      dynamicSpacing[1]=spacing[1];
      dynamicSpacing[2]=spacing[2];
      dynamicSpacing[3]=args_info.spacing_arg; //JV par exemple
      dynamicDeformationField->SetSpacing(dynamicSpacing);
      DynamicDeformationFieldType::PointType origin;
      origin[0]=args_info.xorigin_arg;
      origin[1]=args_info.yorigin_arg;
      origin[2]=args_info.zorigin_arg;
      origin[3]=0; //temporal origin is always 0
      dynamicDeformationField->SetOrigin(origin);

      // Creating iterators for the currentDeformationField and the DynamicDeformationField
      DynamicDeformationFieldIteratorType *dynamicIterator= new DynamicDeformationFieldIteratorType(dynamicDeformationField, dynamicDeformationField->GetLargestPossibleRegion());
      dynamicIteratorPointer=dynamicIterator;
      dynamicIteratorPointer->GoToBegin();
    }
    if (args_info.verbose_flag) std::cout<<"Merging VF number "<< i+1 << std::endl;
    //Copy the current component of the input into dynamicDeformationFieldComponent
    fieldIterator.GoToBegin();
    while(!fieldIterator.IsAtEnd()) {
      dynamicIteratorPointer->Set(fieldIterator.Get());
      ++fieldIterator;
      ++(*dynamicIteratorPointer);
    }
  }


//Write the vector field
  DynamicDeformationFieldWriterType::Pointer writer = DynamicDeformationFieldWriterType::New();
  writer->SetInput( dynamicDeformationField );
  writer->SetFileName( args_info.output_arg );
  if (args_info.verbose_flag) std::cout<<"Writing the dynamic VF"<< std::endl;


  try {

    writer->Update( );
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Problem writing the output file" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
#endif


