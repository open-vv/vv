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
#ifndef CLITKIMAGECOMMON_H
#define CLITKIMAGECOMMON_H
/**
 -------------------------------------------------------------------
 * @file   clitkImageCommon.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   07 Sep 2007 11:30:10

 * @brief  

 -------------------------------------------------------------------*/

// clitk
#include "clitkCommon.h"

// itk
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "gdcmFile.h"
#include "gdcmFileHelper.h"


namespace clitk {
  
  //--------------------------------------------------------------------
  // New Image creation (no allocation)
  template<class PixelType>
  typename itk::Image<PixelType,1>::Pointer NewImage1D(int size, double spacing=1.0);
  template<class PixelType>
  typename itk::Image<PixelType,2>::Pointer NewImage2D(int sx, int sy, double dx=1.0, double dy=1.0);
  template<class PixelType>
  typename itk::Image<PixelType,3>::Pointer NewImage3D(int sx, int sy, int sz, double dx=1.0, double dy=1.0, double dz=1.0);
  template<class PixelType>
  typename itk::Image<PixelType,4>::Pointer NewImage4D(int sx, int sy, int sz, int st, double dx=1.0, double dy=1.0, double dz=1.0, double dt=1.0);

  //--------------------------------------------------------------------
  // New Image creation (with allocation)

  //--------------------------------------------------------------------
  // Read an Write image
  // template<class ImageType>
  //   typename ImageType::Pointer ReadImage(const std::string & filename, const bool verbose=false);
  template<typename ImageType>
  typename ImageType::Pointer readImage(const std::string & filename, const bool verbose=false);  
  template<typename ImageType>
  typename ImageType::Pointer readImage(const std::vector<std::string> & filenames, const bool verbose=false);
  template<class ImageType>
  void writeImage(const typename ImageType::Pointer image, const std::string & filename, const bool verbose=false);
//   template<class ImageType>  
//   void writeConstImage(const typename ImageType::ConstPointer image, const std::string & filename, const bool verbose=false);
  template<class ImageType>  
  void writeImage(const ImageType* image, const std::string & filename, const bool verbose=false);

  //--------------------------------------------------------------------
  // Read/print image header
  itk::ImageIOBase::Pointer readImageHeader(const std::string & filename,bool exit_on_error=true);
  void printImageHeader(itk::ImageIOBase::Pointer header, std::ostream & os, const int level=0);

  //--------------------------------------------------------------------
  // Determine pixetype and dimension of an image file
  void ReadImageDimensionAndPixelType(const std::string & filename, int & dimension, std::string & pixeType);

  //--------------------------------------------------------------------
  // Determine pixetype, dimension and number of pixel components  of an image file
  void ReadImageDimensionAndPixelType(const std::string & filename, int & dimension, std::string & pixeType, int & components);

  //--------------------------------------------------------------------
  // Read a dicom header  
  gdcm::File * readDicomHeader(const std::string & filename, const bool verbose=false);

  //--------------------------------------------------------------------
  template<class ImageType>
  int ComputeHowManyDifferentIntensity(const typename ImageType::Pointer & image, 
				       std::vector<typename ImageType::PixelType> & listOfIntensities);
  template<class InputImageType, class MaskImageType>
  void ComputeWeightsOfEachClasses(const typename InputImageType::Pointer & input, 
				   const typename MaskImageType::Pointer & mask,
				   const std::vector<typename MaskImageType::PixelType> & listOfIntensities, 
				   std::map<typename MaskImageType::PixelType, 
				   std::map<typename InputImageType::PixelType, double> > & mapOfLabelsAndWeights);

#include "clitkImageCommon.txx"

} // end namespace

#endif /* end #define CLITKIMAGECOMMON_H */

