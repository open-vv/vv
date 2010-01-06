/*-------------------------------------------------------------------------

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
-------------------------------------------------------------------------*/

#ifndef CLITKCOMMON_CXX
#define CLITKCOMMON_CXX

/**
   -------------------------------------------------
   * @file   clitkCommon.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   17 May 2006 07:59:06
   * 
   * @brief  
   * 
   * 
   -------------------------------------------------*/

#include "clitkCommon.h"
#include "itkMacro.h"

//------------------------------------------------------------------
// skip line which begin with a sharp '#'
void clitk::skipComment(std::istream & is) 
{
  char c;
  char line[1024];
  if (is.eof()) return;
  is >> c ; 
  while (is && (c == '#')) {
	is.getline (line, 1024); 
	is >> c;
	if (is.eof()) return;
  }
  is.unget();
} ////
//------------------------------------------------------------------
  
//------------------------------------------------------------------
// linear (rough) conversion from Hounsfield Unit to density
double clitk::HU2density(double HU) 
{
  return (HU+1000.0)/1000.0;
} ////
//------------------------------------------------------------------

//------------------------------------------------------------------
// Return filename extension
std::string clitk::GetExtension(const std::string& filename) 
{
  // This assumes that the final '.' in a file name is the delimiter
  // for the file's extension type
  const std::string::size_type it = filename.find_last_of( "." );	
  // This determines the file's type by creating a new string
  // who's value is the extension of the input filename
  // eg. "myimage.gif" has an extension of "gif"
  std::string fileExt( filename, it+1, filename.length() );	
  return( fileExt );
} ////
//------------------------------------------------------------------

