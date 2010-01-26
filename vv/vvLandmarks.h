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
#ifndef vvLandmarks_h
#define vvLandmarks_h

#include <iostream>
#include <vector>

#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vvLandmarksGlyph.h"

//typedef
struct vvLandmark {
    float coordinates[4];
    std::string comments;
    double pixel_value;
};

class vvLandmarks
{
public :
    vvLandmarks(int size);
    ~vvLandmarks();

    void LoadFile(std::string filename);
    void SaveFile(std::string filename);

    void AddLandmark(float x,float y,float z,float t,double value);
    void RemoveLastLandmark();
    void ChangeComments(int index, std::string comments);
    float* GetCoordinates(int index);
    double GetPixelValue(int index);
    std::string GetComments(int index);
    int GetNumberOfPoints() {
        return mLandmarks.size();
    }
    //int GetNumberOfSources(){return mText.size();}

    vtkPolyData* GetOutput() {
        return mPolyData;
    }
    //vtkPolyData* GetSources(int i){return mText[i]->GetOutput();}
    void SetTime(int time);

    bool ErrorMsg(int num,const char * text);

private:
    ///Helper function to tackle the use of the comma as the decimal separator
    std::string replace_dots(std::string input);
    std::vector<vvLandmark> mLandmarks;
    vtkPolyData *mPolyData;
    std::vector<vtkPoints*> mPoints;
    vtkFloatArray* mIds;
    //std::vector<vvLandmarksGlyph*> mText;
    std::string mFilename;
    int mFormatVersion;
};

#endif
