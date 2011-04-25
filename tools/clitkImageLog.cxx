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
#ifndef CLITKIMAGELOG_CXX
#define CLITKIMAGELOG_CXX
/**
 =================================================
 * @file   clitkImageLog.cxx
 * @author Jef Vandemeulebroucke <Jef@creatis.insa-lyon.fr>
 * @date   04 April 2008 15:28:32
 * 
 * @brief  
 * 
 * Take an inverse normalized log of the image intensity
 =================================================*/

// clitk include
#include "clitkImageLog_ggo.h"
#include "clitkIO.h"
#include "clitkCommon.h"

// itk include
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"

int main(int argc, char * argv[]) {

  // init command line
  GGO(clitkImageLog, args_info);
  CLITK_INIT;

  typedef float PixelType;
  const  unsigned int Dimension=3;
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;  
  typedef itk::ImageFileWriter<ImageType> ImageWriterType;  
  typedef itk::ImageRegionIterator<ImageType> IteratorType;  

  //Read input
  ImageReaderType::Pointer reader= ImageReaderType::New();
  reader->SetFileName(args_info.input_arg);
  reader->Update();
  ImageType::Pointer input = reader->GetOutput();
  
  //Create iterators
  IteratorType pi (input, input->GetLargestPossibleRegion());
  pi.GoToBegin();
  
  PixelType max=std::numeric_limits<unsigned short>::max();
  
  //Create output
  while(!pi.IsAtEnd())
    {
      if (pi.Get()< 0)
	{
	  pi.Set(0.);
	}
      else
	{
	  pi.Set(-log((PixelType)(max-pi.Get()+1)/(PixelType)max));
	}
      ++pi;
    }
  
  ImageWriterType::Pointer writer= ImageWriterType::New();
  writer->SetFileName(args_info.output_arg);
  writer->SetInput(input);
  writer->Update();
  return 0;
}



#endif /* end #define CLITKIMAGELOG_CXX */
