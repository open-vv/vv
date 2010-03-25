#ifndef CLITKORIENTATION_CXX
#define CLITKORIENTATION_CXX
/**
   =================================================
   * @file   clitkOrientation.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   01 Nov 2006 18:02:49
   * 
   * @brief  
   * 
   * 
   =================================================*/

#include "clitkOrientation.h"

//====================================================================
itk::SpatialOrientation::CoordinateTerms clitk::GetOrientation(char c)
{
  if ((c == 'R') || (c == 'r')) return itk::SpatialOrientation::ITK_COORDINATE_Right;
  if ((c == 'L') || (c == 'l')) return itk::SpatialOrientation::ITK_COORDINATE_Left;
  if ((c == 'P') || (c == 'p')) return itk::SpatialOrientation::ITK_COORDINATE_Posterior;
  if ((c == 'A') || (c == 'a')) return itk::SpatialOrientation::ITK_COORDINATE_Anterior;
  if ((c == 'I') || (c == 'i')) return itk::SpatialOrientation::ITK_COORDINATE_Inferior;
  if ((c == 'S') || (c == 's')) return itk::SpatialOrientation::ITK_COORDINATE_Superior;
  std::cerr <<"I don't know the orientation '" << c 
	    << "'. Valid letters are LR/AP/IS (or in lowercase)" << std::endl;
  exit(0);
}
//====================================================================

//====================================================================
bool clitk::CheckOrientation(itk::SpatialOrientation::CoordinateTerms a,
			     itk::SpatialOrientation::CoordinateTerms b) 
{
  if ((a==itk::SpatialOrientation::ITK_COORDINATE_Right) || 
      (a==itk::SpatialOrientation::ITK_COORDINATE_Left)) {
    if ((b==itk::SpatialOrientation::ITK_COORDINATE_Right) ||
	(b==itk::SpatialOrientation::ITK_COORDINATE_Left)) {
      return false;
    }
  }
  if ((a==itk::SpatialOrientation::ITK_COORDINATE_Posterior) || 
      (a==itk::SpatialOrientation::ITK_COORDINATE_Anterior)) {
    if ((b==itk::SpatialOrientation::ITK_COORDINATE_Posterior) ||
	(b==itk::SpatialOrientation::ITK_COORDINATE_Anterior)) {
      return false;
    }
  }
  if ((a==itk::SpatialOrientation::ITK_COORDINATE_Inferior) || 
      (a==itk::SpatialOrientation::ITK_COORDINATE_Superior)) {
    if ((b==itk::SpatialOrientation::ITK_COORDINATE_Inferior) ||
	(b==itk::SpatialOrientation::ITK_COORDINATE_Superior)) {
      return false;
    }
  }
  return true;
}
//====================================================================

//====================================================================
itk::SpatialOrientation::ValidCoordinateOrientationFlags clitk::GetOrientation(char a, char b, char c)
{
  itk::SpatialOrientation::CoordinateTerms f1 = clitk::GetOrientation(a);
  itk::SpatialOrientation::CoordinateTerms f2 = clitk::GetOrientation(b);
  itk::SpatialOrientation::CoordinateTerms f3 = clitk::GetOrientation(c);

  if (CheckOrientation(f1, f2) && CheckOrientation(f2,f3) && CheckOrientation(f1,f3)) {
    return static_cast<itk::SpatialOrientation::ValidCoordinateOrientationFlags>(
										 (f1     << itk::SpatialOrientation::ITK_COORDINATE_PrimaryMinor) 
										 + (f2  << itk::SpatialOrientation::ITK_COORDINATE_SecondaryMinor)
										 + (f3 << itk::SpatialOrientation::ITK_COORDINATE_TertiaryMinor));
  }
  std::cerr <<"I don't know the orientation '" << a << b << c 
	    << "'. Valid letters are LR/AP/IS (or in lowercase)" << std::endl;
  exit(0);
}
//====================================================================

