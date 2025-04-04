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

#ifndef CLITKCOMMON_H
#define CLITKCOMMON_H

// clitk include
#include "clitkPortability.h"
#include "clitkDD.h"
#include "clitkExceptionObject.h"

// itk include (include std)
#include <itkContinuousIndex.h>
#include <itkMacro.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

// std include
#include <fstream>

// Include for "rusage"
#include <ctime>
#if defined(unix) || defined(__APPLE__)
#  include <sys/time.h>
#  include <sys/resource.h>
#elif defined(_WIN32)
#  include <windows.h>
#  include <stdint.h>
#endif

#define VTK_EXCLUDE_STRSTREAM_HEADERS

//--------------------------------------------------------------------
namespace clitk {

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef unsigned int uint;

#define CLITK_TRY_CATCH_EXIT(func) \
  try { \
    func; \
  } \
  catch (const itk::ExceptionObject& e) { \
    e.Print(std::cout); \
    exit(-1);\
  } \
  catch (const std::exception& e) { \
    std::cout << e.what() << std::endl; \
    exit(-2);\
  } \
  catch (...) { \
    std::cout << "Unknown excpetion" << std::endl; \
    exit(-3); \
  }

#ifndef ITK_OVERRIDE
# define ITK_OVERRIDE
#endif

  //--------------------------------------------------------------------
  // when everything goes wrong
#define WHEREAMI "[ " << __FILE__  << " ] line " << __LINE__
#define FATAL(a) { std::cerr << "ERROR in " << WHEREAMI << ": " << a << std::endl; exit(0); }

  //--------------------------------------------------------------------
  // GGO with modified struct name
#define GGO(ggo_filename, args_info)                                    \
  args_info_##ggo_filename args_info;					\
  cmdline_parser_##ggo_filename##2(argc, argv, &args_info, 1, 1, 0);			\
  if (args_info.config_given)						\
    cmdline_parser_##ggo_filename##_configfile (args_info.config_arg, &args_info, 0, 0, 1); \
  else cmdline_parser_##ggo_filename(argc, argv, &args_info);

  //--------------------------------------------------------------------
  // skip line with #
  void skipComment(std::istream & is);

  //--------------------------------------------------------------------
  // linear (rough) conversion from Hounsfield Unit to density
  double HU2density(double HU);

  //--------------------------------------------------------------------
  // Return filename extension
  std::string GetExtension(const std::string& filename);

  // Return filename splitting in 1 or 2 parts : directory name (if exists) & filename
  std::vector<std::string> SplitFilename(const std::string& filename);

  //--------------------------------------------------------------------
  // Convert float, double ... to string
  template<class T> std::string toString(const T & t);
  template<class T> std::string toStringVector(const T * t, const int n);
  template<class T> std::string toStringVector(const T & t, const int n);
  template<class T> std::string toStringVector(const std::vector<T> & t);
  template <class T> bool fromString(T& t,
                                     const std::string& s,
                                     std::ios_base& (*f)(std::ios_base&)=std::dec);

  //--------------------------------------------------------------------
  // Display a progress %
  void VerboseInProgress(const int nb, const int current, const int percentage);
  void VerboseInProgressInPercentage(const int nb, const int current, const int percentage);

  //--------------------------------------------------------------------
  // Convert a pixel type to another (downcast)
  template<class TPixelUp, class TPixelDown>
  TPixelDown PixelTypeDownCast(const TPixelUp & x);
  template<>
  float PixelTypeDownCast(const double & x);
  template<>
  double PixelTypeDownCast(const double & x);

  //--------------------------------------------------------------------
  // Return the indexes of sorted values in a vector
  template<class Type> struct vectorComparisonLowerThan;
  template<class Type> struct vectorComparisonGreaterThan;
  template<class Type>
  void GetSortedIndex(const std::vector<Type> & toSort, std::vector<int> & index, bool increasing=true);

  //--------------------------------------------------------------------
  // Return the name of a type as a string
  template<class TPixel>
  std::string GetTypeAsString();

  //--------------------------------------------------------------------
  // Convert radian / degree
  double rad2deg(double anglerad);
  double deg2rad(double anglerad);

  //--------------------------------------------------------------------
  int GetTypeSizeFromString(const std::string & type);

  //--------------------------------------------------------------------
  // Special case to handle "signed_char = schar" ...
  template<class T>
  bool IsSameType(std::string t) { return (t==GetTypeAsString<T>()); }
  template<> bool IsSameType<char>(std::string t);
  template<> bool IsSameType<signed char>(std::string t);
  template<> bool IsSameType<unsigned char>(std::string t);
  template<> bool IsSameType<unsigned short>(std::string t);

