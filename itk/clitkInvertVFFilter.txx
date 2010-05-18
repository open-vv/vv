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
#ifndef __clitkInvertVFFilter_txx
#define __clitkInvertVFFilter_txx
namespace
{

//=========================================================================================================================
//helper class 1 to allow a threaded execution: add contributions of input to output and update weights
//=========================================================================================================================
template<class InputImageType, class OutputImageType> class ITK_EXPORT HelperClass1 : public itk::ImageToImageFilter<InputImageType, OutputImageType>
{

public:
  /** Standard class typedefs. */
  typedef HelperClass1  Self;
  typedef itk::ImageToImageFilter<InputImageType,OutputImageType> Superclass;
  typedef itk::SmartPointer<Self>         Pointer;
  typedef itk::SmartPointer<const Self>   ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( HelperClass1, ImageToImageFilter );

  /** Constants for the image dimensions */
  itkStaticConstMacro(ImageDimension, unsigned int,InputImageType::ImageDimension);


  //Typedefs
  typedef typename OutputImageType::PixelType        PixelType;
  typedef itk::Image<double, ImageDimension > WeightsImageType;
  typedef itk::Image<itk::SimpleFastMutexLock, ImageDimension > MutexImageType;

  //===================================================================================
  //Set methods
  void SetWeights(const typename WeightsImageType::Pointer input) {
    m_Weights = input;
    this->Modified();
  }
  void SetMutexImage(const typename MutexImageType::Pointer input) {
    m_MutexImage=input;
    this->Modified();
    m_ThreadSafe=true;
  }

  //Get methods
  typename  WeightsImageType::Pointer GetWeights() {
    return m_Weights;
  }

  /** Typedef to describe the output image region type. */
  typedef typename OutputImageType::RegionType OutputImageRegionType;

protected:
  HelperClass1();
  ~HelperClass1() {};

  //the actual processing
  void BeforeThreadedGenerateData();
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

  //member data
  typename  WeightsImageType::Pointer m_Weights;
  typename MutexImageType::Pointer m_MutexImage;
  bool m_ThreadSafe;

};



//=========================================================================================================================
//Member functions of the helper class 1
//=========================================================================================================================


//=========================================================================================================================
//Empty constructor
template<class InputImageType, class OutputImageType >
HelperClass1<InputImageType, OutputImageType>::HelperClass1()
{
  m_ThreadSafe=false;
}

//=========================================================================================================================
//Before threaded data
template<class InputImageType, class OutputImageType >
void HelperClass1<InputImageType, OutputImageType>::BeforeThreadedGenerateData()
{
  //Since we will add, put to zero!
  this->GetOutput()->FillBuffer(itk::NumericTraits<double>::Zero);
  this->GetWeights()->FillBuffer(itk::NumericTraits<double>::Zero);
}

//=========================================================================================================================
//update the output for the outputRegionForThread
template<class InputImageType, class OutputImageType>
void HelperClass1<InputImageType, OutputImageType>::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId )
{

  //Get pointer to the input
  typename InputImageType::ConstPointer inputPtr = this->GetInput();

  //Get pointer to the output
  typename OutputImageType::Pointer outputPtr = this->GetOutput();
  typename OutputImageType::SizeType size=outputPtr->GetLargestPossibleRegion().GetSize();

  //Iterator over input
  typedef itk::ImageRegionConstIteratorWithIndex<InputImageType> InputImageIteratorType;

  //define them over the outputRegionForThread
  InputImageIteratorType inputIt(inputPtr, outputRegionForThread);

  //Initialize
  typename InputImageType::IndexType index;
  itk::ContinuousIndex<double,ImageDimension> contIndex;
  typename InputImageType::PointType ipoint;
  typename OutputImageType::PointType opoint;
  typedef typename OutputImageType::PixelType DisplacementType;
  DisplacementType displacement;
  inputIt.GoToBegin();

  //define some temp variables
  signed long baseIndex[ImageDimension];
  double distance[ImageDimension];
  unsigned int dim, counter, upper;
  double totalOverlap,overlap;
  typename OutputImageType::IndexType neighIndex;

  //Find the number of neighbors
  unsigned int neighbors =  1 << ImageDimension;

  //==================================================================================================
  //Loop over the region and add the intensities to the output and the weight to the weights
  //==================================================================================================
  while( !inputIt.IsAtEnd() ) {
    // get the input image index
    index = inputIt.GetIndex();
    inputPtr->TransformIndexToPhysicalPoint( index,ipoint );

    // get the required displacement
    displacement = inputIt.Get();

    // compute the required output image point
    for(unsigned int j = 0; j < ImageDimension; j++ ) opoint[j] = ipoint[j] + (double)displacement[j];

    // Update the output and the weights
    if(outputPtr->TransformPhysicalPointToContinuousIndex(opoint, contIndex ) ) {
      for(dim = 0; dim < ImageDimension; dim++) {
        // The following  block is equivalent to the following line without
        // having to call floor. (Only for positive inputs, we already now that is in the image)
        // baseIndex[dim] = (long) vcl_floor(contIndex[dim] );

        baseIndex[dim] = (long) contIndex[dim];
        distance[dim] = contIndex[dim] - double( baseIndex[dim] );
      }

      //Add contribution for each neighbor
      totalOverlap = itk::NumericTraits<double>::Zero;
      for( counter = 0; counter < neighbors ; counter++ ) {
        overlap = 1.0;          // fraction overlap
        upper = counter;  // each bit indicates upper/lower neighbour

        // get neighbor index and overlap fraction
        for( dim = 0; dim < 3; dim++ ) {
          if ( upper & 1 ) {
            neighIndex[dim] = baseIndex[dim] + 1;
            overlap *= distance[dim];
          } else {
            neighIndex[dim] = baseIndex[dim];
            overlap *= 1.0 - distance[dim];
          }
          upper >>= 1;
        }



        //Set neighbor value only if overlap is not zero
        if( (overlap>0.0)) // &&
          // 			(static_cast<unsigned int>(neighIndex[0])<size[0]) &&
          // 			(static_cast<unsigned int>(neighIndex[1])<size[1]) &&
          // 			(static_cast<unsigned int>(neighIndex[2])<size[2]) &&
          // 			(neighIndex[0]>=0) &&
          // 			(neighIndex[1]>=0) &&
          // 			(neighIndex[2]>=0) )
        {
          //what to store? the original displacement vector?
          if (! m_ThreadSafe) {
            //Set the pixel and weight at neighIndex
            outputPtr->SetPixel(neighIndex, outputPtr->GetPixel(neighIndex) - (displacement*overlap));
            m_Weights->SetPixel(neighIndex, m_Weights->GetPixel(neighIndex) + overlap);
          }

          else {
            //Entering critilal section: shared memory
            m_MutexImage->GetPixel(neighIndex).Lock();

            //Set the pixel and weight at neighIndex
            outputPtr->SetPixel(neighIndex, outputPtr->GetPixel(neighIndex) - (displacement*overlap));
            m_Weights->SetPixel(neighIndex, m_Weights->GetPixel(neighIndex) + overlap);

            //Unlock
            m_MutexImage->GetPixel(neighIndex).Unlock();

          }
          //Add to total overlap
          totalOverlap += overlap;
        }

        if( totalOverlap == 1.0 ) {
          // finished
          break;
        }
      }
    }

    ++inputIt;
  }

}



