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
#ifndef clitkBLUTDIRCommandIterationUpdateDVF_h
#define clitkBLUTDIRCommandIterationUpdateDVF_h

/* =================================================
 * @file   clitkBLUTDIRCommandIterationUpdateDVF.h
 * @author	Romulo Pinho
 * @date	16/02/2011
 *
 * @brief	ITK observer class to respond to iteration events of B-LUT DIR. This
 * observer is meant to output the DVF of the registration at each iteration.
 *
 ===================================================*/

#include "clitkBLUTDIRGenericFilter.h"

namespace clitk
{
  template <class FixedImageType, class OptimizerType, class TransformType>
  class CommandIterationUpdateDVF : public itk::Command
  {
    public:
      typedef  CommandIterationUpdateDVF   Self;
      typedef  itk::Command             Superclass;
      typedef  itk::SmartPointer<Self>  Pointer;
      itkNewMacro( Self );
    protected:
      CommandIterationUpdateDVF() : m_Iteration(0) {};
    public:

      typedef typename FixedImageType::Pointer ImagePointer;
      typedef typename TransformType::Pointer TransformPointer;

      // Set the fixed image, from the specs of the DVF are obtained.
      void SetFixedImage(ImagePointer i) { m_FixedImage = i; }

      // Set the transform
      void SetTransform(TransformPointer t){ m_Transform = t;}

      // Set command line parameters
      void SetArgsInfo(args_info_clitkBLUTDIR a){m_ArgsInfo=a;}

      // Execute
      void Execute(itk::Object *object, const itk::EventObject & event)
      {
        //Execute( (const itk::Object *)caller, event);
       if( !(itk::IterationEvent().CheckEvent( &event )) )
        {
          std::cout << "CommandIterationUpdateDVF::Execute(): check event failed!\n";
          return;
        }

        std::cout << "CommandIterationUpdateDVF::Execute()\n";

        typedef typename OptimizerType::ParametersType ParametersType;
        OptimizerType* optmizer = dynamic_cast<OptimizerType*>(object); 
        ParametersType parameters = optmizer->GetCurrentPosition();
        std::cout << "Iteration " << ++m_Iteration << endl;// << " -> Optimizer parameters: " << parameters << endl;

        //=======================================================
        // RP: Copy-paste from BLUTDIRGenericFilter::UpdateWithInputImageType()
        //=======================================================
        //=======================================================
        // Get the BSpline coefficient images and write them
        //=======================================================
	int niter = m_ArgsInfo.coeffEveryN_arg;
        if ((m_Iteration % niter == 0) && m_ArgsInfo.coeff_given)
        {
          typedef typename TransformType::CoefficientImageType CoefficientImageType;
          std::vector<typename CoefficientImageType::Pointer> coefficientImages = m_Transform->GetCoefficientImages();
          typedef itk::ImageFileWriter<CoefficientImageType> CoeffWriterType;
          typename CoeffWriterType::Pointer coeffWriter = CoeffWriterType::New();
          unsigned nLabels = m_Transform->GetnLabels();

          std::string fname(m_ArgsInfo.coeff_arg);
          int dotpos = fname.length() - 1;
          while (dotpos >= 0 && fname[dotpos] != '.')
            dotpos--;

          for (unsigned i = 0; i < nLabels; ++i)
          {
            std::ostringstream osfname;
            osfname << fname.substr(0, dotpos) << '_' << i << '_' << m_Iteration << fname.substr(dotpos);
            coeffWriter->SetInput(coefficientImages[i]);
            coeffWriter->SetFileName(osfname.str());
            coeffWriter->Update();
          }
        }
      }

      void Execute(const itk::Object * object, const itk::EventObject & event)
      {
        std::cout << "const CommandIterationUpdateDVF::Execute()\n";
      }

      unsigned int m_Iteration;
      ImagePointer m_FixedImage;
      TransformPointer m_Transform;
      args_info_clitkBLUTDIR m_ArgsInfo;
  };
}

#endif // clitkBLUTDIRCommandIterationUpdateDVF_h
