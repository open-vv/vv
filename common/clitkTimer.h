#ifndef CLITKTIMER_H
#define CLITKTIMER_H
/**
   ===================================================================
   * @file   clitkTimer.h
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   18 Jul 2007 16:26:08

   * @brief  

   ===================================================================*/

#include "clitkCommon.h"
#include <ctime> 
#if defined(unix)
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
#if defined(unix)
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

