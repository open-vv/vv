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

#ifndef CLITKFILLMASKFILTER_TXX
#define CLITKFILLMASKFILTER_TXX

//--------------------------------------------------------------------
template <class ImageType>
clitk::FillMaskFilter<ImageType>::FillMaskFilter():
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  // Default global options
  this->SetNumberOfRequiredInputs(1);
  ResetDirections();
  AddDirection(2);
  AddDirection(1);
  AddDirection(0);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::FillMaskFilter<ImageType>::
AddDirection(int dir)
{
  m_Directions.push_back(dir);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::FillMaskFilter<ImageType>::
ResetDirections()
{
  m_Directions.clear();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
template<class ArgsInfoType>
void 
clitk::FillMaskFilter<ImageType>::
SetOptionsFromArgsInfo(ArgsInfoType & mArgsInfo)
{
  ResetDirections();
  for(uint i=0; i<mArgsInfo.dir_given; i++) {
    AddDirection(mArgsInfo.dir_arg[i]);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
template<class ArgsInfoType>
void 
clitk::FillMaskFilter<ImageType>::
SetOptionsToArgsInfo(ArgsInfoType & mArgsInfo)
{
  mArgsInfo.dir_arg = new int[m_Directions.size()];
  mArgsInfo.dir_given = m_Directions.size();
  for(uint i=0; i<mArgsInfo.dir_given; i++) {
    mArgsInfo.dir_arg[i] = m_Directions[i];
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::FillMaskFilter<ImageType>::
GenerateData() 
{
  InputImagePointer input  = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));

  // Cast 
  typedef int InternalPixelType;
  typedef itk::Image<InternalPixelType, ImageType::ImageDimension> InternalImageType;
  typedef itk::CastImageFilter<ImageType,InternalImageType> InputCastImageFilterType;
  typename InputCastImageFilterType::Pointer inputCaster = InputCastImageFilterType::New();
  inputCaster->SetInput(input);
  inputCaster->Update();

  //---------------------------------------- 
  // Loop over directions    
  //---------------------------------------- 
  typename InternalImageType::Pointer output = inputCaster->GetOutput();
  for (unsigned int i=0; i<m_Directions.size();i++) {
    DD(i);
    //---------------------------------------- 
    // Fill the holes of a mask in 2D
    //----------------------------------------
        
    // We define the region to be extracted.
    typedef  itk::Image<InternalPixelType, ImageType::ImageDimension-1> ImageSliceType;
    typedef  itk::Image<InternalPixelType, ImageType::ImageDimension-1> MaskSliceType;
    typename InternalImageType::RegionType region3D = input->GetLargestPossibleRegion();
    typename InternalImageType::RegionType::SizeType size3D = region3D.GetSize();
    typename InternalImageType::RegionType::SizeType size2D = size3D;
    size2D[m_Directions[i]]=0;
    typename InternalImageType::IndexType start2D; 
    start2D.Fill(0);
    typename InternalImageType::RegionType desiredRegion;
    desiredRegion.SetSize( size2D );
    desiredRegion.SetIndex( start2D );
    
    // Extract and Join 
    typedef itk::ExtractImageFilter<InternalImageType, ImageSliceType> ExtractImageFilterType;
    typedef itk::JoinSeriesImageFilter<ImageSliceType, InternalImageType> JoinSeriesFilterType;
    typename JoinSeriesFilterType::Pointer joinFilter=JoinSeriesFilterType::New();
    joinFilter->SetSpacing(input->GetSpacing()[m_Directions[i]]);
      
    //---------------------------------------- 
    // Run over the sliceIndexs
    // ----------------------------------------
    for(unsigned int sliceIndex=0; sliceIndex <size3D[m_Directions[i]]; sliceIndex++)
      {
        //---------------------------------------- 
        // Extract mask sliceIndex
        //----------------------------------------
        typename ExtractImageFilterType::Pointer extractFilter=ExtractImageFilterType::New();
        extractFilter->SetInput(output);
        start2D[m_Directions[i]]=sliceIndex;
        desiredRegion.SetIndex( start2D );
        extractFilter->SetExtractionRegion( desiredRegion );
        extractFilter->Update( );
        typename ImageSliceType::Pointer slice= extractFilter->GetOutput();
        
        // Binarize the image (Before: OBJECT!=0, rest=0, After: object=1, rest=0 )
        typedef itk::BinaryThresholdImageFilter<ImageSliceType,ImageSliceType> BinarizeFilterType;
        typename BinarizeFilterType::Pointer binarizeFilter=BinarizeFilterType::New();
        binarizeFilter->SetInput(slice);
        binarizeFilter->SetUpperThreshold(0);
        binarizeFilter->SetOutsideValue(0);
        binarizeFilter->SetInsideValue(1);
        // writeImage<ImageSliceType>(binarizeFilter->GetOutput(),"/home/jef/tmp/input.mhd");
        
        // Perform connected labelling on the slice (body+air=0 )
        typedef itk::ConnectedComponentImageFilter<ImageSliceType, ImageSliceType> ConnectFilterType;
        typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
        connectFilter->SetInput(binarizeFilter->GetOutput());
        connectFilter->SetBackgroundValue(0);
        connectFilter->SetFullyConnected(false);
        //connectFilter->Update();
        //writeImage<ImageSliceType>(connectFilter->GetOutput(),"/home/jef/tmp/connect.mhd");
	
        // Sort the labels
        typedef itk::RelabelComponentImageFilter<ImageSliceType, ImageSliceType> RelabelFilterType;
        typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
        relabelFilter->SetInput(connectFilter->GetOutput());
        //relabelFilter->Update();
        //writeImage<ImageSliceType>(relabelFilter->GetOutput(),"/home/jef/tmp/label.mhd");	
	
        // Keep the first
        typedef itk::ThresholdImageFilter<ImageSliceType> ThresholdFilterType;
        typename ThresholdFilterType::Pointer thresholdFilter=ThresholdFilterType::New();
        thresholdFilter->SetInput(relabelFilter->GetOutput());
        thresholdFilter->SetUpper(1);
        thresholdFilter->SetOutsideValue(0);
        // thresholdFilter->Update();
        // writeImage<ImageSliceType>(thresholdFilter->GetOutput(),"/home/jef/tmp/bin.mhd");	
        
        // Invert the labels (lung 1, rest 0)
        typename BinarizeFilterType::Pointer switchFilter=BinarizeFilterType::New();
        switchFilter->SetInput(thresholdFilter->GetOutput());
        switchFilter->SetUpperThreshold(0);
        switchFilter->SetOutsideValue(0);
        switchFilter->SetInsideValue(1);
        switchFilter->Update();
        //writeImage<ImageSliceType>(switchFilter->GetOutput(),"/home/jef/tmp/inv_bin.mhd");	
	
        //Join
        joinFilter->SetInput( sliceIndex, switchFilter->GetOutput());
      }
    
    // Join to a 3D image	
    std::cout<<"Joining the slices..."<<std::endl;
    joinFilter->Update();
    
    // Permute the axes to reset to orientation
    typedef itk::PermuteAxesImageFilter<InternalImageType> PermuteFilterType;
    typename PermuteFilterType::Pointer permuteFilter=PermuteFilterType::New();
    permuteFilter->SetInput(joinFilter->GetOutput());
    typename PermuteFilterType::PermuteOrderArrayType order;
    order[m_Directions[i]]=2;
    if( m_Directions[i]==2)
      {
        order[0]=0;
        order[1]=1;
      }
    else if ( m_Directions[i]==1)
      {
        order[0]=0;
        order[2]=1;
      }
    else if (m_Directions[i]==0)
      {
        order[1]=0;
        order[2]=1;
      }
    permuteFilter->SetOrder(order);
    permuteFilter->Update();
    output = permuteFilter->GetOutput();
    
    // Set the image direction to the input one
    output->SetDirection(input->GetDirection());
    output->SetOrigin(input->GetOrigin());
  }
  
  writeImage<InternalImageType>(output, "toto.mhd");
  
  // Cast
  DD("cast");
  typedef itk::CastImageFilter<InternalImageType,ImageType> OutputCastImageFilterType;
  typename OutputCastImageFilterType::Pointer outputCaster =OutputCastImageFilterType::New();
  outputCaster->SetInput(output);
  outputCaster->Update();
  
  // Output
  DD("Graft");
  this->GraftOutput(outputCaster->GetOutput());
}
//--------------------------------------------------------------------

#endif //#define CLITKFILLMASKFILTER_TXX
