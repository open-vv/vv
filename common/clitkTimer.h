#ifndef CLITKTIMER_H
#define CLITKTIMER_H

/**
   ===================================================================
   * @file   clitkTimer.h
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   18 Jul 2007 16:26:08

   * @brief  

   ===================================================================*/

// #ifdef UNIX

#include "clitkCommon.h"
#include <ctime> 
#include <sys/time.h>
#include <sys/resource.h>
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
    rusage mBegin; 
    rusage mEnd;
    long double mElapsed;
    long int mNumberOfCall;
  };

} // end namespace

// #endif
#endif /* end #define CLITKTIMER_H */

