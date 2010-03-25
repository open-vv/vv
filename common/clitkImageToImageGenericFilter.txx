template<class FilterType>
clitk::ImageToImageGenericFilter<FilterType>::ImageToImageGenericFilter(std::string filterName) :
  ImageToImageGenericFilterBase(filterName), 
  mImageTypesManager(static_cast<FilterType*>(this))
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class FilterType>
bool clitk::ImageToImageGenericFilter<FilterType>::Update() {
  GetInputImageDimensionAndPixelType(mDim,mPixelTypeName,mNbOfComponents);    
  
  // Check ImageType
  if (!CheckImageType()) {
    if (mFailOnImageTypeError) ImageTypeError();
    else SetImageTypeError();
    return false;
  }

  // Go ! Call the right templatized function
  mImageTypesManager.DoIt(mDim, mNbOfComponents, mPixelTypeName);
  return true;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class FilterType>
bool clitk::ImageToImageGenericFilter<FilterType>::CheckImageType(unsigned int dim,unsigned int ncomp, std::string pixeltype)
{
  return static_cast<bool>(mImageTypesManager.mMapOfImageTypeToFunction[dim][ncomp][pixeltype]);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class FilterType>
bool clitk::ImageToImageGenericFilter<FilterType>::CheckImageType()
{
  return static_cast<bool>(mImageTypesManager.mMapOfImageTypeToFunction[mDim][mNbOfComponents][mPixelTypeName]);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class FilterType>
std::string clitk::ImageToImageGenericFilter<FilterType>::GetAvailableImageTypes() {
  std::ostringstream oss;
  oss << "The filter <" << mFilterName << "> manages:" << std::endl;
    
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