//------------------------------------------------------------------
// Display progression
void clitk::VerboseInProgress(const int nb, const int current, const int percentage)
{
int stage = current;
  int number_of_stages = nb;

  static int exp = -1;
  int prec = 4;
  int p = (int)(stage*pow(10.0,prec)*1./number_of_stages);
  if (p==exp) return; 
  exp = p;
  float s = p * 100. / pow(10.0,prec);
  char fmt[128];
  sprintf(fmt,"%%%i.%if%%%%",prec<4?prec:prec+1,prec>3?prec-3:0);
  char ch[128];
  
  snprintf(ch,2,fmt,s); 
  //sxsVSCU_MESSAGE(2,ch);
  std::cout << ch << std::flush;
  for (int i=0;i<(prec>3?prec+2:prec+1);++i) 
	std::cout << "\b" << std::flush;//sxsVSCU_MESSAGE(2,"\b");

  /*

  if ((current % (nb/percentage)) == 0) {
	std::cout.width(15); 
	std::cout << "\r" << current << "/" << nb << std::flush;
  }
  */
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Display progression
void clitk::VerboseInProgressInPercentage(const int nb, const int current, const int percentage)
{
  int stage = current;
  int number_of_stages = nb;

  static int exp = -1;
  int prec = 4;
  int p = (int)(stage*pow(10.0,prec)*1./number_of_stages);
  if (p==exp) return; 
  exp = p;
  float s = p * 100. / pow(10.0,prec);
  char fmt[128];
  sprintf(fmt,"%%%i.%if%%%%",prec<4?prec:prec+1,prec>3?prec-3:0);
  char ch[128];
  
  snprintf(ch,2,fmt,s); 
  //sxsVSCU_MESSAGE(2,ch);
  std::cout << ch << std::flush;
  for (int i=0;i<(prec>3?prec+2:prec+1);++i) 
	std::cout << "\b" << std::flush;//sxsVSCU_MESSAGE(2,"\b");

  /*
  if (nb/percentage != 0) {
	if ((current % (nb/percentage)) == 0) {
	  std::cout.width(2); 
	  std::cout << "\r" << (nb/current)*100 << "/100%  " << std::flush;
	}
  }
  */
}
//------------------------------------------------------------------

//------------------------------------------------------------------
// Convert a pixel type to another (downcast)
template<>
float clitk::PixelTypeDownCast(const double & x)
{
  return (float)x;
}
//------------------------------------------------------------------

//------------------------------------------------------------------
double clitk::rad2deg(const double anglerad) {
  return (anglerad/M_PI*180.0);
}
//------------------------------------------------------------------

//------------------------------------------------------------------
double clitk::deg2rad(const double angledeg) {
  return (angledeg*(M_PI/180.0));
}
//------------------------------------------------------------------

//------------------------------------------------------------------
int clitk::GetTypeSizeFromString(const std::string & type) {
#define RETURN_SIZEOF_PIXEL(TYPENAME, TYPE)		\
  if (type == #TYPENAME) return sizeof(TYPE);
  RETURN_SIZEOF_PIXEL(char, char);
  RETURN_SIZEOF_PIXEL(uchar, uchar);
  RETURN_SIZEOF_PIXEL(unsigned char, uchar);
  RETURN_SIZEOF_PIXEL(unsigned_char, uchar);
  RETURN_SIZEOF_PIXEL(short, short);
  RETURN_SIZEOF_PIXEL(ushort, ushort);
  RETURN_SIZEOF_PIXEL(unsigned_short, ushort);
  RETURN_SIZEOF_PIXEL(int, int);
  RETURN_SIZEOF_PIXEL(uint, uint);
  RETURN_SIZEOF_PIXEL(unsigned_int, uint);
  RETURN_SIZEOF_PIXEL(float, float);
  RETURN_SIZEOF_PIXEL(double, double);
  return 0;
}
//------------------------------------------------------------------

//------------------------------------------------------------------
template<>
bool clitk::IsSameType<signed char>(std::string t) { 
  if ((t==GetTypeAsString<signed char>()) || (t == "schar")) return true; else return false; 
}

template<>
bool clitk::IsSameType<char>(std::string t) { 
  if ((t==GetTypeAsString<char>()) || (t == "char")) return true; else return false; 
}

template<>
bool clitk::IsSameType<unsigned char>(std::string t) { 
  if ((t==GetTypeAsString<unsigned char>()) || (t == "uchar")) return true; else return false; 
}

template<>
bool clitk::IsSameType<unsigned short>(std::string t) { 
  if ((t==GetTypeAsString<unsigned short>()) || (t == "ushort")) return true; else return false; 
}
//------------------------------------------------------------------

//------------------------------------------------------------------
void clitk::FindAndReplace(std::string & line, 
		    const std::string & tofind, 
		    const std::string & replacement) {
  int pos = line.find(tofind);
  while (pos!= (int)std::string::npos) {
    line.replace(pos, tofind.size(), replacement);
    pos = line.find(tofind, pos+tofind.size()+1);
  }
}
//------------------------------------------------------------------

//------------------------------------------------------------------
void clitk::FindAndReplace(std::string & line, 
		    const std::vector<std::string> & tofind, 
		    const std::vector<std::string> & toreplace) {
  for(unsigned int i=0; i<tofind.size(); i++) {
    FindAndReplace(line, tofind[i], toreplace[i]);
  }
}
//------------------------------------------------------------------

//------------------------------------------------------------------
void clitk::FindAndReplace(std::ifstream & in, 
		    const std::vector<std::string> & tofind, 
		    const std::vector<std::string> & toreplace,
		    std::ofstream & out) {
  std::string line;
  if (tofind.size() != toreplace.size()) {
    std::cerr << "Error' tofind' is size=" << tofind.size() << std::endl;
    std::cerr << "... while 'toreplace' is size=" << toreplace.size() << std::endl;
    exit(0);
  }
  while (std::getline(in,line)) {
    FindAndReplace(line, tofind, toreplace);
    out << line << std::endl;
  }
}
//------------------------------------------------------------------

//------------------------------------------------------------------
double clitk::ComputeEuclideanDistanceFromPointToPlane(const itk::ContinuousIndex<double, 3> point, 
						       const itk::ContinuousIndex<double, 3> pointInPlane, 
						       const itk::ContinuousIndex<double, 3> normalPlane) {
  // http://mathworld.wolfram.com/Plane.html
  // http://mathworld.wolfram.com/Point-PlaneDistance.html
  double a = normalPlane[0];
  double b = normalPlane[1];
  double c = normalPlane[2];
  double x0 = pointInPlane[0];
  double y0 = pointInPlane[1];
  double z0 = pointInPlane[2];
  double x = point[0];
  double y = point[1];
  double z = point[2];

  double norm = sqrt(x0*x0 + y0*y0 + z0*z0);
  DD(norm);
  double d = -a*x0 - b*y0 - c*z0;
  DD(d);
  double distance = (a*x + b*y + c*z + d) / norm;
  
  return distance;
}
//------------------------------------------------------------------

//--------------------------------------------------------------------
// Open a file for reading
void clitk::openFileForReading(std::ifstream & is, const std::string & filename) {
  is.open(filename.c_str(), std::ios::in);
  if ( is.fail() ) {
    itkGenericExceptionMacro(<< "Could not open file (for reading): " << filename);
  }
}
//--------------------------------------------------------------------
  
//--------------------------------------------------------------------
// Open a file for writing
void clitk::openFileForWriting(std::ofstream & os, const std::string & filename)  {
  os.open(filename.c_str(), std::ios::out);
  if ( os.fail() ) {
    itkGenericExceptionMacro(<< "Could not open file (for writing): " << filename);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
double clitk::cotan(double i) { return(1.0 / tan(i)); }
double clitk::invcotan(double x) { 
  //  http://mathworld.wolfram.com/InverseCotangent.html
  double y;
  if (x<0) {
    y = -0.5*M_PI-atan(x);
  }
  else {
    y = 0.5*M_PI-atan(x);
  }
  return y;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
std::streambuf * clitk_stdcerr_backup;
void clitk::disableStdCerr() {
  clitk_stdcerr_backup = std::cerr.rdbuf();
  std::stringstream oss;
  std::cerr.rdbuf( oss.rdbuf() );
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::enableStdCerr() {
  std::cerr.rdbuf(clitk_stdcerr_backup);
}
//--------------------------------------------------------------------

#endif /* end #define CLITKCOMMON_CXX */

