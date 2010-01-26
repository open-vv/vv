/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
#include "vvLandmarks.h"

#include <ios>
#include <fstream>
#include <sstream>
#include <string>
#include <locale.h>

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "clitkCommon.h"

vvLandmarks::vvLandmarks(int size)
{
    mLandmarks.resize(0);
    mFilename = "";

    for (int i = 0; i < size; i++)
    {
        vtkPoints *points = vtkPoints::New();
        mPoints.push_back(points);
    }
    mPolyData = vtkPolyData::New();
    mIds = vtkFloatArray::New();
}

vvLandmarks::~vvLandmarks()
{
    for (unsigned int i = 0; i < mPoints.size(); i++) {
        mPoints[i]->Delete();
    }
    /*for(unsigned int i = 0; i < mText.size(); i++) {
      mText[i]->Delete();
      }*/
    if (mIds)
        mIds->Delete();
    if (mPolyData)
        mPolyData->Delete();
}

void vvLandmarks::AddLandmark(float x,float y,float z,float t,double value)
{
    vvLandmark point;
    point.coordinates[0] = x;
    point.coordinates[1] = y;
    point.coordinates[2] = z;
    point.coordinates[3] = t;
    point.pixel_value=value;
    mLandmarks.push_back(point);
    mPoints[int(t)]->InsertNextPoint(x,y,z);

    /*std::stringstream numberVal;
    numberVal << (mLandmarks.size()-1);
    vvLandmarksGlyph *number = vvLandmarksGlyph::New();
    number->SetText(numberVal.str().c_str());
    number->BackingOff();
    mText.push_back(number);*/
    mIds->InsertNextTuple1(0.55);
    //mIds->InsertTuple1(mLandmarks.size(),mLandmarks.size());
    SetTime(int(t));
}

void vvLandmarks::RemoveLastLandmark()
{
    mPoints[mLandmarks.back().coordinates[3]]->SetNumberOfPoints(
        mPoints[mLandmarks.back().coordinates[3]]->GetNumberOfPoints()-1);
    mPolyData->Modified();
    //mText.pop_back();
    mLandmarks.pop_back();
    mIds->RemoveLastTuple();
}

void vvLandmarks::ChangeComments(int index, std::string comments)
{
    mLandmarks[index].comments = comments;
}

double vvLandmarks::GetPixelValue(int index)
{
    return mLandmarks[index].pixel_value;
}

float* vvLandmarks::GetCoordinates(int index)
{
    return mLandmarks[index].coordinates;
}

std::string vvLandmarks::GetComments(int index)
{
    return mLandmarks[index].comments;
}

