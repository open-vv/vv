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
#ifndef __clitkPointListReader_h
#define __clitkPointListReader_h
#include "clitkPointListReader.h"

namespace clitk
{


  template <unsigned int Dimension>
  class ITK_EXPORT PointListReader : 
    public itk::LightObject 
  {
  public:
    //------------------------------------
    // Standard itk typedefs
    //-----------------------------------
    typedef PointListReader                     Self;
    typedef LightObject                         Superclass;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self); 

    /** Run-time type information (and related methods). */
    itkTypeMacro(PointListReader, PointListReader); 

    // Typedefs
    typedef itk::Point<double, Dimension> PointType;
    typedef std::vector<PointType> PointListType;
    typedef std::vector< PointListType> PointListsType;   

    void SetVerbose(bool a){m_Verbose=a;}
    PointListType Read(const  std::string &);
    PointListsType Read(char**, const unsigned int &);
    

  protected:
    
    // Constructor & Destructor
    PointListReader();
    ~PointListReader(){};

    bool m_Verbose;

  };

} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkPointListReader.txx"
#endif

#endif
