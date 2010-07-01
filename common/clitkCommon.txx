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
#ifndef CLITKCOMMON_TXX
#define CLITKCOMMON_TXX
/**
   -------------------------------------------------
   * @file   clitkCommon.txx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   18 May 2006
   *
   -------------------------------------------------*/

//-------------------------------------------------------
// Utility functions for text file parsing (author: joel schaerer)

template<class ElementType>
ElementType parse_value(std::string str)
{
  std::istringstream parser(str);
  ElementType value;
  parser >> value;
  assert(!parser.fail());
  return value;
}

template<class ElementType>
std::vector<ElementType> parse_string(std::string str,char delim)
{
  std::istringstream ss(str);
  std::string token;
  std::vector<ElementType> result;
  while (std::getline(ss,token,delim)) {
    result.push_back(parse_value<ElementType>(token));
  }
  return result;
}

template<class ElementType>
std::vector<std::vector<ElementType> > parse_file(const char* filename,char delim)
{
  std::ifstream fs(filename);
  std::string line;
  std::vector<std::vector<ElementType> > result;
  while (std::getline(fs,line)) {
    if (line[0] != '#') //skip comments
      result.push_back(parse_string<ElementType>(line,delim));
  }
  return result;
}

//--------------------------------------------------------------------
// Convert float, double ... to string
template<class T>
std::string toString(const T & t)
{
  std::ostringstream myStream;
  myStream << t << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Convert float*, double* ... to string
template<class T>
std::string toStringVector(const T * t, const int n)
{
  std::ostringstream myStream;
  for(int i=0; i<n-1; i++)
    myStream << clitk::toString<T>(t[i]) << " ";
  myStream << clitk::toString<T>(t[n-1]) << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Convert float*, double* ... to string
template<class T>
std::string toStringVector(const T & t, const int n)
{
  std::ostringstream myStream;
  for(int i=0; i<n-1; i++)
    myStream << t[i] << " ";
  myStream << t[n-1] << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Convert float*, double* ... to string
template<class T>
std::string toStringVector(const std::vector<T> & t)
{
  return toStringVector(&t[0], t.size());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Convert a pixel type to another (downcast)
template<class TPixelUp, class TPixelDown>
TPixelDown PixelTypeDownCast(const TPixelUp & x)
{
  return (TPixelDown)lrint(x);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class Type>
struct vectorComparisonLowerThan: public std::binary_function<int, int, bool> {
  vectorComparisonLowerThan(const std::vector<Type> & v):vect(v) {};
  bool operator()(int x, int y) {
    return (vect[x] < vect[y]);
  }
  const std::vector<Type> & vect;
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class Type>
struct vectorComparisonGreaterThan: public std::binary_function<int, int, bool> {
  vectorComparisonGreaterThan(const std::vector<Type> & v):vect(v) {};
  bool operator()(int x, int y) {
    return (vect[x] > vect[y]);
  }
  const std::vector<Type> & vect;
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class Type>
void GetSortedIndex(const std::vector<Type> & toSort, std::vector<int> & index, bool increasing)
{
  index.resize(toSort.size());
  for(unsigned int i=0; i<index.size(); i++) index[i] = i;
  if (increasing)
    std::sort(index.begin(),
              index.end(),
              vectorComparisonLowerThan<double>(toSort));
  else
    std::sort(index.begin(),
              index.end(),
              vectorComparisonGreaterThan<double>(toSort));
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class TPixel>
std::string GetTypeAsString()
{
  //  http://www.vtk.org/doc/release/3/html/vtkSetGet_8h-source.html
  // and
  // itkImageIOBase.cxx
  const std::type_info & PixType = typeid(TPixel);
  std::string pixelName;
  if (PixType == typeid(char)) pixelName = "char"; // 'plain" char is different from signed char and unsigned char ...
  else if (PixType == typeid(signed char)) pixelName = "signed_char";
  else if (PixType == typeid(unsigned char)) pixelName = "unsigned_char";
  else if (PixType == typeid(short)) pixelName = "short";
  else if (PixType == typeid(unsigned short)) pixelName = "unsigned_short";
  else if (PixType == typeid(int)) pixelName = "int";
  else if (PixType == typeid(unsigned int)) pixelName = "unsigned_int";
  else if (PixType == typeid(float)) pixelName = "float";
  else if (PixType == typeid(double)) pixelName = "double";
  else pixelName = PixType.name();
  return pixelName;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
void CloneImage(const typename ImageType::Pointer & input, typename ImageType::Pointer & output)
{
  output->SetRegions(input->GetLargestPossibleRegion());
  output->SetOrigin(input->GetOrigin());
  output->SetSpacing(input->GetSpacing());
  output->Allocate();
  typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  ConstIteratorType pi(input,input->GetLargestPossibleRegion());
  pi.GoToBegin();
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType po(output,input->GetLargestPossibleRegion());
  po.GoToBegin();
  while (!pi.IsAtEnd()) {
    po.Set(pi.Get());
    ++pi;
    ++po;
  }
}
//--------------------------------------------------------------------

#endif /* end #define CLITKCOMMON_TXX */

