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
===========================================================================*/
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: RelativePositionPropImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2010/07/12 06:57:25 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/

#ifndef __RelativePositionPropImageFilter_h
#define __RelativePositionPropImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkConceptChecking.h"
#include "itkPointSet.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkMinimumImageFilter.h"

namespace itk
{


  /** \class RelativePositionPropImageFilter
   * \brief Compute the fuzzy subset of an image which satisfies some directional relative position.
   * \author Jamal Atif and Olivier Nempont
   * 
   * This filter computes a fuzzy subset of an image satisfying a particular directionnal relative position from an object (crisp or fuzzy).
   *
   * 
   *	\par INPUT / OUTPUT
   *	This filter takes a crisp or a fuzzy object as input. 
   *  In fuzzy case, the values have to be defined between 0 and 1. 
   *  
   *  The result is a fuzzy subset which values are defined between
   *  0 if the relation isn't fulfilled in this point to 1 is the relation is 
   *  fully satisfied.
   *  WARNING: the output image type as to be decimal.
   *  
   *	\par PARAMETERS
   *  \par
   *  The Alpha1 and Alpha2 parameters are used to specify the direction.
   *  Alpha1 is the angle in 'xy' plane from 'x' unit vector.
   *  Alpha2 is used in 3D to specify the angle with 'xy' plane
   *  
   *  \par
   * 	K is an opening parameter. Higher value enlarge the support of the result.
   *  By default it is fixed at PI/2  
   * 
   *  \par REFERENCE
   *   Fuzzy Relative Position Between Objects in Image Processing: A Morphological Approach
   * 	 Isabelle Bloch
   *   IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, VOL. 21, NO. 7, JULY 1999
   *   
   *   This filter is implemented using the propagation algorithm
   */

  template <class TInputImage, class TOutputImage, class TtNorm=Functor::Minimum<
                                                     typename TOutputImage::PixelType,
                                                     typename TOutputImage::PixelType,
                                                     typename TOutputImage::PixelType>  >
  class ITK_EXPORT RelativePositionPropImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage > 
  {
  public:
    /** Standard class typedefs. */
    typedef RelativePositionPropImageFilter Self;
    typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Extract some information from the image types.  Dimensionality
     * of the two images is assumed to be the same. */
    typedef typename TOutputImage::PixelType OutputPixelType;
    typedef typename TOutputImage::InternalPixelType OutputInternalPixelType;
    typedef typename TInputImage::PixelType InputPixelType;
    typedef typename TInputImage::InternalPixelType InputInternalPixelType;

    /** Extract some information from the image types.  Dimensionality
     * of the two images is assumed to be the same. */
    itkStaticConstMacro(ImageDimension, unsigned int,
                        TOutputImage::ImageDimension);
  

    typedef typename itk::Image<typename TInputImage::IndexType , ImageDimension>
      CorrespondanceMapType;
    typedef float TabulationPixelType;
    typedef typename itk::Image<TabulationPixelType , ImageDimension> TabulationImageType;
  
  
    /** Image typedef support. */
    typedef TInputImage  InputImageType;
    typedef TOutputImage OutputImageType;
  
    typedef TtNorm TNormType;
  
    typedef itk::Vector<double, ImageDimension> VectorType;


    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(RelativePositionPropImageFilter, ImageToImageFilter);

    /** The output pixel type must be signed. */
    itkConceptMacro(SignedOutputPixelType, (Concept::Signed<OutputPixelType>));
  
    /** Standard get/set macros for filter parameters. */

  
    itkSetMacro(Alpha1, double);
    itkGetMacro(Alpha1, double);
    itkSetMacro(Alpha2, double);
    itkGetMacro(Alpha2, double);
  
    itkSetMacro(K1, double);
    itkGetMacro(K1, double);
    //   itkSetMacro(K2, double);
    //   itkGetMacro(K2, double);

    itkSetMacro(Radius, double);
    itkGetMacro(Radius, double);

    itkSetMacro(VerboseProgress, bool);
    itkGetMacro(VerboseProgress, bool);
    itkBooleanMacro(VerboseProgress);

    itkSetMacro(Fast, bool);
    itkGetMacro(Fast, bool);
    itkBooleanMacro(Fast);
  
    void computeDirection()
    {
      if (ImageDimension == 2) {
          m_DirectionVector[0]=cos(m_Alpha1);
          m_DirectionVector[1]=sin(m_Alpha1);		
        }
      else { // 3D
        m_DirectionVector[0]=cos(m_Alpha1)*cos(m_Alpha2);
        m_DirectionVector[1]=cos(m_Alpha2)*sin(m_Alpha1);
        m_DirectionVector[2]=sin(m_Alpha2);
      }
    }
  

    virtual void GenerateInputRequestedRegion() throw(InvalidRequestedRegionError);
    void EnlargeOutputRequestedRegion (DataObject * output);

  protected:
    RelativePositionPropImageFilter()
      {
        m_Alpha1 = 0;
        m_Alpha2 = 0;
        m_K1 = vcl_acos(-1.0)/2;
        // m_K2 = 3.1417/2;
        m_Radius = 2; // DS
        m_Fast = true; // DS
        m_VerboseProgress = false;
      }
    virtual ~RelativePositionPropImageFilter() {}
    void PrintSelf(std::ostream& os, Indent indent) const;

    //void GenerateThreadedData(const typename TOutputImage::RegionType& outputRegionForThread, int threadId);
    void GenerateData();

  private:
    RelativePositionPropImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented


    /** The angles*/
    double m_Alpha1; 
    double m_Alpha2; 
    double m_K1;
    // double m_K2;
  
    unsigned int m_Radius;
    TNormType m_TNorm;
    bool m_VerboseProgress;
     
    VectorType m_DirectionVector; 

    /**
     * 2 pass instead of 2^NDimension. Warning this may cause some artifacts 
     */
    bool m_Fast;

    //allocation et initialisation de la carte de correspondance
    typename CorrespondanceMapType::Pointer InitCorrespondanceMap();
  
    //compute the tabulation map
    typename TabulationImageType::Pointer ComputeAngleTabulation();
  
  
  };
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "RelativePositionPropImageFilter.txx"
#endif

#endif