//=========================================================================================================================
//helper class 2 to allow a threaded execution of normalisation by the weights
//=========================================================================================================================
template<class InputImageType, class OutputImageType> class HelperClass2 : public itk::ImageToImageFilter<InputImageType, OutputImageType>
{

public:
  /** Standard class typedefs. */
  typedef HelperClass2  Self;
  typedef itk::ImageToImageFilter<InputImageType,OutputImageType> Superclass;
  typedef itk::SmartPointer<Self>         Pointer;
  typedef itk::SmartPointer<const Self>   ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( HelperClass2, ImageToImageFilter );

  /** Constants for the image dimensions */
  itkStaticConstMacro(ImageDimension, unsigned int,InputImageType::ImageDimension);

  //Typedefs
  typedef typename OutputImageType::PixelType        PixelType;
  typedef itk::Image<double,ImageDimension> WeightsImageType;

  //Set methods
  void SetWeights(const typename WeightsImageType::Pointer input) {
    m_Weights = input;
    this->Modified();
  }
  void SetEdgePaddingValue(PixelType value) {
    m_EdgePaddingValue = value;
    this->Modified();
  }

  /** Typedef to describe the output image region type. */
  typedef typename OutputImageType::RegionType OutputImageRegionType;

protected:
  HelperClass2();
  ~HelperClass2() {};


  //the actual processing
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );


  //member data
  typename     WeightsImageType::Pointer m_Weights;
  PixelType m_EdgePaddingValue;

} ;



//=========================================================================================================================
//Member functions of the helper class 2
//=========================================================================================================================


//=========================================================================================================================
//Empty constructor
template<class InputImageType, class OutputImageType > HelperClass2<InputImageType, OutputImageType>::HelperClass2()
{
  m_EdgePaddingValue=itk::NumericTraits<PixelType>::Zero;
}


