/*=========================================================================

  Program:   vv
  Module:    $RCSfile: clitkImageToImageGenericFilter.txx,v $
  Language:  C++
  Date:      $Date: 2010/02/10 14:59:49 $
  Version:   $Revision: 1.5 $
  Author :   Joel Schaerer <joel.schaerer@creatis.insa-lyon.fr>
             David Sarrut <david.sarrut@creatis.insa-lyon.fr>

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

//--------------------------------------------------------------------
template<class ImageType> 
void clitk::ImageToImageGenericFilterBase::SetNextOutput(typename ImageType::Pointer output) {
  if (mOutputFilenames.size())
    {
      clitk::writeImage<ImageType>(output, mOutputFilenames.front(), mIOVerbose);
      mOutputFilenames.pop_front();
    }
  if (mInputVVImages.size()) //We assume that if a vv image is set as input, we want one as the output
    mOutputVVImages.push_back(vvImageFromITK<ImageType::ImageDimension,typename ImageType::PixelType>(output));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
#ifdef _MSC_VER
#  pragma warning(disable: 4715) //Disable "The specified function can potentially not return a value"
#endif
template<class ImageType> 
typename ImageType::Pointer clitk::ImageToImageGenericFilterBase::GetInput(unsigned int n) {
  if (mInputFilenames.size() > n) {
    return clitk::readImage<ImageType>(mInputFilenames[n], mIOVerbose);
  }
  else if (mInputVVImages.size() > n)
    return typename ImageType::Pointer(const_cast<ImageType*>(vvImageToITK<ImageType>(mInputVVImages[n]).GetPointer()));
  else
    assert(false); //No input, this shouldn't happen
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
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
template<class FilterType>
bool clitk::ImageToImageGenericFilter<FilterType>::CheckImageType(unsigned int dim,unsigned int ncomp, std::string pixeltype)
{
    return static_cast<bool>(mImageTypesManager.mMapOfImageTypeToFunction[dim][ncomp][pixeltype]);
}

template<class FilterType>
bool clitk::ImageToImageGenericFilter<FilterType>::CheckImageType()
{
    return static_cast<bool>(mImageTypesManager.mMapOfImageTypeToFunction[mDim][mNbOfComponents][mPixelTypeName]);
}

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
                oss << "Dim: " << (*i).first 
                    << ", Components: " << (*j).first 
                    << ", Type: " << (*k).first << std::endl;
            }
        }
    }
    return oss.str();
}
//--------------------------------------------------------------------

