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
#include "vvLandmarks.h"

#include <ios>
#include <fstream>
#include <sstream>
#include <string>
#include <locale.h>

#include <vtkVersion.h>
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "clitkCommon.h"
#include <itksys/SystemTools.hxx>

//--------------------------------------------------------------------
vvLandmarks::vvLandmarks(int size)
{
  mLandmarks.resize(size);
  mFilenames.resize(0);
  mTime = 0;

  for (int i = 0; i < size; i++) {
    mLandmarks[i].resize(0);
    vtkPoints *points = vtkPoints::New();
    mPoints.push_back(points);
    mIds.push_back(vtkFloatArray::New());
    mLabels.push_back(vtkStringArray::New());
    mLabels.back()->SetName("labels");
  }
  mPolyData = vtkPolyData::New();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vvLandmarks::~vvLandmarks()
{
  for (unsigned int i = 0; i < mPoints.size(); i++) {
    mPoints[i]->Delete();
    mIds[i]->Delete();
    mLabels[i]->Delete();
  }
  /*for(unsigned int i = 0; i < mText.size(); i++) {
    mText[i]->Delete();
    }*/
  if (mPolyData)
    mPolyData->Delete();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvLandmarks::AddLandmark(float x,float y,float z,float t,double value)
{
  vvLandmark point;
  vtkIdType idPoint;
  point.coordinates[0] = x;
  point.coordinates[1] = y;
  point.coordinates[2] = z;
  point.coordinates[3] = t;
  point.pixel_value=value;
  mLandmarks[mTime].push_back(point);

  idPoint = mPoints[int(t)]->InsertNextPoint(x,y,z);
  std::string str_vtkIdType;	    // string which will contain the result
  std::ostringstream convert;	    // stream used for the conversion
  convert << idPoint;	    		// insert the textual representation of 'idPoint' in the characters in the stream
  str_vtkIdType = convert.str();    // set 'str_vtkIdType' to the contents of the stream
  mLabels[mTime]->InsertNextValue(str_vtkIdType.c_str());

  std::stringstream numberVal;
  numberVal << (mLandmarks.size()-1);
  /*
  vvLandmarksGlyph *number = vvLandmarksGlyph::New();
  number->SetText(numberVal.str().c_str());
  number->BackingOff();
  DD(numberVal.str().c_str());
  mText.push_back(number);
  */

  mIds[mTime]->InsertNextTuple1(0.55);
  //mIds->InsertTuple1(mLandmarks.size(),mLandmarks.size());
  SetTime(int(t));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvLandmarks::RemoveLastLandmark()
{
  mPoints[mTime]->SetNumberOfPoints(mPoints[mTime]->GetNumberOfPoints()-1);
  //  mText.pop_back();
  mLandmarks[mTime].pop_back();
  mIds[mTime]->RemoveLastTuple();
  mLabels[mTime]->SetNumberOfValues(mLabels[mTime]->GetNumberOfValues()-1);
  mLabels[mTime]->Modified();
  mPolyData->Modified();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvLandmarks::RemoveLandmark(int index)
{
  // erase a vtkPoint by shifiting the array .
  // not a problem here because there are no 
  // pologyons linking the points
  int t = mTime;//mLandmarks[index].coordinates[3];
  int npoints = mPoints[t]->GetNumberOfPoints();
  for (int i = index; i < npoints - 1; i++) {
    mPoints[t]->InsertPoint(i, mPoints[t]->GetPoint(i+1));
	std::string str_i;		     // string which will contain the result
	std::ostringstream convert;	 // stream used for the conversion
	convert << i;			     // insert the textual representation of 'i' in the characters in the stream
	str_i = convert.str();		 // set 'str_i' to the contents of the stream
	mLabels[t]->SetValue(i,str_i.c_str());
    }
  mPoints[t]->SetNumberOfPoints(npoints-1);
  mLabels[t]->SetNumberOfValues(npoints-1);
  mLabels[t]->Modified();
  mPolyData->Modified();

  mLandmarks[t].erase(mLandmarks[t].begin() + index);
  mIds[t]->RemoveLastTuple();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvLandmarks::RemoveAll()
{
  for (unsigned int i = 0; i < mLandmarks.size(); i++) {
    mLandmarks[i].clear();
    mPoints[i]->SetNumberOfPoints(0);
    mLabels[i]->SetNumberOfValues(0);
    mIds[i]->SetNumberOfValues(0);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void vvLandmarks::ChangeComments(int index, std::string comments)
{
  mLandmarks[mTime][index].comments = comments;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double vvLandmarks::GetPixelValue(int index)
{
  return mLandmarks[mTime][index].pixel_value;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
float* vvLandmarks::GetCoordinates(int index)
{
  return mLandmarks[mTime][index].coordinates;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string vvLandmarks::GetComments(int index)
{
  return mLandmarks[mTime][index].comments;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool vvLandmarks::LoadFile(std::vector<std::string> filenames)
{
  // all files in the sequence must be of the same type
  std::string extension = itksys::SystemTools::GetFilenameExtension(filenames[0]);
  if (extension == ".txt")
    return LoadTxtFile(filenames);
  else if (extension == ".pts")
    return LoadPtsFile(filenames);

  return false;
}

//--------------------------------------------------------------------
bool vvLandmarks::LoadTxtFile(std::vector<std::string> filenames)
{
  mFilenames = filenames;
  for (unsigned int i = 0; i < mPoints.size(); i++) {
    mLandmarks[i].clear();
    mPoints[i]->SetNumberOfPoints(0);
  }

  int err = 0;
  for (unsigned int f = 0; f < filenames.size(); f++) {
    std::ifstream fp(filenames[f].c_str(), std::ios::in|std::ios::binary);
    if (!fp.is_open()) {
      std::cerr <<"Unable to open file " << filenames[f] << std::endl;
      err++;
    }
    vtkIdType idPoint;
    char line[255];
    bool first_line=true;
    while (fp.getline(line,255)) {
      //    DD(line);
      std::string stringline = line;
      stringline += "\n";
      
      if (first_line) {
        first_line=false;
        ///New landmark format: first line is "LANDMARKSXX", where XX is the version number
        if (stringline.size() >= 10 && stringline.compare(0,9,"LANDMARKS")==0) {
          std::istringstream ss(stringline.c_str()+9);
          ss >> mFormatVersion;
          continue; //skip first line
        } else
          mFormatVersion=0;
      }
      if (stringline.size() > 1) {
        vvLandmark point;
        int previousSpace = 0;
        int space=0;
        if (mFormatVersion>0) {
          space = stringline.find(" ", previousSpace+1);
          if (space < -1 || space > (int)stringline.size()) {
            ErrorMsg(mLandmarks.size(),"index");
            continue;
          }
          //int index = atoi(stringline.substr(previousSpace,space - previousSpace).c_str());
          previousSpace = space;
        }
        space = stringline.find(" ", previousSpace+1);
        if (space < -1 || space > (int)stringline.size()) {
          ErrorMsg(mLandmarks.size(),"x position");
          continue;
        }
        point.coordinates[0] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
        //      DD(point.coordinates[0]);
        previousSpace = space;
        space = stringline.find(" ", previousSpace+1);
        if (space < -1 || space > (int)stringline.size()) {
          ErrorMsg(mLandmarks.size(),"y position");
          continue;
        }
        point.coordinates[1] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
        //      DD(point.coordinates[1]);
        previousSpace = space;
        space = stringline.find(" ", previousSpace+1);
        if (space < -1 || space > (int)stringline.size()) {
          ErrorMsg(mLandmarks.size(),"z position");
          continue;
        }
        point.coordinates[2] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
        previousSpace = space;
        if (mFormatVersion>0) {
          space = stringline.find(" ", previousSpace+1);
          if (space < -1 || space > (int)stringline.size()) {
            ErrorMsg(mLandmarks.size(),"t position");
            continue;
          }
          point.coordinates[3] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
          previousSpace = space;
          space = stringline.find(" ", previousSpace+1);
          if (space < -1 || space > (int)stringline.size()) {
            ErrorMsg(mLandmarks.size(),"pixel value");
            continue;
          }
          point.pixel_value = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
          //        DD(point.pixel_value);
        } else {
          point.pixel_value=0.; //Not in file
          point.coordinates[3]=(float)f;
        }
        previousSpace = space;
        //this is the maximum size of comments
        space = (stringline.find("\n", previousSpace+1) < 254 ? stringline.find("\n", previousSpace+1) : 254);
        if (previousSpace != -1) {
          point.comments = stringline.substr(previousSpace,space - (previousSpace)).c_str();
        }
        //      DD(point.comments);
        mLandmarks[int(point.coordinates[3])].push_back(point);
        mIds[int(point.coordinates[3])]->InsertNextTuple1(0.55);
        idPoint = mPoints[int(point.coordinates[3])]->InsertNextPoint(
                                                            point.coordinates[0],point.coordinates[1],point.coordinates[2]);
        std::string str_vtkIdType;	    // string which will contain the result
        std::ostringstream convert;	// stream used for the conversion
        convert << idPoint;		    // insert the textual representation of 'idPoint' in the characters in the stream
        str_vtkIdType = convert.str(); // set 'str_vtkIdType' to the contents of the stream
        mLabels[int(point.coordinates[3])]->InsertNextValue(str_vtkIdType.c_str());
      }
    }
  }

  if (err > 0 && err == filenames.size())
    return false;
  
  SetTime(0);
  
  return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool vvLandmarks::LoadPtsFile(std::vector<std::string> filenames)
{
  mFilenames = filenames;
  for (unsigned int i = 0; i < mPoints.size(); i++) {
    mPoints[i]->SetNumberOfPoints(0);
    mLandmarks[i].clear();
  }

  int err = 0;
  for (unsigned int f = 0; f < filenames.size(); f++) {
    std::ifstream fp(filenames[f].c_str(), std::ios::in|std::ios::binary);
    if (!fp.is_open()) {
      std::cerr <<"Unable to open file " << filenames[f] << std::endl;
      err++;
    }
    vtkIdType idPoint;
    char line[255];
    bool first_line=true;
    while (fp.getline(line,255)) {
      std::string stringline = line;
      stringline += "\n";
      
      std::string separators = "\t\n\r ";
      if (stringline.size() > 1) {
        vvLandmark point;
        int previousSpace = 0;
        int space=0;

        if (stringline[0] == '#') // comments
          continue;
        
        space = stringline.find_first_of(separators, previousSpace+1);
        if (space == std::string::npos) {
          ErrorMsg(mLandmarks.size(),"x position");
          continue;
        }
        point.coordinates[0] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
        //      DD(point.coordinates[0]);
        previousSpace = space;
        space = stringline.find_first_of(separators, previousSpace+1);
        if (space == std::string::npos) {
          ErrorMsg(mLandmarks.size(),"y position");
          continue;
        }
        point.coordinates[1] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
        //      DD(point.coordinates[1]);
        previousSpace = space;
        space = stringline.find_first_of(separators, previousSpace+1);
        if (space == std::string::npos) {
          ErrorMsg(mLandmarks.size(),"z position");
          continue;
        }
        point.coordinates[2] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
        previousSpace = space;
        point.pixel_value=0.; //Not in file
        point.coordinates[3]=(float)f;  //Not in file
        point.comments = "";  //Not in file

        //      DD(point.comments);
        mLandmarks[int(point.coordinates[3])].push_back(point);
        mIds[int(point.coordinates[3])]->InsertNextTuple1(0.55);
        idPoint = mPoints[int(point.coordinates[3])]->InsertNextPoint(
                                                            point.coordinates[0],point.coordinates[1],point.coordinates[2]);
        std::string str_vtkIdType;     // string which will contain the result
        std::ostringstream convert;  // stream used for the conversion
        convert << idPoint;        // insert the textual representation of 'idPoint' in the characters in the stream
        str_vtkIdType = convert.str(); // set 'str_vtkIdType' to the contents of the stream
        mLabels[int(point.coordinates[3])]->InsertNextValue(str_vtkIdType.c_str());
      }
    }
  }
  
  SetTime(0);
  DD("vvLandmarks::LoadPtsFile")
  if (err > 0 && err == filenames.size())
    return false;

  
  return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool vvLandmarks::ErrorMsg(int num,const char * text)
{
  std::cerr << "error when loading point " << num << " at " << text << std::endl;
  return false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvLandmarks::SaveFile(std::string filename)
{
  std::string fileContent = "LANDMARKS1\n"; //File format version identification
  for (unsigned int t = 0; t < mLandmarks.size(); t++) {
    for (unsigned int i = 0; i < mLandmarks[t].size(); i++) {
      std::stringstream out;
      out.imbue(std::locale("C")); //This is to specify that the dot is to be used as the decimal separator
      out << i << " "
          << mLandmarks[t][i].coordinates[0] << " "
          << mLandmarks[t][i].coordinates[1] << " "
          << mLandmarks[t][i].coordinates[2] << " "
          << mLandmarks[t][i].coordinates[3] << " "
          << mLandmarks[t][i].pixel_value << " ";
      fileContent += out.str();
      if (mLandmarks[t][i].comments.size() == 0)
        fileContent += " ";
      else
        fileContent += mLandmarks[t][i].comments;
      fileContent += "\n";
    }
  }
  std::ofstream fp(filename.c_str(), std::ios::trunc);
  if ( !fp ) {
    std::cerr << "Unable to open file" << std::endl;
    return;
  }
  fp << fileContent.c_str()<< std::endl;
  fp.close();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void vvLandmarks::SetTime(int time)
{
  if (time >= 0 && time <= ((int)mPoints.size() -1)) {
    mPolyData->SetPoints(mPoints[time]);
    mPolyData->GetPointData()->SetScalars(mIds[time]);
    mPolyData->GetPointData()->AddArray(mLabels[time]);
    mPolyData->Modified();
#if VTK_MAJOR_VERSION <= 5
    mPolyData->Update();
#else
    //mPolyData->Update();
#endif
    mTime = time;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string vvLandmarks::replace_dots(std::string input)
{
  ///Replaces the dots used in the file with the decimal separator in use on the platform
  lconv * conv=localeconv();
  unsigned int position = input.find( "." );
  while ( position < input.size() ) {
    input.replace(position, 1, conv->decimal_point);
    position = input.find( ".", position + 1 );
  }
  return input;
}
//--------------------------------------------------------------------
