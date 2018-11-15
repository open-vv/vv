/*=========================================================================
 *
 *  COPY OF itkPasteImageFilter to remove VerifyInputInformation
 *
 *=========================================================================*/

#ifndef __clitkPasteImageFilter_hxx
#define __clitkPasteImageFilter_hxx

#include "itkPasteImageFilter.h"

namespace clitk
{
  template< class TInputImage, class TSourceImage, class TOutputImage >
  PasteImageFilter< TInputImage, TSourceImage, TOutputImage >
  ::PasteImageFilter()
  {
    //this->ProcessObject::SetNumberOfRequiredInputs(2);

    this->InPlaceOff();
    this->m_DestinationIndex.Fill(0);
  }

} // end namespace clitk

#endif
