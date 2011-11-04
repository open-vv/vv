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

#ifndef CLITKANATOMICALFEATUREDATABASE_H
#define CLITKANATOMICALFEATUREDATABASE_H

// clitk
#include "clitkCommon.h"
#include "clitkImageCommon.h"

namespace clitk {
  

  //--------------------------------------------------------------------
  /*
    Class to store and retreive anatomical feature such as 3D
    named landmarks points. 
  */
  class AnatomicalFeatureDatabase: public itk::Object
  {
  public:
    // typedef
    typedef std::string TagType;

    // New macro
    typedef itk::Object                    Superclass;
    typedef AnatomicalFeatureDatabase      Self;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;
    itkNewMacro(Self);
    static Pointer New(std::string filename);

    // Set/Get filename 
    itkSetMacro(Filename, std::string);
    itkGetConstMacro(Filename, std::string);

    // Read and write DB
    void Write();
    void Load();
    itkGetConstMacro(Path, std::string);
    itkSetMacro(Path, std::string);
    
    // Set Get landmarks
    typedef itk::Point<double,3> PointType3D;
    void SetPoint3D(TagType tag, PointType3D & p);
    void GetPoint3D(TagType tag, PointType3D & p);
    double GetPoint3D(std::string tag, int dim);
    bool TagExist(std::string tag);
    std::string GetTagValue(std::string tag);

    // Set Get image
    void SetImageFilename(TagType tag, std::string f);
    template<class ImageType>
    typename ImageType::Pointer GetImage(TagType tag, bool reload=false);
    template<class ImageType>
    void  SetImage(TagType tag, std::string f, typename ImageType::Pointer image, bool write=false);
    template<class ImageType>
    void ReleaseImage(TagType tag);
    
    // Set Get Double
    void SetDouble(TagType tag, double d);
    double GetDouble(TagType tag);
    
    // Remove Tag
    void RemoveTag(TagType tag);

  protected:
    AnatomicalFeatureDatabase();
    ~AnatomicalFeatureDatabase() {}

    std::string m_Filename;
    std::string m_Path;
    typedef itk::ImageBase<3> ImageBaseType;
    typedef std::map<TagType, std::string> MapTagType;
    typedef std::map<TagType, ImageBaseType*> MapTagImageType; 
    MapTagType m_MapOfTag;
    MapTagImageType m_MapOfImage;

  }; // end class
  //--------------------------------------------------------------------

#define NewAFDB(NAME, FILE)                                             \
  clitk::AnatomicalFeatureDatabase::Pointer NAME = clitk::AnatomicalFeatureDatabase::New(FILE);

  #include "clitkAnatomicalFeatureDatabase.txx" 

} // end namespace clitk
//--------------------------------------------------------------------

#endif // CLITKANATOMICALFEATUREDATABASE_H