void vvLandmarks::LoadFile(std::string filename)
{
    std::ifstream fp(filename.c_str(), std::ios::in|std::ios::binary);
    if (!fp.is_open())
    {
        std::cerr <<"Unable to open file " << filename << std::endl;
        return;
    }
    mFilename = filename;
    mLandmarks.clear();
    char line[255];
    for (unsigned int i = 0; i < mPoints.size();i++)
        mPoints[i]->SetNumberOfPoints(0);
    bool first_line=true;
    while (fp.getline(line,255))
    {
        std::string stringline = line;
        if (first_line)
        {
            first_line=false;
            ///New landmark format: first line is "LANDMARKSXX", where XX is the version number
            if (stringline.size() >= 10 && stringline.compare(0,9,"LANDMARKS")==0)
            {
                std::istringstream ss(stringline.c_str()+9);
                ss >> mFormatVersion;
                continue; //skip first line
            }
            else 
                mFormatVersion=0;
        }
        DD(mFormatVersion);
        if (stringline.size() > 1)
        {
            vvLandmark point;
            int previousSpace = 0;
            int space=0;
            if (mFormatVersion>0)
            {
                space = stringline.find(" ", previousSpace+1);
                if (space < -1 || space > (int)stringline.size())
                {
                    ErrorMsg(mLandmarks.size(),"index");
                    continue;
                }
                //int index = atoi(stringline.substr(previousSpace,space - previousSpace).c_str());
                previousSpace = space;
            }
            space = stringline.find(" ", previousSpace+1);
            if (space < -1 || space > (int)stringline.size())
            {
                ErrorMsg(mLandmarks.size(),"x position");
                continue;
            }
            point.coordinates[0] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
            previousSpace = space;
            space = stringline.find(" ", previousSpace+1);
            if (space < -1 || space > (int)stringline.size())
            {
                ErrorMsg(mLandmarks.size(),"y position");
                continue;
            }
            point.coordinates[1] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
            previousSpace = space;
            space = stringline.find(" ", previousSpace+1);
            if (space < -1 || space > (int)stringline.size())
            {
                ErrorMsg(mLandmarks.size(),"z position");
                continue;
            }
            point.coordinates[2] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
            previousSpace = space;
            if (mFormatVersion>0)
            {
                space = stringline.find(" ", previousSpace+1);
                if (space < -1 || space > (int)stringline.size())
                {
                    ErrorMsg(mLandmarks.size(),"t position");
                    continue;
                }
                point.coordinates[3] = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
                previousSpace = space;
                space = stringline.find(" ", previousSpace+1);
                if (space < -1 || space > (int)stringline.size())
                {
                    ErrorMsg(mLandmarks.size(),"pixel value");
                    continue;
                }
                point.pixel_value = atof(replace_dots(stringline.substr(previousSpace,space - previousSpace)).c_str());
            }
            else
            {
                point.pixel_value=0.; //Not in file
                point.coordinates[3]=0.;
            }
            previousSpace = space;
            //this is the maximum size of comments
            space = (stringline.find("\n", previousSpace+1) < 254 ? stringline.find("\n", previousSpace+1) : 254);
            point.comments = stringline.substr(previousSpace,space - (previousSpace)).c_str();
            mLandmarks.push_back(point);
            mIds->InsertNextTuple1(0.55);
            mPoints[int(point.coordinates[3])]->InsertNextPoint(
                point.coordinates[0],point.coordinates[1],point.coordinates[2]);
        }
    }
    SetTime(0);
}

bool vvLandmarks::ErrorMsg(int num,const char * text)
{
    std::cerr << "error when loading point " << num << " at " << text << std::endl;
    return false;
}

void vvLandmarks::SaveFile(std::string filename)
{
    std::string fileContent = "LANDMARKS1\n"; //File format version identification
    for (unsigned int i = 0; i < mLandmarks.size(); i++) {
        std::stringstream out;
        out.imbue(std::locale("C")); //This is to specify that the dot is to be used as the decimal separator
        out << i << " "
            << mLandmarks[i].coordinates[0] << " "
            << mLandmarks[i].coordinates[1] << " "
            << mLandmarks[i].coordinates[2] << " "
            << mLandmarks[i].coordinates[3] << " "
            << mLandmarks[i].pixel_value << " ";
        fileContent += out.str();
        if (mLandmarks[i].comments.size() == 0)
            fileContent += " ";
        else
            fileContent += mLandmarks[i].comments;
        fileContent += "\n";
    }
    std::ofstream fp(filename.c_str(), std::ios::trunc);
    if ( !fp )
    {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }
    fp << fileContent.c_str()<< std::endl;
    fp.close();
}

void vvLandmarks::SetTime(int time)
{
    if (time >= 0 && time <= ((int)mPoints.size() -1))
    {
        mPolyData->SetPoints(mPoints[time]);
        mPolyData->GetPointData()->SetScalars(mIds);
        mPolyData->Modified();
        mPolyData->Update();
    }
}

std::string vvLandmarks::replace_dots(std::string input)
{
    ///Replaces the dots used in the file with the decimal separator in use on the platform
    lconv * conv=localeconv();
    unsigned int position = input.find( "." );
    while ( position < input.size() ) 
    {
        input.replace(position, 1, conv->decimal_point);
        position = input.find( ".", position + 1 );
    } 
    return input;
}
