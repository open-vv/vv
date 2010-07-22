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
#ifndef __clitkPointListWriter_h
#define __clitkPointListWriter_h
#include "clitkPointListWriter.h"

namespace clitk
{


  template <unsigned int Dimension>
  class ITK_EXPORT PointListWriter : 
    public itk::LightObject 
  {
  public:
    //------------------------------------
    // Standard itk typedefs
    //-----------------------------------
    typedef PointListWriter                     Self;
    typedef LightObject                         Superclass;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self); 

    /** Run-time type information (and related methods). */
    itkTypeMacro(PointListWriter, LightObject); 

    // Typedefs
    typedef itk::Point<double, Dimension> PointType;
    typedef std::vector<PointType> PointListType;
    typedef std::vector< PointListType> PointListsType; 
    typedef itk::Vector<float, Dimension> DisplacementType;
    typedef std::vector<DisplacementType> DisplacementListType;
    typedef std::vector< DisplacementListType> DisplacementListsType;   

    // Members
    void Write(const  PointListType & , const  std::string &);
    void Write(const  PointListsType & , char** );
    void Write(const  PointListsType & , const  std::string &){;};

    void Write(const  DisplacementListType & , const  std::string &);
    void Write(const  DisplacementListsType & , char**);
    void Write(const  DisplacementListsType & , const  std::string &){;};

    
  protected:
    
    // Constructor & Destructor+
    PointListWriter();
    ~PointListWriter(){};


  };

} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkPointListWriter.txx"
#endif

#endif
