#ifndef CLITKDICOMWAVE2TEXT_H
#define CLITKDICOMWAVE2TEXT_H

#include "clitkCommon.h"
//gdcm include
#include "gdcmUtil.h"
#include "gdcmFile.h"
#include "gdcmValEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"
#include "gdcmSerieHelper.h"


namespace clitk {

  //---------------------------------------------------------------------
  class DicomWave2Text
  {

      public:
      //constructor;
      DicomWave2Text();
      //destructor;
      ~DicomWave2Text();

  };
  
} // end namespace

#endif