//====================================================================
itk::SpatialOrientation::ValidCoordinateOrientationFlags clitk::GetOrientation(const std::string & orient)
{
  if (orient.size() >= 3) return GetOrientation(orient[0], orient[1], orient[2]);
  std::cerr <<"I don't know the orientation '" << orient 
	    << "'. Valid string are three letters LR/AP/IS (or in lowercase)" << std::endl;
  exit(0);
}
//====================================================================

//====================================================================
itk::SpatialOrientation::CoordinateTerms clitk::GetOrientation(const int i, const itk::SpatialOrientation::ValidCoordinateOrientationFlags orient)
{
  if (i==0) return static_cast<itk::SpatialOrientation::CoordinateTerms>((orient << 24) >> 24);
  if (i==1) return static_cast<itk::SpatialOrientation::CoordinateTerms>((orient << 16) >> 24);
  if (i==2) return static_cast<itk::SpatialOrientation::CoordinateTerms>(orient >> 16);
  std::cerr <<"Invalid index = " << i << " in GetOrientation" << std::endl;
  exit(0);
}
//====================================================================

//====================================================================
int clitk::WhereIsDimInThisOrientation(const int dim, const itk::SpatialOrientation::ValidCoordinateOrientationFlags flag) {
  if (dim ==0) {
    for(int i=0; i<3; i++) {
      int j = GetOrientation(i, flag);
      if ((j == itk::SpatialOrientation::ITK_COORDINATE_Right) || 
	  (j == itk::SpatialOrientation::ITK_COORDINATE_Left)) return i;
    }
  }	
  if (dim ==1) {
    for(int i=0; i<3; i++) {
      int j = GetOrientation(i, flag);
      if ((j == itk::SpatialOrientation::ITK_COORDINATE_Anterior) || 
	  (j == itk::SpatialOrientation::ITK_COORDINATE_Posterior)) return i;
    }
  }	
  if (dim ==2) {
    for(int i=0; i<3; i++) {
      int j = GetOrientation(i, flag);
      if ((j == itk::SpatialOrientation::ITK_COORDINATE_Superior) || 
	  (j == itk::SpatialOrientation::ITK_COORDINATE_Inferior)) return i;
    }
  }	
  return 0; // just to avoid warning
}
//====================================================================

//====================================================================
int clitk::GetDim(const itk::SpatialOrientation::CoordinateTerms t)
{
  if ((t == itk::SpatialOrientation::ITK_COORDINATE_Right) || 
      (t == itk::SpatialOrientation::ITK_COORDINATE_Left)) return 0;
  if ((t == itk::SpatialOrientation::ITK_COORDINATE_Anterior) ||
      (t == itk::SpatialOrientation::ITK_COORDINATE_Posterior)) return 1;
  if ((t == itk::SpatialOrientation::ITK_COORDINATE_Inferior) || 
      (t == itk::SpatialOrientation::ITK_COORDINATE_Superior)) return 2;
  std::cerr <<"Invalid CoordinateTerms = " << t << std::endl;
  exit(0);
}
//====================================================================

//====================================================================
void clitk::FlipPoint(const itk::Point<double, 3> in, 
		      const itk::SpatialOrientation::ValidCoordinateOrientationFlags inFlag, 
		      const itk::SpatialOrientation::ValidCoordinateOrientationFlags outFlag, 
		      const itk::Point<double, 3> & imageSize,
		      itk::Point<double, 3> & out)
{
  for(int i=0; i<3; i++) {
    // DD(i);
    itk::SpatialOrientation::CoordinateTerms inT = GetOrientation(i, inFlag);
    // DD(inT);
    int inDim = GetDim(inT);
    // DD(inDim);
    int outDim = WhereIsDimInThisOrientation(inDim, outFlag);
    // DD(outDim);
    // 	DD(in[i]);
    if (inT == GetOrientation(outDim, outFlag)) out[outDim] = in[i];
    else out[outDim] = imageSize[i]-in[i];
    // DD(out[outDim]);
  }
}
//====================================================================

#endif /* end #define CLITKORIENTATION_CXX */

