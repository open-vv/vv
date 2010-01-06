#ifndef CLITKTIMER_CXX
#define CLITKTIMER_CXX

/**
   =================================================
   * @file   clitkTimer.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   18 Jul 2007 16:27:45
   * 
   * @brief  
   * 
   * 
   =================================================*/

// #ifdef UNIX

#include "clitkTimer.h"

//====================================================================
/// Constructs the class 
clitk::Timer::Timer() { 
  Reset(); 
}
//====================================================================

//====================================================================
void clitk::Timer::Start() {
  getrusage(RUSAGE_SELF, &mBegin);
  mNumberOfCall++;
}
//====================================================================

//====================================================================
void clitk::Timer::Stop(bool accumulate) {
  getrusage(RUSAGE_SELF, &mEnd);
  if (accumulate) {
    mElapsed += (mEnd.ru_utime.tv_usec - mBegin.ru_utime.tv_usec)+
      (mEnd.ru_utime.tv_sec - mBegin.ru_utime.tv_sec)*1000000;
  }
  else {
    mNumberOfCall--;
  }
}
//====================================================================

//====================================================================
void clitk::Timer::Print(std::ostream & os) const {
  if (mNumberOfCall != 1) {
    os << "Timer #     = " << mNumberOfCall << std::endl;
    os << "Timer total = " << mElapsed << " usec \t" << mElapsed/1000000.0 << " sec." << mElapsed/1000000.0/60 << " min."
       << mElapsed/1000000.0/60/60 << " hours." << std::endl;
  }
  long double tus = mElapsed/mNumberOfCall;
  long double ts = tus/1000000.0;
  long double tm = ts/60.0;
  long double th = tm/60.0;
  os << "Timer = " << tus << " usec\t" << ts << " sec.\t" << tm << " min.\t" << th << " hours." << std::endl;
  // os << "\tmBegin.ru_utime.tv_sec = " << mBegin.ru_utime.tv_sec << std::endl;
//   os << "\tmEnd.ru_utime.tv_sec = " << mEnd.ru_utime.tv_sec << std::endl;
//   os << "\tmBegin.ru_utime.tv_usec = " << mBegin.ru_utime.tv_usec << std::endl;
//   os << "\tmEnd.ru_utime.tv_usec = " << mEnd.ru_utime.tv_usec << std::endl;  
}
//====================================================================

//====================================================================
void clitk::Timer::Print(std::string text, std::ostream & os) const {
  os << text;
  Print(os);
}  
//====================================================================

//====================================================================
void clitk::Timer::Reset() {
  mNumberOfCall = 0;
  mElapsed = 0;
}
//====================================================================

// #endif // If UNIX
#endif /* end #define CLITKTIMER_CXX */

