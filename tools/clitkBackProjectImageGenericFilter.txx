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
#ifndef CLITKBACKPROJECTIMAGEGENERICFILTER_TXX
#define CLITKBACKPROJECTIMAGEGENERICFILTER_TXX
/**
   =================================================
   * @file   clitkBackProjectImageGenericFilter.txx
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   30 April 2008
   * 
   * @brief Project a 3D image using a cone-beam geometry
   * 
   =================================================*/


namespace clitk
{

  //================================================================================
  template <class  InputPixelType> void BackProjectImageGenericFilter::UpdateWithPixelType()
  {

    //---------------------------------
    // Define the images
    //---------------------------------
    const unsigned int InputImageDimension=2;
    typedef itk::Image<InputPixelType, InputImageDimension> InputImageType;
    typedef itk::ImageFileReader<InputImageType> ImageReaderType;
    typename  ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input = reader->GetOutput();

    //Define the output type
    //JV always float?
    const unsigned int OutputImageDimension=3;    
    typedef float OutputPixelType;    
    typedef itk::Image<OutputPixelType, OutputImageDimension> OutputImageType;

    //Create the BackProjectImageFilter
    typedef BackProjectImageFilter<InputImageType, OutputImageType> BackProjectImageFilterType;
    typename BackProjectImageFilterType::Pointer filter=BackProjectImageFilterType::New();


    //---------------------------------
    //Pass all the necessary parameters
    //---------------------------------
    unsigned int i=0;
    filter->SetInput(input);
    if (m_ArgsInfo.threads_given) {
#if ITK_VERSION_MAJOR <= 4
      filter->SetNumberOfThreads(m_ArgsInfo.threads_arg);
#else
      filter->SetNumberOfWorkUnits(m_ArgsInfo.threads_arg);
#endif
    }

    //Projection parameters
    OutputImageType::PointType iso;
    if (m_ArgsInfo.iso_given)
    {
      for(i=0;i<OutputImageDimension;i++)
        iso[i]=m_ArgsInfo.iso_arg[i];
      filter->SetIsoCenter(iso);
    }
    if (m_ArgsInfo.panel_shift_given)
      filter->SetPanelShift(m_ArgsInfo.panel_shift_arg[0], m_ArgsInfo.panel_shift_arg[1]);
    filter->SetSourceToScreen(m_ArgsInfo.screen_arg);
    filter->SetSourceToAxis(m_ArgsInfo.axis_arg); 
    filter->SetProjectionAngle(m_ArgsInfo.angle_arg);
    if (m_ArgsInfo.matrix_given)
    {
      itk::Matrix<double,4,4> rt =ReadMatrix3D(m_ArgsInfo.matrix_arg);
            filter->SetRigidTransformMatrix(rt);
    }
    filter->SetEdgePaddingValue(static_cast<OutputPixelType>(m_ArgsInfo.pad_arg));

    //Output image info
    if (m_ArgsInfo.like_given)
    {
      typedef itk::ImageFileReader<OutputImageType> ReaderType;
      ReaderType::Pointer reader2=ReaderType::New();
      reader2->SetFileName(m_ArgsInfo.like_arg);
      reader2->Update();

      OutputImageType::Pointer image=reader2->GetOutput();
      filter->SetOutputParametersFromImage(image);
    }
    else
    {
      if(m_ArgsInfo.origin_given)
      {
        OutputImageType::PointType origin;
        for(i=0;i<OutputImageDimension;i++)
          origin[i]=m_ArgsInfo.origin_arg[i];
        filter->SetOutputOrigin(origin);
      }
      if (m_ArgsInfo.spacing_given)
      {
        OutputImageType::SpacingType spacing;
        for(i=0;i<OutputImageDimension;i++)
          spacing[i]=m_ArgsInfo.spacing_arg[i];
        filter->SetOutputSpacing(spacing);
      }
      if (m_ArgsInfo.spacing_given)
      {
        OutputImageType::SizeType size;
        for(i=0;i<OutputImageDimension;i++)
          size[i]=m_ArgsInfo.size_arg[i];
        filter->SetOutputSize(size);
      }
    }

    //Go
    filter->Update();

    //Get the output
    OutputImageType::Pointer output=filter->GetOutput();

    //Write the output
    typedef itk::ImageFileWriter<OutputImageType> OutputWriterType;
    OutputWriterType::Pointer outputWriter = OutputWriterType::New();
    outputWriter->SetInput(output);
    outputWriter->SetFileName(m_ArgsInfo.output_arg); 
    if (m_Verbose)std::cout<<"Writing back projected image..."<<std::endl;
    outputWriter->Update(); 
  }

}
 
#endif //#define CLITKBACKPROJECTIMAGEGENERICFILTER_TXX
