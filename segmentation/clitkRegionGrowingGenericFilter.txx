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

#ifndef CLITKREGIONGROWINGGENERICFILTER_TXX
#define CLITKREGIONGROWINGGENERICFILTER_TXX

#include <itkBinaryBallStructuringElement.h>
#include <itkConstShapedNeighborhoodIterator.h>


//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::RegionGrowingGenericFilter<ArgsInfoType>::RegionGrowingGenericFilter():
  ImageToImageGenericFilter<Self>("RegionGrowing") 
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  //InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::RegionGrowingGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar);
  ADD_IMAGE_TYPE(Dim, short);
  // ADD_IMAGE_TYPE(Dim, int);
  ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::RegionGrowingGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.input_given)   this->AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given)  this->SetOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::RegionGrowingGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  DD("UpdateWithInputImageType");
  const int Dimension = ImageType::ImageDimension;

  // ImageTypes
  typedef ImageType InputImageType;
  typedef ImageType OutputImageType;
  typedef typename ImageType::PixelType PixelType;
    
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Seed
  typedef typename  std::vector<typename InputImageType::IndexType> SeedsType;
  SeedsType seeds(1);
  if (mArgsInfo.seed_given==Dimension)
    for (unsigned int i=0; i<Dimension;i++)
      seeds[0][i]=mArgsInfo.seed_arg[i];
    
  else if ( mArgsInfo.seed_given==1)
    seeds[0].Fill(mArgsInfo.seed_arg[0]);
    
  else seeds[0].Fill(mArgsInfo.seed_arg[0]);
  if (mArgsInfo.verbose_flag) std::cout<<"Setting seed seeds to "<<seeds[0]<<"..."<<std::endl;

  if (mArgsInfo.seedRadius_given)
    {
      typedef itk::BinaryBallStructuringElement<PixelType, Dimension> BallType;
      typename BallType::RadiusType r;

      if (mArgsInfo.seedRadius_given == Dimension)
        for (unsigned i = 0; i < Dimension; i++)
          r[i] = mArgsInfo.seedRadius_arg[i];
      else
        r.Fill(mArgsInfo.seedRadius_arg[0]);

      BallType ball;
      ball.SetRadius(r);
      ball.CreateStructuringElement();

      typedef itk::ConstShapedNeighborhoodIterator<InputImageType> IteratorType;
      IteratorType it(ball.GetRadius(),
                      input,
                      input->GetLargestPossibleRegion());
#if ITK_VERSION_MAJOR < 4
      typename BallType::ConstIterator nit;
      unsigned idx = 0;
      for (nit = ball.Begin(); nit != ball.End(); ++nit, ++idx)
        {
          if (*nit)
            {
              it.ActivateOffset(it.GetOffset(idx));
            }
          else
            {
              it.DeactivateOffset(it.GetOffset(idx));
            }
        }
#else
      it.CreateActiveListFromNeighborhood(ball);
      it.NeedToUseBoundaryConditionOff();
#endif

      it.SetLocation(seeds[0]);
      for (typename IteratorType::ConstIterator i = it.Begin(); !i.IsAtEnd(); ++i)
        {
          typename InputImageType::IndexType id = seeds[0] + i.GetNeighborhoodOffset();
          if (id != seeds[0] && input->GetLargestPossibleRegion().IsInside(id))
            seeds.push_back(id);
        }
    }

  // Filter
  typedef itk::ImageToImageFilter<InputImageType, OutputImageType> ImageToImageFilterType;
  typename ImageToImageFilterType::Pointer filter;

  switch (mArgsInfo.type_arg)
    {
    case 0: {
		
      typedef itk::ConnectedThresholdImageFilter<InputImageType, OutputImageType> ImageFilterType;
      typename ImageFilterType::Pointer f= ImageFilterType::New();
	
      f->SetLower(mArgsInfo.lower_arg);
      f->SetUpper(mArgsInfo.upper_arg);
      f->SetReplaceValue(static_cast<PixelType>(mArgsInfo.pad_arg));
      for (typename SeedsType::const_iterator it = seeds.begin(); it != seeds.end(); ++it)
        f->AddSeed(*it);
      filter=f;
      if (mArgsInfo.verbose_flag) std::cout<<"Using the connected threshold image filter..."<<std::endl;

      break;
    }

    case 1: {
		
      typedef itk::NeighborhoodConnectedImageFilter<InputImageType, OutputImageType> ImageFilterType;
      typename ImageFilterType::Pointer f= ImageFilterType::New();
	
      // Radius
      typename  InputImageType::SizeType size;
      if (mArgsInfo.radius_given==Dimension)
        for (unsigned int i=0; i<Dimension;i++)
          size[i]=mArgsInfo.radius_arg[i];
	
      else if ( mArgsInfo.radius_given==1)
        size.Fill(mArgsInfo.radius_arg[0]);
	
      else size.Fill(mArgsInfo.radius_arg[0]);
      if (mArgsInfo.verbose_flag) std::cout<<"Setting neighborhood radius to "<<size<<"..."<<std::endl;

      f->SetLower(mArgsInfo.lower_arg);
      f->SetUpper(mArgsInfo.upper_arg);
      f->SetReplaceValue(static_cast<PixelType>(mArgsInfo.pad_arg));
      for (typename SeedsType::const_iterator it = seeds.begin(); it != seeds.end(); ++it)
        f->AddSeed(*it);
      f->SetRadius(size);
      filter=f;
      if (mArgsInfo.verbose_flag) std::cout<<"Using the neighborhood threshold connected image filter..."<<std::endl;

      break;
    }

    case 2: {
	
      typedef itk::ConfidenceConnectedImageFilter<InputImageType, OutputImageType> ImageFilterType;
      typename ImageFilterType::Pointer f= ImageFilterType::New();
	
      // Radius
      typename  InputImageType::SizeType size;
      if (mArgsInfo.radius_given==Dimension)
        for (unsigned int i=0; i<Dimension;i++)
          size[i]=mArgsInfo.radius_arg[i];
	
      else if ( mArgsInfo.radius_given==1)
        size.Fill(mArgsInfo.radius_arg[0]);
	
      else size.Fill(mArgsInfo.radius_arg[0]);
      if (mArgsInfo.verbose_flag) std::cout<<"Setting neighborhood radius to "<<size<<"..."<<std::endl;

      f->SetMultiplier( mArgsInfo.multiplier_arg );
      f->SetNumberOfIterations( mArgsInfo.multiplier_arg );
      for (typename SeedsType::const_iterator it = seeds.begin(); it != seeds.end(); ++it)
        f->AddSeed(*it);
      f->SetNumberOfIterations( mArgsInfo.iter_arg);
      f->SetReplaceValue(static_cast<PixelType>(mArgsInfo.pad_arg));
      f->SetInitialNeighborhoodRadius(size[0]);
      filter=f;
      if (mArgsInfo.verbose_flag) std::cout<<"Using the confidence threshold connected image filter..."<<std::endl;
	
      break;
    }

    case 3: {
		
      typedef clitk::LocallyAdaptiveThresholdConnectedImageFilter<InputImageType, OutputImageType> ImageFilterType;
      typename ImageFilterType::Pointer f= ImageFilterType::New();
	
      // Radius
      typename  InputImageType::SizeType size;
      if (mArgsInfo.radius_given==Dimension)
        for (unsigned int i=0; i<Dimension;i++)
          size[i]=mArgsInfo.radius_arg[i];
      else size.Fill(mArgsInfo.radius_arg[0]);
      if (mArgsInfo.verbose_flag) std::cout<<"Setting neighborhood radius to "<<size<<"..."<<std::endl;

      // params
      f->SetLower(mArgsInfo.lower_arg);
      f->SetUpper(mArgsInfo.upper_arg);
      f->SetLowerBorderIsGiven(mArgsInfo.adaptLower_flag);
      f->SetLowerBorderIsGiven(mArgsInfo.adaptUpper_flag);
      f->SetReplaceValue(static_cast<PixelType>(mArgsInfo.pad_arg));
      f->SetMultiplier(mArgsInfo.multiplier_arg);
      f->SetMaximumSDIsGiven(mArgsInfo.maxSD_given);
      if (mArgsInfo.maxSD_given) f->SetMaximumSD(mArgsInfo.maxSD_arg);
      for (typename SeedsType::const_iterator it = seeds.begin(); it != seeds.end(); ++it)
        f->AddSeed(*it);
      f->SetRadius(size);
      filter=f;
      if (mArgsInfo.verbose_flag) std::cout<<"Using the locally adaptive threshold connected image filter..."<<std::endl;

      break;
    }
  
    case 4: {
		
      typedef clitk::ExplosionControlledThresholdConnectedImageFilter<InputImageType, OutputImageType> ImageFilterType;
      typename ImageFilterType::Pointer f= ImageFilterType::New();
	
      // 	// Radius
      // 	typename  InputImageType::SizeType size;
      // 	if (mArgsInfo.radius_given==Dimension)
      // 	  for (unsigned int i=0; i<Dimension;i++)
      // 	    size[i]=mArgsInfo.radius_arg[i];
      // 	else size.Fill(mArgsInfo.radius_arg[0]);
      // 	if (mArgsInfo.verbose_flag) std::cout<<"Setting neighborhood radius to "<<size<<"..."<<std::endl;
	
      // params
      f->SetVerbose(mArgsInfo.verbose_flag);
      f->SetLower(mArgsInfo.lower_arg);
      f->SetUpper(mArgsInfo.upper_arg);
      f->SetMinimumLowerThreshold(mArgsInfo.minLower_arg);
      f->SetMaximumUpperThreshold(mArgsInfo.maxUpper_arg);
      f->SetAdaptLowerBorder(mArgsInfo.adaptLower_flag);
      f->SetAdaptUpperBorder(mArgsInfo.adaptUpper_flag);
      f->SetReplaceValue(static_cast<PixelType>(mArgsInfo.pad_arg));
      f->SetMultiplier(mArgsInfo.multiplier_arg);
      f->SetThresholdStepSize(mArgsInfo.step_arg);
      f->SetMinimumThresholdStepSize(mArgsInfo.minStep_arg);
      f->SetFullyConnected(mArgsInfo.full_flag);
      for (typename SeedsType::const_iterator it = seeds.begin(); it != seeds.end(); ++it)
        f->AddSeed(*it);
      filter=f;
      if (mArgsInfo.verbose_flag) std::cout<<"Using the explosion controlled threshold connected image filter..."<<std::endl;

      break;
    }
  
    }


  filter->SetInput(input);
  filter->Update();
  typename OutputImageType::Pointer output=filter->GetOutput();

  // Write/Save results
  this->template SetNextOutput<OutputImageType>(output); 
}
//--------------------------------------------------------------------

 
#endif //#define CLITKREGIONGROWINGGENERICFILTER_TXX
