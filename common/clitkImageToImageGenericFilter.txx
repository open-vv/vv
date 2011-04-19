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
template<class FilterType>
clitk::ImageToImageGenericFilter<FilterType>::ImageToImageGenericFilter(std::string filterName) :
  ImageToImageGenericFilterBase(filterName),
  mImageTypesManager(static_cast<FilterType*>(this))
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class FilterType>
bool clitk::ImageToImageGenericFilter<FilterType>::Update()
{
  GetInputImageDimensionAndPixelType(m_Dim,m_PixelTypeName,m_NbOfComponents);

  // Check ImageType
  if (!CheckImageType()) {
    if (m_FailOnImageTypeError) ImageTypeError();
    else SetImageTypeError();
    return false;
  }

  // Go ! Call the right templatized function
  mImageTypesManager.DoIt(m_Dim, m_NbOfComponents, m_PixelTypeName);
  return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class FilterType>
bool clitk::ImageToImageGenericFilter<FilterType>::CheckImageType(unsigned int dim, unsigned int ncomp, std::string pixeltype)
{
  //SR: commented line creates an element in mMapOfImageTypeToFunction which, even if 0, is confusing, e.g. for GetAvailableImageTypes
  //return static_cast<bool>(mImageTypesManager.mMapOfImageTypeToFunction[dim][ncomp][pixeltype]);
  typename ImageTypesManager<FilterType>::MapOfImageDimensionToFunctionType &m = mImageTypesManager.mMapOfImageTypeToFunction;
  return (m            .find(dim)       != m.end()      &&
          m[dim]       .find(ncomp)     != m[dim].end() &&
          m[dim][ncomp].find(pixeltype) != m[dim][ncomp].end());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class FilterType>
bool clitk::ImageToImageGenericFilter<FilterType>::CheckImageType()
{
  return CheckImageType(m_Dim, m_NbOfComponents, m_PixelTypeName);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class FilterType>
std::string clitk::ImageToImageGenericFilter<FilterType>::GetAvailableImageTypes()
{
  std::ostringstream oss;
  oss << "The filter <" << m_FilterName << "> manages:" << std::endl;

  typedef typename ImageTypesManager<FilterType>::MapOfImageComponentsToFunctionType::const_iterator MCompItType;
  typedef typename ImageTypesManager<FilterType>::MapOfImageDimensionToFunctionType::const_iterator MDimItType;
  typedef typename ImageTypesManager<FilterType>::MapOfPixelTypeToFunctionType::const_iterator MPixelItType;
  for (MDimItType i=mImageTypesManager.mMapOfImageTypeToFunction.begin();
       i!=mImageTypesManager.mMapOfImageTypeToFunction.end();
       i++) {
    for (MCompItType j=(*i).second.begin(); j!= (*i).second.end(); j++) {
      for (MPixelItType k=(*j).second.begin(); k!= (*j).second.end(); k++) {
        oss << "Dim: " << (*i).first;
        if ((*j).first != 1) oss << ", Components: " << (*j).first;
        oss << ", Type: " << (*k).first << std::endl;
      }
    }
  }
  return oss.str();
}
//--------------------------------------------------------------------