  //--------------------------------------------------------------------
  template<class T1>
  std::string CreateListOfTypes(bool last=true) {
    return GetTypeAsString<T1>();
  }

  template<class T1, class T2>
  std::string CreateListOfTypes(bool last=true) {
    if (last) return CreateListOfTypes<T1>()+" and "+CreateListOfTypes<T2>();
    else return CreateListOfTypes<T1>()+", "+CreateListOfTypes<T2>();
  }

  template<class T1, class T2, class T3>
  std::string CreateListOfTypes(bool last=true) {
    if (last) return CreateListOfTypes<T1,T2>(false)+" and "+CreateListOfTypes<T3>();
    else return CreateListOfTypes<T1,T2>(false)+", "+CreateListOfTypes<T3>();
  }

  template<class T1, class T2, class T3, class T4>
  std::string CreateListOfTypes(bool last=true) {
    if (last) return CreateListOfTypes<T1,T2,T3>(false)+" and "+CreateListOfTypes<T4>();
    else return CreateListOfTypes<T1,T2,T3>(false)+", "+CreateListOfTypes<T4>();
  }

  template<class T1, class T2, class T3, class T4, class T5>
  std::string CreateListOfTypes(bool last=true) {
    if (last) return CreateListOfTypes<T1,T2,T3,T4>(false)+" and "+CreateListOfTypes<T5>();
    else return CreateListOfTypes<T1,T2,T3,T4>(false)+", "+CreateListOfTypes<T5>();
  }

  template<class T1, class T2, class T3, class T4, class T5, class T6>
  std::string CreateListOfTypes(bool last=true) {
    if (last) return CreateListOfTypes<T1,T2,T3,T4,T5>(false)+" and "+CreateListOfTypes<T6>();
    else return CreateListOfTypes<T1,T2,T3,T4,T5>(false)+", "+CreateListOfTypes<T6>();
  }

  template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  std::string CreateListOfTypes(bool last=true) {
    if (last) return CreateListOfTypes<T1,T2,T3,T4,T5,T6>(false)+" and "+CreateListOfTypes<T7>();
    else return CreateListOfTypes<T1,T2,T3,T4,T5,T6>(false)+", "+CreateListOfTypes<T7>();
  }

  template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  std::string CreateListOfTypes(bool last=true) {
    if (last) return CreateListOfTypes<T1,T2,T3,T4,T5,T6,T7>(false)+" and "+CreateListOfTypes<T8>();
    else return CreateListOfTypes<T1,T2,T3,T4,T5,T6,T7>(false)+", "+CreateListOfTypes<T8>();
  }
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  void FindAndReplace(std::string & line, const std::string & tofind, const std::string & replacement);
  void FindAndReplace(std::string & line, const std::vector<std::string> & tofind, const std::vector<std::string> & toreplace);
  void FindAndReplace(std::ifstream & in, const std::vector<std::string> & tofind, const std::vector<std::string> & toreplace, std::ofstream & out);
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  double ComputeEuclideanDistanceFromPointToPlane(const itk::ContinuousIndex<double, 3> point,
						  const itk::ContinuousIndex<double, 3> pointInPlane,
						  const itk::ContinuousIndex<double, 3> normalPlane);

  //--------------------------------------------------------------------
  // Open a File for reading/writing
  void openFileForReading(std::ifstream & is, const std::string & filename);
  void openFileForWriting(std::ofstream & os, const std::string & filename);

  //--------------------------------------------------------------------
  void readDoubleFromFile(const std::string & filename, std::vector<double> & list);

  //--------------------------------------------------------------------
  double cotan(double i);
  double invcotan(double i);

  //--------------------------------------------------------------------
  void disableStdCerr();
  void enableStdCerr();

  //--------------------------------------------------------------------
  template<class ImageType>
  void CloneImage(const typename ImageType::Pointer & input, typename ImageType::Pointer & output);

  //--------------------------------------------------------------------
  void PrintMemoryUsed();

  //--------------------------------------------------------------------
  // Convert a map to a vector
  template <typename M, typename V>
  void MapToVecFirst(const M & m, V & v);
  template <typename M, typename V>
  void MapToVecSecond(const M & m, V & v);

  //--------------------------------------------------------------------
  // Find/replace string
  template<class T>
  int inline findAndReplace(T& source, const T& find, const T& replace);

#include "clitkCommon.txx"

} // end namespace

#endif /* end #define CLITKCOMMON_H */
