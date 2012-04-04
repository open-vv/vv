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
  ======================================================================-====*/

#ifndef CLITKMESHTOBINARYIMAGEFILTER_H
#define CLITKMESHTOBINARYIMAGEFILTER_H

// clitk
#include "clitkCommon.h"

// itk
#include <itkImageSource.h>

// vtk
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

namespace clitk {
    
  /* --------------------------------------------------------------------     
     Convert a mesh composed of a list of 2D contours, into a 3D binray
     image.     
     -------------------------------------------------------------------- */
  
  template <class ImageType>
  class ITK_EXPORT MeshToBinaryImageFilter: public itk::ImageSource<ImageType>
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageSource<ImageType>   Superclass;
    typedef MeshToBinaryImageFilter       Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
       
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(MeshToBinaryImageFilter, Superclass);

    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    typedef itk::Image<float, ImageDimension> FloatImageType;

    /** Some convenient typedefs. */
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   RegionType; 
    typedef typename ImageType::PixelType    PixelType;
    typedef typename ImageType::SpacingType  SpacingType;
    typedef typename ImageType::SizeType     SizeType;
    typedef typename ImageType::PointType    PointType;
    typedef itk::Image<PixelType, ImageDimension-1> SliceType;
    
    /** Input : initial image and object */
    itkSetMacro(Mesh, vtkSmartPointer<vtkPolyData>);
    itkGetConstMacro(Mesh, vtkSmartPointer<vtkPolyData>);

    itkSetMacro(LikeImage, ImagePointer);
    itkGetConstMacro(LikeImage, ImagePointer);

    itkSetMacro(Extrude, bool);
    itkGetMacro(Extrude, bool);
    itkBooleanMacro(Extrude);    

  protected:
    MeshToBinaryImageFilter();
    virtual ~MeshToBinaryImageFilter() {}
    
    virtual void GenerateOutputInformation();
    virtual void GenerateData();

    bool m_Extrude;
    ImagePointer m_LikeImage;
    vtkSmartPointer<vtkPolyData> m_Mesh;

  private:
    MeshToBinaryImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkMeshToBinaryImageFilter.txx"
#endif

#endif
