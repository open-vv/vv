/*=========================================================================

  Program:   vv
  Module:    $RCSfile: clitkImageToImageGenericFilterBase.txx,v $
  Language:  C++
  Date:      $Date: 2010/03/02 13:02:52 $
  Version:   $Revision: 1.1 $
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

// //--------------------------------------------------------------------
// template<class ImageType> 
// void clitk::ImageToImageGenericFilterBase::SetNextOutput(typename ImageType::Pointer output) {
//   if (mOutputFilenames.size())
//     {
//       clitk::writeImage<ImageType>(output, mOutputFilenames.front(), mIOVerbose);
//       mOutputFilenames.pop_front();
//     }
//   if (mInputVVImages.size()) //We assume that if a vv image is set as input, we want one as the output
//     mOutputVVImages.push_back(vvImageFromITK<ImageType::ImageDimension,typename ImageType::PixelType>(output));
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// template<class ImageType> 
// typename ImageType::Pointer clitk::ImageToImageGenericFilterBase::GetInput(unsigned int n) {
//   if (mInputFilenames.size() > n) {
//     return clitk::readImage<ImageType>(mInputFilenames[n], mIOVerbose);
//   }
//   else if (mInputVVImages.size() > n)
//     return typename ImageType::Pointer(const_cast<ImageType*>(vvImageToITK<ImageType>(mInputVVImages[n]).GetPointer()));
//   else
//     {
//       assert(false); //No input, this shouldn't happen
//       return typename ImageType::Pointer(NULL);
//     }
// }
// //--------------------------------------------------------------------


