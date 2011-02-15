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
#ifndef CLITKTIMER_H
#define CLITKTIMER_H

//#include "clitkCommon.h"
#include <ctime> 
#if defined(unix) || defined(__APPLE__)
#  include <sys/time.h>
#  include <sys/resource.h>
#elif defined(WIN32)
#  include <windows.h>
#endif
#include <iostream>
#include <utility>

namespace clitk {

  class Timer {
  public:
    
    //====================================================================
    Timer();
    void Start();
    void Stop(bool accumulate=true);
    void Reset();
    void Print(std::ostream & os=std::cout) const; 
    void Print(std::string text, std::ostream & os=std::cout) const;
    //====================================================================
    
    //====================================================================
    long double GetTimeInMicroSecond()     const { return mElapsed; }
    long double GetMeanTimeInMicroSecond() const { return mElapsed/mNumberOfCall; }
    long int GetNumberOfCall()             const { return mNumberOfCall; }
    //====================================================================
    
  protected:
#if defined(unix) || defined(__APPLE__)
    rusage mBegin; 
    rusage mEnd;
#elif defined(WIN32)
    unsigned __int64 mBegin;
    unsigned __int64 mEnd;
    unsigned __int64 mFrequency;
#endif
    long double mElapsed;
    long int mNumberOfCall;
  };

} // end namespace

// #endif
#endif /* end #define CLITKTIMER_H */

