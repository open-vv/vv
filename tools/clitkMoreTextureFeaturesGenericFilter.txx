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
#ifndef clitkMoreTextureFeaturesGenericFilter_txx
#define clitkMoreTextureFeaturesGenericFilter_txx

// itk include
#include "itkScalarImageToCooccurrenceMatrixFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include <clitkCommon.h>
//std
#include <math.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
MoreTextureFeaturesGenericFilter<args_info_type>::MoreTextureFeaturesGenericFilter():
    ImageToImageGenericFilter<Self>("MoreTextureFeatures")
{
    InitializeImageType<2>();
    InitializeImageType<3>();
    InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void MoreTextureFeaturesGenericFilter<args_info_type>::InitializeImageType()
{
    ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void MoreTextureFeaturesGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
    mArgsInfo=a;
    this->m_Verbose = mArgsInfo.verbose_flag;
    this->SetIOVerbose(mArgsInfo.verbose_flag);
    if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

    if (mArgsInfo.input_given) {
        this->SetInputFilename(mArgsInfo.input_arg);
    }
    if (mArgsInfo.mask_given) {
        this->AddInputFilename(mArgsInfo.mask_arg);
    }
    if (mArgsInfo.nbin_given) {
        this->m_NBin = mArgsInfo.nbin_arg;
    } else {
        this->m_NBin = 256;
    }
    if (mArgsInfo.minr_given) {
        this->m_minRange = mArgsInfo.minr_arg;
    } else {
        this->m_minRange = -1;
    }
    if (mArgsInfo.maxr_given) {
        this->m_maxRange = mArgsInfo.maxr_arg;
    } else {
        this->m_maxRange = -1;
    }
    if (mArgsInfo.output_given) {
      this->SetOutputFilename(mArgsInfo.output_arg);
    }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
MoreTextureFeaturesGenericFilter<args_info_type>::UpdateWithInputImageType()
{
    // Reading inputs
    const unsigned int Dimension = InputImageType::ImageDimension;
    typedef itk::Image<double, Dimension> OutputImageType;
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
    typename OutputImageType::Pointer maskImage = NULL;
    if (mArgsInfo.mask_given) {
        maskImage = this->template GetInput<OutputImageType>(1);
    } else {
        maskImage = OutputImageType::New();
        typename OutputImageType::IndexType start;
        for(unsigned i=0;i<OutputImageType::ImageDimension;i++) {
            start[i] = 0;  // first index on X
        }
        typename OutputImageType::SizeType  size;
        for(unsigned i=0;i<OutputImageType::ImageDimension;i++) {
            size[i] = input->GetLargestPossibleRegion().GetSize()[i];  // size along i
        }
        typename OutputImageType::RegionType region;
        region.SetSize(size);
        region.SetIndex(start);
        maskImage->SetRegions(region);
        maskImage->SetOrigin(input->GetOrigin());
        maskImage->SetSpacing(input->GetSpacing());
        maskImage->SetDirection(input->GetDirection());
        maskImage->Allocate();
        maskImage->FillBuffer(1);
    }
    typedef itk::MaskImageFilter< InputImageType, OutputImageType, OutputImageType > MaskFilterType;
    typename MaskFilterType::Pointer maskFilter = MaskFilterType::New();
    maskFilter->SetInput(input);
    maskFilter->SetOutsideValue(-itk::NumericTraits< double >::max());
    maskFilter->SetMaskImage(maskImage);
    //Find the max first
    typedef itk::MinimumMaximumImageFilter<OutputImageType> MinimumMaximumImageFilterType;
    typename MinimumMaximumImageFilterType::Pointer maxFilter=MinimumMaximumImageFilterType::New();
    maxFilter->SetInput(maskFilter->GetOutput());
    maxFilter->Update();
    typename OutputImageType::PixelType maxImg = maxFilter->GetMaximum();
    //
    typedef itk::MaskImageFilter< InputImageType, OutputImageType, OutputImageType > MaskFilterType;
    typename MaskFilterType::Pointer maskFilter2 = MaskFilterType::New();
    maskFilter2->SetInput(input);
    maskFilter2->SetOutsideValue(itk::NumericTraits< double >::max());
    maskFilter2->SetMaskImage(maskImage);
    //Find the min
    typename MinimumMaximumImageFilterType::Pointer minFilter=MinimumMaximumImageFilterType::New();
    minFilter->SetInput(maskFilter2->GetOutput());
    minFilter->Update();
    typename OutputImageType::PixelType minImg = minFilter->GetMinimum();
    if(m_Verbose) {
        std::cout<<"Min of input image = "<<minImg<<std::endl;
        std::cout<<"Max of input image = "<<maxImg<<std::endl;
    }
    if(this->m_minRange == -1) {
        this->m_minRange = std::floor(minImg*10000.0)/10000.0-1.0/10000.0;
    } else {
        if(minImg <= this->m_minRange) {
            itk::ImageRegionIterator<InputImageType> itII(input,input->GetLargestPossibleRegion());
            itII.GoToBegin();
            while(!itII.IsAtEnd()) {
                if(itII.Get() <= this->m_minRange) {
                    itII.Set(this->m_minRange+1.0/10000.0);
                    //itII.Set(this->m_minRange);
                }
                ++itII;
            }
            //this->m_minRange = this->m_minRange-1.0/10000.0;
        }
    }
    if(this->m_maxRange == -1) {
        this->m_maxRange = std::ceil(maxImg*10000.0)/10000.0+1.0/10000.0;
    } else {
        if(maxImg >= this->m_maxRange) {
            itk::ImageRegionIterator<InputImageType> itII(input,input->GetLargestPossibleRegion());
            itII.GoToBegin();
            while(!itII.IsAtEnd()) {
                if(itII.Get() >= this->m_maxRange) {
                    itII.Set(this->m_maxRange-1.0/10000.0);
                    //itII.Set(this->m_maxRange);
                }
                ++itII;
            }
            //this->m_maxRange = this->m_maxRange+1.0/10000.0;
        }
    }
    if(m_Verbose) {
        std::cout<<"Min range is set to "<<this->m_minRange<<std::endl;
        std::cout<<"Max range is set to "<<this->m_maxRange<<std::endl;
    }
    //Before using ScalarImageToCooccurrenceMatrixFilter, let's do the rescaling + rounding
    typedef itk::SubtractImageFilter <InputImageType, OutputImageType, OutputImageType> SubtractImageFilterType;
    typename SubtractImageFilterType::Pointer subtractConstantFromImageFilter = SubtractImageFilterType::New();
    subtractConstantFromImageFilter->SetInput(input);
    subtractConstantFromImageFilter->SetConstant2(this->m_minRange);
    //Let's do the multiplication
    typedef itk::MultiplyImageFilter<OutputImageType, OutputImageType, OutputImageType> MultiplyImageFilterType;
    typename MultiplyImageFilterType::Pointer multiplytFilter = MultiplyImageFilterType::New();
    multiplytFilter->SetInput(subtractConstantFromImageFilter->GetOutput());
    multiplytFilter->SetConstant((double) (this->m_NBin) / (this->m_maxRange - this->m_minRange));
    //Let's round the result - floor
    multiplytFilter->Update();
    typename OutputImageType::Pointer floorImg = multiplytFilter->GetOutput();
    itk::ImageRegionIterator<OutputImageType> itSI(floorImg,floorImg->GetLargestPossibleRegion());
    itSI.GoToBegin();
    while(!itSI.IsAtEnd()) {
      itSI.Set(std::floor(itSI.Get()));
      ++itSI;
    }
    // Save the floorImg if output given
    if(mArgsInfo.output_given) {
      this->template SetNextOutput<OutputImageType>(floorImg);
    }
    // Main filter
    typedef itk::Statistics::ScalarImageToCooccurrenceMatrixFilter<OutputImageType> ScalarImageToCooccurrenceMatrixFilterType;
    typedef typename InputImageType::OffsetType OffsetType;
    OffsetType offset;
    offset.Fill(0);
    double mean_correlation = 0;
    double std_correlation = 0;
    double mean_homogeneity = 0;
    double std_homogeneity = 0;
    double mean_dissimilarity = 0;
    double std_dissimilarity = 0;
    double** sumMatrix = new double*[this->m_NBin];
    for(int i=0; i<this->m_NBin; ++i) {
        sumMatrix[i]=new double[this->m_NBin];
    }
    //Init sumMatrix
    for(int i = 0; i < this->m_NBin; ++i) {
        for(int j = 0; j < this->m_NBin; ++j) {
            sumMatrix[i][j]=0;
        }
    }
    double energy_sumMatrix = 0;
    double entropy_sumMatrix = 0;
    double inverseDifferenceMoment_sumMatrix = 0;
    double inertia_sumMatrix = 0;
    double clusterShade_sumMatrix = 0;
    double clusterProminence_sumMatrix = 0;
    double correlation_sumMatrix = 0;
    double dissimilarity_sumMatrix = 0;
    double homogeneity_sumMatrix = 0;
    if (Dimension == 2) { //4 directions
        const unsigned int nd = 4;
        double*** matrix = new double**[nd];
        for(unsigned i = 0; i < nd; ++i) {
            matrix[i] = new double*[this->m_NBin];
            for(int j = 0; j < this->m_NBin; ++j)
                matrix[i][j] = new double[this->m_NBin];
        }
        unsigned count[nd] = {};
        double correlation[nd] = {};
        double homogeneity[nd] = {};
        double dissimilarity[nd] = {};
        int list_dir_x[nd] = {1, 1, 0, -1};
        int list_dir_y[nd] = {0, 1, 1,  1};
        for(unsigned int d=0; d<nd; d++) {
            typename ScalarImageToCooccurrenceMatrixFilterType::Pointer glcmGenerator=ScalarImageToCooccurrenceMatrixFilterType::New();
            glcmGenerator->SetInput(floorImg);
            if (mArgsInfo.mask_given) {
                glcmGenerator->SetMaskImage(maskImage);
            }
            // Set filter members
            offset[0]=list_dir_x[d];
            offset[1]=list_dir_y[d];
            glcmGenerator->SetOffset(offset);
            if(this->m_Verbose) {
                std::cout<<"The offset is set to "<<offset<<std::endl;
            }
            glcmGenerator->SetNumberOfBinsPerAxis(this->m_NBin); //reasonable number of bins
            glcmGenerator->SetPixelValueMinMax(0, this->m_NBin-1); //for input UCHAR pixel type
            glcmGenerator->Update();
            for(unsigned int i = 0; i < glcmGenerator->GetOutput()->GetSize()[0]; ++i) {
                for(unsigned int j = 0; j < glcmGenerator->GetOutput()->GetSize()[1]; ++j) {
                    matrix[d][i][j]=glcmGenerator->GetOutput()->GetFrequency(j+i*glcmGenerator->GetOutput()->GetSize()[0]);
                    count[d]+=matrix[d][i][j];
                    sumMatrix[i][j]+=matrix[d][i][j];
                }
            }
            if(this->m_Verbose) {
                std::cout<<"count[d] = "<< count[d] <<std::endl;
            }
            for(int i = 0; i < this->m_NBin; ++i) {
                for(int j = 0; j < this->m_NBin; ++j) {
                    matrix[d][i][j]=matrix[d][i][j]/count[d];
                }
            }
            if(this->m_Verbose) {
                std::cout << "Frequency = ["<<std::endl;
                for(int i = 0; i < this->m_NBin; ++i) {
                    for(int j = 0; j < this->m_NBin; ++j) {
                        std::cout << matrix[d][i][j];
                        if( j != this->m_NBin - 1 ) {
                            std::cout << ",";
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << "]" << std::endl;
            }
            //Let's calculate the TFs in each direction
            //Index calculation - works only for symetric matrices !
            double sumI = 0;
            double stdI = 0;
            for(int i = 0; i < this->m_NBin; i++) {
                for(int j = 0; j < this->m_NBin; j++) {
                    sumI+=j*matrix[d][i][j];
                }
            }
            for(int i = 0; i < this->m_NBin; i++) {
                for(int j = 0; j < this->m_NBin; j++) {
                    stdI+=(j-sumI)*(j-sumI)*matrix[d][i][j];
                }
            }
            stdI = std::sqrt(stdI);
            for(int i = 0; i < this->m_NBin; i++) {
                for(int j = 0; j < this->m_NBin; j++) {
                    correlation[d]+=(i-sumI)*(j-sumI)*matrix[d][i][j]/(stdI*stdI);
                    homogeneity[d]+=(matrix[d][i][j])/(1+std::abs((int)i-(int)j));
                    dissimilarity[d]+=std::abs((int)i-(int)j)*(matrix[d][i][j]);
                }
            }
        }
        //Let's calculate the TFs mean + std
        for(unsigned int d=0; d<nd; d++) {
            mean_correlation   += correlation[d];
            mean_homogeneity   += homogeneity[d];
            mean_dissimilarity += dissimilarity[d];
        }
        mean_correlation   = mean_correlation/nd;
        mean_homogeneity   = mean_homogeneity/nd;
        mean_dissimilarity = mean_dissimilarity/nd;
        for(unsigned int d=0; d<nd; d++) {
            std_correlation   += (correlation[d]-mean_correlation)*(correlation[d]-mean_correlation);
            std_homogeneity   += (homogeneity[d]-mean_homogeneity)*(homogeneity[d]-mean_homogeneity);
            std_dissimilarity += (dissimilarity[d]-mean_dissimilarity)*(dissimilarity[d]-mean_dissimilarity);
        }
        std_correlation = std::sqrt(std_correlation/nd);
        std_homogeneity = std::sqrt(std_homogeneity/nd);
        std_dissimilarity = std::sqrt(std_dissimilarity/nd);
        // Clean
        for (unsigned i = 0; i < nd; ++i) {
            for (int j = 0; j < this->m_NBin; ++j)
                delete[] matrix[i][j];
            delete[] matrix[i];
        }
        delete[] matrix;
        matrix=NULL;
    }
    else if (Dimension == 3) { //13 directions
        const unsigned int nd = 13;
        //double matrix[nd][this->m_NBin][this->m_NBin];
        double*** matrix = new double**[nd];
        for(unsigned i = 0; i < nd; ++i) {
            matrix[i] = new double*[this->m_NBin];
            for(int j = 0; j < this->m_NBin; ++j)
                matrix[i][j] = new double[this->m_NBin];
        }
        unsigned count[nd] = {};
        double correlation[nd] = {};
        double homogeneity[nd] = {};
        double dissimilarity[nd] = {};
        int list_dir_x[nd] = {-1, 0, 1, 0, 1, -1, 0, 1, 1, -1,  0,  1,  1};
        int list_dir_y[nd] = { 1, 1, 1, 0, 0,  1, 1, 1, 0,  1,  1,  1,  0};
        int list_dir_z[nd] = { 1, 1, 1, 1, 1,  0, 0, 0, 0, -1, -1, -1, -1};
        for(unsigned int d=0; d<nd; d++) {
            typename ScalarImageToCooccurrenceMatrixFilterType::Pointer glcmGenerator=ScalarImageToCooccurrenceMatrixFilterType::New();
            glcmGenerator->SetInput(floorImg);
            if (mArgsInfo.mask_given) {
                glcmGenerator->SetMaskImage(maskImage);
            }
            // Set filter members
            offset[0]=list_dir_x[d];
            offset[1]=list_dir_y[d];
            offset[2]=list_dir_z[d];
            glcmGenerator->SetOffset(offset);
            if(this->m_Verbose) {
                std::cout<<"The offset is set to "<<offset<<std::endl;
            }
            glcmGenerator->SetNumberOfBinsPerAxis(this->m_NBin); //reasonable number of bins
            glcmGenerator->SetPixelValueMinMax(0, this->m_NBin -1); //for input UCHAR pixel type
            glcmGenerator->Update();
            for(unsigned int i = 0; i < glcmGenerator->GetOutput()->GetSize()[0]; ++i) {
                for(unsigned int j = 0; j < glcmGenerator->GetOutput()->GetSize()[1]; ++j) {
                    matrix[d][i][j]=glcmGenerator->GetOutput()->GetFrequency(j+i*glcmGenerator->GetOutput()->GetSize()[0]);
                    count[d]+=matrix[d][i][j];
                    sumMatrix[i][j]+=matrix[d][i][j];
                }
            }
            if(this->m_Verbose) {
                std::cout<<"count[d] = "<< count[d] <<std::endl;
            }
            for(int i = 0; i < this->m_NBin; ++i) {
                for(int j = 0; j < this->m_NBin; ++j) {
                    matrix[d][i][j]=matrix[d][i][j]/count[d];
                }
            }
            if(this->m_Verbose) {
                std::cout << "Frequency = ["<<std::endl;
                for(int i = 0; i < this->m_NBin; ++i) {
                    for(int j = 0; j < this->m_NBin; ++j) {
                        std::cout << matrix[d][i][j];
                        if( j != this->m_NBin - 1 ) {
                            std::cout << ",";
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << "]" << std::endl;
            }
            //Let's calculate the TFs in each direction
            //Index calculation - works only for symetric matrices !
            double sumI = 0;
            double stdI = 0;
            for(int i = 0; i < this->m_NBin; i++) {
                for(int j = 0; j < this->m_NBin; j++) {
                    sumI+=j*matrix[d][i][j];
                }
            }
            for(int i = 0; i < this->m_NBin; i++) {
                for(int j = 0; j < this->m_NBin; j++) {
                    stdI+=(j-sumI)*(j-sumI)*matrix[d][i][j];
                }
            }
            stdI = std::sqrt(stdI);
            //
            for(int i = 0; i < this->m_NBin; i++) {
                for(int j = 0; j < this->m_NBin; j++) {
                    correlation[d]+=((i-sumI)*(j-sumI)*matrix[d][i][j])/(stdI*stdI);
                    homogeneity[d]+=(matrix[d][i][j])/(1+std::abs((int)i-(int)j));
                    dissimilarity[d]+=std::abs((int)i-(int)j)*(matrix[d][i][j]);
                }
            }
        }
        //Let's calculate the TFs mean + std
        for(unsigned int d=0; d<nd; d++) {
            mean_correlation   += correlation[d];
            mean_homogeneity   += homogeneity[d];
            mean_dissimilarity += dissimilarity[d];
        }
        mean_correlation   = mean_correlation/nd;
        mean_homogeneity   = mean_homogeneity/nd;
        mean_dissimilarity = mean_dissimilarity/nd;
        for(unsigned int d=0; d<nd; d++) {
            std_correlation   += (correlation[d]-mean_correlation)*(correlation[d]-mean_correlation);
            std_homogeneity   += (homogeneity[d]-mean_homogeneity)*(homogeneity[d]-mean_homogeneity);
            std_dissimilarity += (dissimilarity[d]-mean_dissimilarity)*(dissimilarity[d]-mean_dissimilarity);
        }
        std_correlation = std::sqrt(std_correlation/nd);
        std_homogeneity = std::sqrt(std_homogeneity/nd);
        std_dissimilarity = std::sqrt(std_dissimilarity/nd);
        // Clean
        for (unsigned i = 0; i < nd; ++i) {
            for (int j = 0; j < this->m_NBin; ++j)
                delete[] matrix[i][j];
            delete[] matrix[i];
        }
        delete[] matrix;
        matrix=NULL;
    } else {
        std::cerr<<"This tools works only for 2D or 3D images"<<std::endl;
        exit(1);
    }
    //Let's process the sumMatrix
    double sum_sumMatrix = 0;
    for(int i = 0; i < this->m_NBin; ++i) {
        for(int j = 0; j < this->m_NBin; ++j) {
                sum_sumMatrix += sumMatrix[i][j];
        }
    }
    for(int i = 0; i < this->m_NBin; ++i) {
        for(int j = 0; j < this->m_NBin; ++j) {
                sumMatrix[i][j]/=sum_sumMatrix;
        }
    }
    //Let's calculate the TFs of sumMatrix
    //Index calculation - works only for symetric matrices !
    double sumI_sumMatrix = 0;
    double stdI_sumMatrix = 0;
    for(int i = 0; i < this->m_NBin; i++) {
        for(int j = 0; j < this->m_NBin; j++) {
            sumI_sumMatrix+=j*sumMatrix[i][j];
        }
    }
    for(int i = 0; i < this->m_NBin; i++) {
        for(int j = 0; j < this->m_NBin; j++) {
            stdI_sumMatrix+=(j-sumI_sumMatrix)*(j-sumI_sumMatrix)*sumMatrix[i][j];
        }
    }
    stdI_sumMatrix = std::sqrt(stdI_sumMatrix);
    //
    for(int i = 0; i < this->m_NBin; i++) {
        for(int j = 0; j < this->m_NBin; j++) {
            energy_sumMatrix+=(sumMatrix[i][j]*sumMatrix[i][j]);
            entropy_sumMatrix+=(-sumMatrix[i][j]*log(sumMatrix[i][j]+std::numeric_limits<double>::epsilon()));
            inverseDifferenceMoment_sumMatrix+=(sumMatrix[i][j]/(1+(i-j)*(i-j)));
            inertia_sumMatrix+=((i-j)*(i-j)*sumMatrix[i][j]);
            clusterShade_sumMatrix+=((i+j-sumI_sumMatrix-sumI_sumMatrix)*(i+j-sumI_sumMatrix-sumI_sumMatrix)*(i+j-sumI_sumMatrix-sumI_sumMatrix)*sumMatrix[i][j]);
            clusterProminence_sumMatrix+=((i+j-sumI_sumMatrix-sumI_sumMatrix)*(i+j-sumI_sumMatrix-sumI_sumMatrix)*(i+j-sumI_sumMatrix-sumI_sumMatrix)*(i+j-sumI_sumMatrix-sumI_sumMatrix)*sumMatrix[i][j]);
            correlation_sumMatrix+=((i-sumI_sumMatrix)*(j-sumI_sumMatrix)*sumMatrix[i][j])/(stdI_sumMatrix*stdI_sumMatrix);
            dissimilarity_sumMatrix+=std::abs((int)i-(int)j)*(sumMatrix[i][j]);
            homogeneity_sumMatrix+=(sumMatrix[i][j])/(1+std::abs((int)i-(int)j));
        }
    }
    // Clean
    for(int i=0; i<this->m_NBin; ++i) {
        delete[] sumMatrix[i];
    }
    delete[] sumMatrix;
    sumMatrix = NULL;
    // Print the values
    std::cout << "Correlation: mean = "  << mean_correlation   << " - std = " << std_correlation   <<std::endl;
    std::cout << "Dissimilarity: mean = "<< mean_dissimilarity << " - std = " << std_dissimilarity <<std::endl;
    std::cout << "Homogeneity: mean = "  << mean_homogeneity   << " - std = " << std_homogeneity   <<std::endl;
    std::cout << "Energy1 = "                   << energy_sumMatrix                       <<std::endl;
    std::cout << "Entropy1 = "                  << entropy_sumMatrix                      <<std::endl;
    std::cout << "InverseDifferenceMoment1 = "  << inverseDifferenceMoment_sumMatrix      <<std::endl;
    std::cout << "Inertia1 = "                  << inertia_sumMatrix                      <<std::endl;
    std::cout << "ClusterShade1 = "             << clusterShade_sumMatrix                 <<std::endl;
    std::cout << "ClusterProminence1 = "        << clusterProminence_sumMatrix            <<std::endl;
    std::cout << "Correlation1 = "              << correlation_sumMatrix                  <<std::endl;
    std::cout << "Dissimilarity1 = "            << dissimilarity_sumMatrix                <<std::endl;
    std::cout << "Homogeneity1 = "              << homogeneity_sumMatrix                  <<std::endl;
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkMoreTextureFeaturesGenericFilter_txx