//=========================================================================================================================
//update the output for the outputRegionForThread
template<class InputImageType, class OutputImageType > void HelperClass2<InputImageType, OutputImageType>::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId )
{

  //Get pointer to the input
  typename InputImageType::ConstPointer inputPtr = this->GetInput();

  //Get pointer to the output
  typename OutputImageType::Pointer outputPtr = this->GetOutput();

  //Iterators over input, weigths  and output
  typedef itk::ImageRegionConstIterator<InputImageType> InputImageIteratorType;
  typedef itk::ImageRegionIterator<OutputImageType> OutputImageIteratorType;
  typedef itk::ImageRegionIterator<WeightsImageType> WeightsImageIteratorType;

  //define them over the outputRegionForThread
  OutputImageIteratorType outputIt(outputPtr, outputRegionForThread);
  InputImageIteratorType inputIt(inputPtr, outputRegionForThread);
  WeightsImageIteratorType weightsIt(m_Weights, outputRegionForThread);


  //==================================================================================================
  //loop over the output and normalize the input, remove holes
  PixelType neighValue;
  double  zero = itk::NumericTraits<double>::Zero;
  while (!outputIt.IsAtEnd()) {
    //the weight is not zero
    if (weightsIt.Get() != zero) {
      //divide by the weight
      outputIt.Set(static_cast<PixelType>(inputIt.Get()/weightsIt.Get()));
    }

    //copy the value of the  neighbour that was just processed
    else {
      if(!outputIt.IsAtBegin()) {
        //go back
        --outputIt;

        //Neighbour cannot have zero weight because it should be filled already
        neighValue=outputIt.Get();
        ++outputIt;
        outputIt.Set(neighValue);
        //DD("hole filled");
      } else {
        //DD("is at begin, setting edgepadding value");
        outputIt.Set(m_EdgePaddingValue);
      }
    }
    ++weightsIt;
    ++outputIt;
    ++inputIt;

  }//end while
}//end member


}//end nameless namespace



namespace clitk
{

//=========================================================================================================================
// The rest is the InvertVFFilter
//=========================================================================================================================

//=========================================================================================================================
//constructor
template <class InputImageType, class OutputImageType>
InvertVFFilter<InputImageType, OutputImageType>::InvertVFFilter()
{
  m_EdgePaddingValue=itk::NumericTraits<PixelType>::Zero; //no other reasonable value?
  m_ThreadSafe=false;
  m_Verbose=false;
}


//=========================================================================================================================
//Update
template <class InputImageType, class OutputImageType> void InvertVFFilter<InputImageType, OutputImageType>::GenerateData()
{

  //Get the properties of the input
  typename InputImageType::ConstPointer inputPtr=this->GetInput();
  typename WeightsImageType::RegionType region;
  typename WeightsImageType::RegionType::SizeType size=inputPtr->GetLargestPossibleRegion().GetSize();
  region.SetSize(size);
  typename OutputImageType::IndexType start;
  for (unsigned int i=0; i< ImageDimension; i++) start[i]=0;
  region.SetIndex(start);
  PixelType zero = itk::NumericTraits<double>::Zero;


  //Allocate the weights
  typename WeightsImageType::Pointer weights=WeightsImageType::New();
  weights->SetRegions(region);
  weights->Allocate();
  weights->SetSpacing(inputPtr->GetSpacing());

  //===========================================================================
  //Inversion is divided in in two loops, for each we will call a threaded helper class
  //1. add contribution of input to output and update weights
  //2. normalize the output by the weight and remove holes
  //===========================================================================


  //===========================================================================
  //1. add contribution of input to output and update weights

  //Define an internal image type

  typedef itk::Image<itk::Vector<double,ImageDimension>, ImageDimension > InternalImageType;

  //Call threaded helper class 1
  typedef HelperClass1<InputImageType, InternalImageType > HelperClass1Type;
  typename HelperClass1Type::Pointer helper1=HelperClass1Type::New();

  //Set input
  if(m_NumberOfThreadsIsGiven)helper1->SetNumberOfThreads(m_NumberOfThreads);
  helper1->SetInput(inputPtr);
  helper1->SetWeights(weights);

  //Threadsafe?
  if(m_ThreadSafe) {
    //Allocate the mutex image
    typename MutexImageType::Pointer mutex=InvertVFFilter::MutexImageType::New();
    mutex->SetRegions(region);
    mutex->Allocate();
    mutex->SetSpacing(inputPtr->GetSpacing());
    helper1->SetMutexImage(mutex);
    if (m_Verbose) std::cout <<"Inverting using a thread-safe algorithm" <<std::endl;
  } else  if(m_Verbose)std::cout <<"Inverting using a thread-unsafe algorithm" <<std::endl;

  //Execute helper class
  helper1->Update();

  //Get the output
  typename InternalImageType::Pointer temp= helper1->GetOutput();
  weights=helper1->GetWeights();


  //===========================================================================
  //2. Normalize the output by the weights and remove holes
  //Call threaded helper class
  typedef HelperClass2<InternalImageType, OutputImageType> HelperClass2Type;
  typename HelperClass2Type::Pointer helper2=HelperClass2Type::New();

  //Set temporary output as input
  helper2->SetInput(temp);
  helper2->SetWeights(weights);
  helper2->SetEdgePaddingValue(m_EdgePaddingValue);

  //Execute helper class
  if (m_Verbose) std::cout << "Normalizing the output VF..."<<std::endl;
  helper2->Update();

  //Set the output
  this->SetNthOutput(0, helper2->GetOutput());
}



}

#endif
