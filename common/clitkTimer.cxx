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
clitk::Timer::Timer()
{
  Reset();
#if defined(WIN32)
  QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequency);
#endif
}
//====================================================================

//====================================================================
void clitk::Timer::Start()
{
#if defined(unix) || defined(__APPLE__)
  getrusage(RUSAGE_SELF, &mBegin);
#elif defined(WIN32)
  QueryPerformanceCounter((LARGE_INTEGER*)&mBegin);
#endif
  mNumberOfCall++;
}
//====================================================================

//====================================================================
void clitk::Timer::Stop(bool accumulate)
{
#if defined(unix) || defined (__APPLE__)
  getrusage(RUSAGE_SELF, &mEnd);
  if (accumulate) {
    mElapsed += (mEnd.ru_utime.tv_usec - mBegin.ru_utime.tv_usec)+
                (mEnd.ru_utime.tv_sec - mBegin.ru_utime.tv_sec)*1000000;
  }
#elif defined(WIN32)
  QueryPerformanceCounter((LARGE_INTEGER*)&mEnd);
  if (accumulate) {
    mElapsed += ((mEnd-mBegin)*1000000)/(long double)mFrequency;
  }
#endif
  else {
    mNumberOfCall--;
  }
}
//====================================================================

//====================================================================
void clitk::Timer::Print(std::ostream & os) const
{
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
void clitk::Timer::Print(std::string text, std::ostream & os) const
{
  os << text.c_str();
  Print(os);
}
//====================================================================

//====================================================================
void clitk::Timer::Reset()
{
  mNumberOfCall = 0;
  mElapsed = 0;
}
//====================================================================

// #endif // If UNIX
#endif /* end #define CLITKTIMER_CXX */

