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
#ifndef vvLandmarks_h
#define vvLandmarks_h
#include <iostream>
#include <vector>

#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vvLandmarksGlyph.h"
#include "vtkStringArray.h"

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

    bool LoadFile(std::string filename);
    void SaveFile(std::string filename);

    void AddLandmark(float x,float y,float z,float t,double value);
    void RemoveLastLandmark();
    void RemoveLandmark(int index);
    void ChangeComments(int index, std::string comments);
    float* GetCoordinates(int index);
    double GetPixelValue(int index);
    std::string GetComments(int index);
    unsigned int GetNumberOfPoints() { return (unsigned int) mLandmarks.size(); }
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
    vtkStringArray* mLabels;
    std::string mFilename;
    int mFormatVersion;

    bool LoadTxtFile(std::string filename);
    bool LoadPtsFile(std::string filename);
  
};

#endif
