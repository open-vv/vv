/*-------------------------------------------------------------------------
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
  
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
  
  -------------------------------------------------------------------------*/

#ifndef CLITKCOMMON_TXX
#define CLITKCOMMON_TXX

/**
   -------------------------------------------------
   * @file   clitkCommon.txx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   18 May 2006
   * 
   -------------------------------------------------*/

//--------------------------------------------------------------------
// Convert float, double ... to string
template<class T>
std::string toString(const T & t) {
  std::ostringstream myStream;
  myStream << t << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Convert float*, double* ... to string
template<class T>
std::string toStringVector(const T * t, const int n) {
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
std::string toStringVector(const T & t, const int n) {
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
std::string toStringVector(const std::vector<T> & t) {
  return toStringVector(&t[0], t.size());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Convert a pixel type to another (downcast)
template<class TPixelUp, class TPixelDown>
TPixelDown PixelTypeDownCast(const TPixelUp & x) {
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
void GetSortedIndex(const std::vector<Type> & toSort, std::vector<int> & index, bool increasing) {
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
std::string GetTypeAsString() {
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

#endif /* end #define CLITKCOMMON_TXX */

