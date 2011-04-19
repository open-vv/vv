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
===========================================================================*/
#ifndef clitkDecomposeAndReconstructImageFilter_txx
#define clitkDecomposeAndReconstructImageFilter_txx

/* =================================================
 * @file   clitkDecomposeAndReconstructImageFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<class InputImageType, class OutputImageType>
  DecomposeAndReconstructImageFilter<InputImageType, OutputImageType>::DecomposeAndReconstructImageFilter()
  {
   m_Verbose=false;
   for (unsigned int i=0; i<InputImageDimension; i++)
     m_Radius[i]=1;
   m_NumberOfNewLabels=1;
   m_FullyConnected=true;
   m_BackgroundValue=0;
   m_ForegroundValue=1;
   m_MaximumNumberOfLabels=10;
   m_MinimumObjectSize=10;
   m_MinimumNumberOfIterations=1;

  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <class InputImageType, class  OutputImageType> 
  void 
  DecomposeAndReconstructImageFilter<InputImageType, OutputImageType>::GenerateData()
  {


    // Internal type
    typedef itk::Image<InternalPixelType, InputImageDimension> InternalImageType;

    // Filters used
    typedef clitk::DecomposeThroughErosionImageFilter<InputImageType, InternalImageType> DecomposeThroughErosionImageFilterType;
    typedef clitk::ReconstructThroughDilationImageFilter<InternalImageType, InputImageType> ReconstructThroughDilationImageFilterType;

    // Erode 
    typename DecomposeThroughErosionImageFilterType::Pointer erosionFilter=DecomposeThroughErosionImageFilterType::New();
    erosionFilter->SetInput(this->GetInput());
    erosionFilter->SetVerbose(m_Verbose);
    erosionFilter->SetFullyConnected(m_FullyConnected);
    erosionFilter->SetRadius(m_Radius);
    erosionFilter->SetNumberOfNewLabels(m_NumberOfNewLabels);
    erosionFilter->SetMinimumObjectSize(m_MinimumObjectSize);
    erosionFilter->SetMinimumNumberOfIterations(m_MinimumNumberOfIterations);
    erosionFilter->Update();

    // Reconstruct
    typename ReconstructThroughDilationImageFilterType::Pointer reconstructionFilter =ReconstructThroughDilationImageFilterType::New();
    reconstructionFilter->SetInput(erosionFilter->GetOutput());
    reconstructionFilter->SetVerbose(m_Verbose);
    reconstructionFilter->SetRadius(m_Radius);
    reconstructionFilter->SetMaximumNumberOfLabels(m_MaximumNumberOfLabels);
    reconstructionFilter->SetBackgroundValue(m_BackgroundValue);
    reconstructionFilter->SetForegroundValue(m_ForegroundValue);
    reconstructionFilter->Update();
    
    // Output
    this->SetNthOutput(0,reconstructionFilter->GetOutput());
  }


}//end clitk
 
#endif //#define clitkDecomposeAndReconstructImageFilter_txx
