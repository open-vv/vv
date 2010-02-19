#ifndef CLITKSIGNAL_CXX
#define CLITKSIGNAL_CXX

#include "clitkSignal.h"

namespace clitk {

  //---------------------------------------------------------------------
  void Signal::Read(string fileName){
    ifstream signalStream(fileName.c_str());
    SignalValueType point;
    if(!signalStream.is_open()){
      std::cerr << "ERROR: Couldn't open file " << fileName << " in Signal::Read" << std::endl;
      return;
    }
    skipComment(signalStream);
    while(!signalStream.eof()) {
      skipComment(signalStream);
      signalStream >> point;
      skipComment(signalStream);
      m_Data.push_back(point);
      //      cout << point << endl;
    }
    signalStream.close();
    SetSamplingPeriod(1.);
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  void Signal::Read(string fileName, int col){
    ifstream signalStream(fileName.c_str());
    SignalValueType point;
    if(!signalStream.is_open()){
      std::cerr << "ERROR: Couldn't open file " << fileName << " in Signal::Read" << std::endl;
      return;
    }
    skipComment(signalStream);
    while(!signalStream.eof()) {
      skipComment(signalStream);

      // Read one line
      std::string line;
      std::getline(signalStream, line);
        
      // Get column nb col
      istringstream iss(line);
      for(int i=0; i<col; i++) { // skip col-1 first column
        string sub;
        iss >> sub;
      }
      iss >> point;
      if (!iss) { 
        std::cerr << "ERROR: no col n" << col << " in the line '" << line << "' ?" << std::endl;
        exit(0);
      }
      skipComment(signalStream);
      m_Data.push_back(point);
      //      cout << point << endl;
    }
    signalStream.close();
    SetSamplingPeriod(1.);
  }
  //---------------------------------------------------------------------

  
  
  //---------------------------------------------------------------------
  //Convert 1D image to signal
  Signal Signal::ConvertImageToSignal( Signal::ImageType::Pointer image)
  {
    //empty signal
  Signal signal;

  //make an image iterator
  itk::ImageRegionConstIterator<ImageType> it(image,image->GetLargestPossibleRegion());
  it.Begin();

  //copy
  while(!it.IsAtEnd())
    {
      signal.push_back(it.Get());
      ++it;
    }

  //Spacing
  signal.SetSamplingPeriod(image->GetSpacing()[0]);

  return signal;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  //Convert 1D signal to image
  Signal::ImageType::Pointer Signal::ConvertSignalToImage(Signal signal)
  {
  //empty image
  ImageType::Pointer image =ImageType::New();
  ImageType::RegionType region;
  ImageType::RegionType::IndexType index;
  index[0]=0;
  ImageType::RegionType::SizeType size;
  size[0]=signal.size();

  region.SetIndex(index);
  region.SetSize(size);

  image->SetRegions(region);
  image->Allocate();

  //make an image iterator
  itk::ImageRegionIterator<ImageType> mIt(image,image->GetLargestPossibleRegion());
  mIt.Begin();

  //make a signal iterator
  Signal::const_iterator sIt=signal.begin();

  //copy
  while(sIt!=signal.end())
    {
      mIt.Set(*sIt);
      sIt++;++mIt;
    }
  
  
  //spacing
  ImageType::SpacingType spacing;
  spacing[0]=signal.GetSamplingPeriod();
  image->SetSpacing(spacing);
  
  return image;
  
  }
  //---------------------------------------------------------------------



  //---------------------------------------------------------------------
  void Signal::Write(const string fileName){
    ofstream signalStream(fileName.c_str());
    if(!signalStream.is_open()){
      cerr << "ERROR: Couldn't open file " << fileName << " in Signal::Write" << endl;
      return;
    }

    iterator it=begin();
    while(it!=end()) {
      signalStream << *it << endl;      
      it++;
    }
    signalStream.close();
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal & Signal::operator/=(Signal & div){
    if(size()!= div.size())
      {
	std::cerr << "Error: signal size must be the same!" << std::endl;
	return  *this;
      }
    iterator it=begin();
    iterator itD;
    itD=div.begin();
    while(it!=end())
      {
	if(*itD!=0)
	  *it/=*itD;
	else
	  cerr << "Division by 0 in operator/= skipped" << endl;
	it++;itD++;
      }
    return *this;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal & Signal::operator*=(Signal & mul){
    if(size()!= mul.size())
      {
	std::cerr << "Error: signal size must be the same!" << std::endl;
	return  *this;
      }
    iterator it=begin();
    iterator itD;
    itD=mul.begin();
    while(it!=end()){
     	*it *= *itD;
	it++;itD++;
    }
    return *this;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::Normalize(double newMin,double newMax){
    Signal temp (m_SamplingPeriod);
    vector<double> extrema=GetGlobalMinMax();
    iterator itSig=begin();
    while(itSig!=end()){
      temp.push_back( ((*itSig)-extrema[0])*(newMax-newMin)/(extrema[1]-extrema[0]) + newMin );
      itSig++;
    }
    return temp;        
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  vector<double> Signal::GetGlobalMinMax() const {
    vector<double> extrema(2);
    if(size()==0){
      cerr << "ERROR: GetExtrema / No signal" << endl;
      return extrema;
    }
    extrema[0]=m_Data[0];
    extrema[1]=m_Data[0];
    for(unsigned int i=1;i<m_Data.size();i++){
      if(extrema[0]>m_Data[i]) extrema[0]=m_Data[i];
      if(extrema[1]<m_Data[i]) extrema[1]=m_Data[i];
    }
    return extrema;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  double Signal::GetGlobalMean() const {
    double m = 0.0;
    for(unsigned int i=0;i<m_Data.size();i++){
      m += m_Data[i];
    }
    return m/(double)m_Data.size();
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::MovingAverageFilter ( unsigned int length)  {
    
    Signal temp(m_SamplingPeriod);
    
    for (unsigned int i=0; i <size(); i++)
      {
	double accumulator=0.;
	unsigned int scale=0;
	for (unsigned int j= max(0,static_cast<int>(i)-static_cast<int>(length));  j<=min(size(), i+length); j++)
	  {
	    accumulator+=m_Data[j];
	    scale++;
	  }
	temp.push_back(accumulator/scale);
      }
    return temp;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::GaussLikeFilter ( )  {

    Signal temp(m_SamplingPeriod);
    if (size()<2)
      return *this;
    else
      {
	//first sample: mirrorring BC
	temp.push_back((2.*m_Data[0]+2*m_Data[1])/4.);
	
	//middle samples
	for (unsigned int i=1; i <size()-1; i++)
	  temp.push_back((2.*m_Data[i]+m_Data[i-1]+m_Data[i+1])/4.);
	
	//end sample: mirrorring BC
	temp.push_back((2.*m_Data[size()-1]+2*m_Data[size()-2])/4.);
	return temp;
      }
  }
  //---------------------------------------------------------------------
	       

  //---------------------------------------------------------------------
  Signal Signal::NormalizeMeanStdDev(double newMean,double newStdDev)
  {
    iterator itSig=begin();
    double sum=0, sum2=0;
    Signal temp(m_SamplingPeriod);    

    //Calculate the mean and the std dev
    while(itSig!=end())
      {
	sum += *itSig;
	sum2 += (*itSig) * (*itSig);	  
	itSig++;
      }   
    double oldMean=sum/size();	
    double oldStdDev=sqrt(sum2/size()-oldMean*oldMean);
    
    //find a and b  
    double a = newStdDev/oldStdDev;
    double b = newMean - a * oldMean;
    itSig=begin();
    while(itSig!=end())
      {
	temp.push_back( a *(*itSig) + b );
	itSig++;
      }
    return temp;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
//   Signal Signal::HighPassFilter (double sampPeriod, double cutOffFrequency )
//   {
//     //output
//     Signal temp(m_SamplingPeriod);
//     temp.resize(size());
// 
//     //the fft
//     SIGNAL_FFT_TYPE fft;
//   
//     //calculate the cut off frequency  
//     unsigned int samp=lrint(cutOffFrequency*static_cast<double>(size())*sampPeriod);
//    
//     //forward fft with empty fft
//     if(fft.size()==0)  OneDForwardFourier(*this, fft);
//     
//     //remove the frequencies
//     for(unsigned int i=0;i<samp && i<fft.size();i++)
//       fft[i]=complex<double>(0.,0.);
//     
//     //backward with remaining frequencies
//     OneDBackwardFourier(fft,temp);
//     return temp;
//   }
  //---------------------------------------------------------------------

  
 //---------------------------------------------------------------------
//   Signal Signal::LowPassFilter (double sampPeriod, double cutOffFrequency )
//   {
//     //output
//     Signal temp(m_SamplingPeriod);
//     temp.resize(size());
// 
//     //the fft
//     SIGNAL_FFT_TYPE fft;
//   
//     //calculate the cut off frequency  
//     unsigned int samp=lrint(cutOffFrequency*static_cast<double>(size())*sampPeriod);
//     
//     //forward fft with empty fft
//     if(fft.size()==0)  OneDForwardFourier(*this, fft);
//     unsigned int fsize=fft.size();
// 
//     //remove the frequencies 
//     unsigned int limit=min (samp, fsize);
//     for(unsigned int i=limit;i<fft.size();i++)
//     	fft[i]=complex<double>(0.,0.);
// 	
//     //backward with remaining frequencies
//     OneDBackwardFourier(fft,temp);
//     return temp;
//   }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
//   void  Signal::OneDForwardFourier(const Signal& input, SIGNAL_FFT_TYPE & fft)
//   {
//     //Create output array
//     fft.resize(input.size()/2+1);
//     //Temp copy
//     double *tempCopy=new double[size()];
//     copy(begin(), end(), tempCopy);
// 
//     //Forward Fourier Transform   
//     fftw_plan p;
//     p=fftw_plan_dft_r2c_1d(size(),tempCopy,reinterpret_cast<fftw_complex*>(&(fft[0])),FFTW_ESTIMATE);
//     fftw_execute(p);
//     fftw_destroy_plan(p);
//     //delete tempCopy;
//     return;
//   }
  //---------------------------------------------------------------------
  
  
  //---------------------------------------------------------------------
//   void Signal::OneDBackwardFourier(SIGNAL_FFT_TYPE & fft, Signal &output)
//   {
//       
//     //Backward
//     fftw_plan p;
//     p=fftw_plan_dft_c2r_1d(output.size(),reinterpret_cast<fftw_complex*>(&(fft[0])),&(output[0]),FFTW_ESTIMATE);
//     fftw_execute(p); 
//     fftw_destroy_plan(p);
//   
//     vector<double>::iterator it=output.begin();
//     while(it!=output.end()){    
//       *it /= (double)output.size();
//       it++;
//     } 
//     return;
//   }
  //---------------------------------------------------------------------

  
  //---------------------------------------------------------------------
//   double Signal::MaxFreq(const Signal &sig,SIGNAL_FFT_TYPE & fft)
//   {
//   
//     if(fft.size()==0) OneDForwardFourier(sig,fft);
//     int posMax=1;
//     double amplitude, amplitudeMax=abs(fft[1]);
//     for(unsigned int i=1;i<fft.size();i++){      
//       amplitude=abs(fft[i]);
//       if(amplitude>amplitudeMax){
// 	posMax=i;
// 	amplitudeMax=amplitude;
//       }
//     }
//     return ((double)(posMax)/((double)sig.size()*sig.GetSamplingPeriod()));
//   }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::DetectLocalExtrema(unsigned int width)
  {
    Signal temp(m_SamplingPeriod);
    bool isMin, isMax;
    unsigned int upper, lower;

    //has to be at least 1
    width=max(static_cast<int>(width), 1);

    for(unsigned int i=0 ; i < size() ; i++){
      isMax=true;
      isMin=true;

      for(unsigned int j=1; j< width+1; j++)
	{
	  //set the boundaries
	  upper = min( size(), i+j);
	  lower = max( static_cast<int>(0), (int)i-(int)j);

	  //check if max
	  if( ! (m_Data[i] >= m_Data[lower] && m_Data[i] >= m_Data[upper]))isMax=false;
	      
	  //check if min
	  if( ! (m_Data[i]<= m_Data[lower] && m_Data[i] <= m_Data[upper])) isMin=false;
	      
	  //if neither, go to the next value
	  if( (!isMax) && (!isMin)) break;
	}

      if (isMax) temp.push_back(1);
      else if (isMin) temp.push_back(0);
      else temp.push_back(0.5);
    }
    return temp;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::LimPhase()
  {

    //phase is defined as going from 0 to 1 linearly between end expiration
    Signal phase(m_SamplingPeriod);
    phase.resize(size());

    unsigned int firstBeginCycle=0;
    unsigned int firstEndCycle=0;
    unsigned int beginCycle=0; 

    //=========================================
    //Search the first value in extrema not 0.5  
    while(m_Data[beginCycle]!=1)
      {
      beginCycle++;
      }
    
    //We search the corresponding end
    unsigned int endCycle=beginCycle+1; 
    while(endCycle != size() && m_Data[endCycle]!=1){
      endCycle++;
   
      }

    //============================================================
    //Calculate phase at the beginning (before the first extremum)
    for(unsigned int i=0 ; i<beginCycle ; i++)
      {

	//if before first cycle is longer than first cycle: let it go from 0 to 1
	if((double)beginCycle/(double)(endCycle-beginCycle) > 1)
	  {
	    phase[i] = (double)(i-0)/(double)(beginCycle-0);
	  }
	//copy the phase values later
	else
	  { 
	    firstBeginCycle=beginCycle;
	    firstEndCycle=endCycle;
	  }
	
      }

    //===================================================================
    //Middle part
    while(endCycle != size()){
	
      //fill between extrema
      for(unsigned int i=beginCycle ; i<endCycle ; i++){
	phase[i] = (double)(i-beginCycle)/(double)(endCycle-beginCycle);
      }

      //Find next cycle
      beginCycle = endCycle++;
      while(endCycle != size() && m_Data[endCycle]!=1)
	endCycle++;
    }

    //===================================================================
    //Calculate phase at the end (after the last extremum)
    endCycle = beginCycle--;

    //count backwards till the previous
    while(m_Data[beginCycle]!=1) beginCycle--;
    for(unsigned int i=endCycle ; i<size() ; i++)
      {
   
	//after last extrema is longer then last cycle
	if((double)(size()-endCycle)/(double)(endCycle-beginCycle) > 1){

	  //make the last part go till 1
	  phase[i] = (double)(i-endCycle)/(double)(size()-endCycle);	
	}

	//the last part is shorter, copy the last cycle values
	else{
	  phase[i] = phase[i -(endCycle-beginCycle)];
	    }
      }

    //===================================================================
    //check it some remains to be copied in the beginning
    if (firstBeginCycle!=0)
      {
	for(unsigned int i=0 ; i<firstBeginCycle ; i++)
	  phase[i] =phase[i+firstEndCycle-firstBeginCycle];
      }

    return phase;
  }
  //---------------------------------------------------------------------



  //---------------------------------------------------------------------
  Signal Signal::MonPhase()
  {//monPhase

    //phase is defined as going from 0 to 1 linearly between end expiration
    Signal phase(m_SamplingPeriod);
    phase.resize(size());

    unsigned int firstBeginCycle=0;
    unsigned int firstEndCycle=0;
    unsigned int cycleCounter=0;
    unsigned int beginCycle=0; 

    //===================================================================
    //Search the first value in extrema not 0.5  
    while(m_Data[beginCycle]!=1)
      {
      beginCycle++;
    
      }
    //We search the corresponding end
    unsigned int endCycle=beginCycle+1; 
    while(endCycle != size() && m_Data[endCycle]!=1){
      endCycle++;
    
      }

    //===================================================================
    //Calculate phase at the beginning (before the first extremum)
    for(unsigned int i=0 ; i<beginCycle ; i++)
      {

	//if before first cycle is longer than first cycle: let it go from 0 to 1
	if((double)beginCycle/(double)(endCycle-beginCycle) > 1)
	  {
	    phase[i] = (double)(i-0)/(double)(beginCycle-0);
	  }
	//copy the phase values later
	else
	  { 
	    firstBeginCycle=beginCycle;
	    firstEndCycle=endCycle;
	  }
	
      }
    
    //===================================================================
    //Middle part
    while(endCycle != size()){

      cycleCounter++;
      //fill between extrema
      for(unsigned int i=beginCycle ; i<endCycle ; i++)
	phase[i] = (double)cycleCounter+(double)(i-beginCycle)/(double)(endCycle-beginCycle);


      //Find next cycle
      beginCycle = endCycle++;
      while(endCycle != size() && m_Data[endCycle]!=1)
	endCycle++;
    }

    //===================================================================
    //Calculate phase at the end (after the last extremum)
    endCycle = beginCycle--;

    //count backwards till the previous
    while(m_Data[beginCycle]!=1) beginCycle--;
    for(unsigned int i=endCycle ; i<size() ; i++)
      {
   
	//after last extrema is longer then last cycle
	if((double)(size()-endCycle)/(double)(endCycle-beginCycle) > 1){

	  //make the last part go till 1
	  phase[i] = (double)cycleCounter+(double)(i-endCycle)/(double)(size()-endCycle);	
	}

	//the last part is shorter, copy the last cycle values
	else{
	  phase[i] = phase[i -(endCycle-beginCycle)]+1;
	    }
      }

    //===================================================================
    //check it some remains to be copied in the beginning
    if (firstBeginCycle!=0)
      {
	for(unsigned int i=0 ; i<firstBeginCycle ; i++)
	  phase[i] =phase[i+firstEndCycle-firstBeginCycle]-1;
      }

    return phase;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::MonPhaseDE(double eEPhaseValue, double eIPhaseValue)
  {
    //First calculate monPhase
    Signal monPhase=this->MonPhase();

    //Create an empty signal
    Signal phase(size(), -1);
    phase.SetSamplingPeriod(m_SamplingPeriod);

    //Fill in the values at the extrema position
    iterator phaseIt=phase.begin();
    iterator monPhaseIt=monPhase.begin();
    iterator extremaIt =begin();
    while (extremaIt!= end())
    {
      if (*extremaIt==0.) *phaseIt=eIPhaseValue+floor(*monPhaseIt);
      else if (*extremaIt==1.) *phaseIt=eEPhaseValue+floor(*monPhaseIt);
      extremaIt++; phaseIt++;monPhaseIt++;
    }
    
    return phase;

  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::LinearlyInterpolateScatteredValues()
  {
    //Linearly interpolate the values in between
    unsigned int i=0;
    unsigned int beginCycle=0;
    unsigned int endCycle=0;
   
    //Create a new signal
    Signal temp(size(),-1);
    temp.SetSamplingPeriod(m_SamplingPeriod);
      
    //start from the first value
    while (m_Data[i]==-1)i++;
    beginCycle=i; 
    i++;
    
    //Go to the next
    while ( (m_Data[i]==-1) && (i<size()) )i++;
    while (i < size())
      {
	endCycle=i;
	
	//fill in in between
	for (unsigned int k=beginCycle;k<endCycle+1; k++)
	  temp[k]=m_Data[beginCycle]+(double)(k-beginCycle)*
	    (m_Data[endCycle]-m_Data[beginCycle])/(double)(endCycle-beginCycle); 
	
	//swap and move on
	beginCycle=endCycle;
	i++;
	while( (i< size()) && (m_Data[i]==-1) ) i++;
      }
    
    //For the last part
    if (beginCycle!= size()-1)
      {
	//continue with same slope
	for (unsigned int k=beginCycle+1; k<size();k++)
	  temp[k]=temp[beginCycle]+(double)(k-beginCycle)*(temp[beginCycle]-temp[beginCycle-1]);
	
      }
    
    //For the first part
    if (temp[0]==-1)
      {
	i=0;
	while (temp[i]==-1)i++;
	beginCycle=i;
	i++;
	while (m_Data[i]==-1)i++;
	endCycle=i;

	//if the first filled half cycle is longer, copy it
	if(beginCycle<(endCycle-beginCycle))
	  {
	    for (unsigned int k=0; k< beginCycle;k++)
	      temp[k]=temp[k+endCycle-beginCycle];
	  }

	//if the first filled half cycle is longer, start from zero
	else
	  {
	    for (unsigned int k=0; k< beginCycle;k++)
	      temp[k]=k*temp[beginCycle]/(beginCycle);
	  }
      }

    return temp;
  }
  //---------------------------------------------------------------------


//  //---------------------------------------------------------------------
//  Signal Signal::ApproximateScatteredValuesWithBSplines(unsigned int splineOrder, unsigned int numberOfControlPoints)
//   {
//     //filter requires a vector pixelType
//     typedef itk::PointSet<VectorType, 1> PointSetType;
//     PointSetType::Pointer pointSet=PointSetType::New();
//     typedef PointSetType::PointType PointType;
    
//     unsigned int i=0;  
//     unsigned int pointIndex=0;  
//     while (i< size())
//       {
//       if(m_Data[i]!=-1)
// 	{
// 	  PointType p;
// 	  p[0]= i;//JV spacing is allways 1
// 	  pointSet->SetPoint( pointIndex, p );
// 	  pointSet->SetPointData( pointIndex, m_Data[i] ); 
// 	  pointIndex++;
// 	}
//       i++;
//       }

//     //define the output signal properties
//     ImageType::RegionType::SizeType outputSize;
//     outputSize[0]= size();
//     ImageType::PointType outputOrigin;
//     outputOrigin[0]=0.0;//JV may need to be changed
//     ImageType::SpacingType outputSpacing;
//     outputSpacing[0]=1; //JV add relation to the original signal spacing

//     //Convert
//     typedef itk::BSplineScatteredDataPointSetToImageFilter< PointSetType, VectorImageType > PointSetToImageFilterType;
//     PointSetToImageFilterType::Pointer pointSetToImageFilter= PointSetToImageFilterType::New();
//     pointSetToImageFilter->SetInput(pointSet);
//     pointSetToImageFilter->SetSplineOrder(splineOrder);//JV
//     pointSetToImageFilter->SetSize(outputSize);
//     pointSetToImageFilter->SetOrigin(outputOrigin);
//     pointSetToImageFilter->SetSpacing(outputSpacing);
    
//     //Convert to 
//     itk::FixedArray<unsigned int,1> num;
//     num[0]=numberOfControlPoints;
//     pointSetToImageFilter->SetNumberOfControlPoints(num);//JV
//     pointSetToImageFilter->Update();
//     VectorImageType::Pointer approximatedSignal=pointSetToImageFilter->GetOutput();

//     //Convert and return
//     return ConvertVectorImageToSignal(approximatedSignal);
//   }
//   //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::ConvertVectorImageToSignal(VectorImageType::Pointer image)
  {
    //empty signal
    Signal signal;
    
    //make an image iterator
    itk::ImageRegionConstIterator<VectorImageType> it(image,image->GetLargestPossibleRegion());
    it.Begin();
    
    //copy
    while(!it.IsAtEnd())
      {
	signal.push_back(it.Get()[0]);
	++it;
      }
    
    //Spacing
    signal.SetSamplingPeriod(image->GetSpacing()[0]);
    
    return signal;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  Signal Signal::LimitSignalRange()
  {
    //empty signal
    Signal signal(m_SamplingPeriod);
    iterator it=begin();
    while(it != end())
      {
	signal.push_back(*it-floor(*it));
	it++;
      }
    return signal;
  }


  //---------------------------------------------------------------------
  double Signal::SSD(const Signal &sig2) const{
    if(sig2.size() != size()){
      cerr << "ERROR in Signal::SSD: signals don't have the same size" << endl;
      return -1;
    }
    double result=0.;
    for(unsigned int i=0;i<size();i++){
      result+=pow(sig2[i]-m_Data[i],2);
    }
    result/=size();
    result=sqrt(result);
    return result;
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  void Signal::AddValue(double v) {
    for(unsigned int i=0;i<size();i++) {
      m_Data[i] += v;
    }
  }
  //---------------------------------------------------------------------


  //---------------------------------------------------------------------
  void Signal::ComputeAugmentedSpace(clitk::Signal & outputX, 
                                     clitk::Signal & outputY, 
                                     unsigned int delay) const {
    if (size() <= delay) {
      std::cerr << "Error in signal length is " << size()
                << " while delay is " << delay << " : too short. Abort." << std::endl;
      exit(0);
    }
    outputX.resize(size()-delay);
    outputY.resize(size()-delay);
    for(unsigned int i=0; i<outputX.size(); i++) {
      outputX[i] = m_Data[i+delay];
      outputY[i] = m_Data[i];
    }
  }
  //---------------------------------------------------------------------














  //  double Signal::Compare(Signal & sigRef) {
  //     double coeffCorrParam[5]={0.,0.,0.,0.,0.};
    
  //     const_iterator itSig=begin();
  //     const_iterator itSigRef=sigRef.begin();//+offset;
  //     while(itSig!=end()){
  //       coeffCorrParam[0]+=*itSig;
  //       coeffCorrParam[1]+=*itSigRef;
  //       coeffCorrParam[2]+=(*itSig)*(*itSigRef);
  //       coeffCorrParam[3]+=(*itSig)*(*itSig);
  //       coeffCorrParam[4]+=(*itSigRef)*(*itSigRef);      
  //       itSig++;itSigRef++;
  //     }

  //     double coeffCorr = pow(size()*coeffCorrParam[2]-coeffCorrParam[0]*coeffCorrParam[1],2)/
  //       ((size()*coeffCorrParam[3]-pow(coeffCorrParam[0],2))*
  //        (size()*coeffCorrParam[4]-pow(coeffCorrParam[1],2)));
    
  //     return coeffCorr;    
  //   }
  
  //   int Signal::DerivateSigne( const_iterator & it) const{
  //     int pos=-1;
  //     if(it==begin())
  //       pos=1;    
  //     if((*it)==(*(it+pos)))
  //       return 0;
  //     else if((*it)<(*(it+pos)))
  //       return 1*pos;
  //     else // Case : ((*it)>(*(it+pos)))
  //       return -1*pos;    
  //   }

  //   void Signal::CenteredFiniteDifferences(Signal & result,int order,int* weights){
  //     const_iterator itSig=begin()+order;
  //     result.resize(size());
  //     iterator itDer=result.begin()+order;
  //     while(itSig!=end()-order){
  //       (*itDer)=0.;
  //       for(int i=-order;i<=order;i++){
  // 	*itDer+=*(itSig-i)*weights[i+order];      
  //       }
  //       itSig++;itDer++;
  //     }
  //   }

  //   void Signal::FirstDerivate(Signal & result,int order){
  //     if(order==1){
  //       int weights[3]={-1,0,1};
  //       CenteredFiniteDifferences(result,order,weights);
  //     }
  //     else if(order==2){
  //       int weights[5]={1,-8,0,8,-1};
  //       CenteredFiniteDifferences(result,order,weights);
  //     }
  //   }

  //   void Signal::SecondDerivate(Signal & result,int order){
  //     if(order==1){
  //       int weights[3]={1,-2,1};
  //       CenteredFiniteDifferences(result,order,weights);
  //     }
  //     else if(order==2){
  //       int weights[5]={-1,16,-30,16,-1};
  //       CenteredFiniteDifferences(result,order,weights);
  //     }
  //   }



  //   void Signal::NormalizeMeanStdDev(double newMean,double newStdDev){
  //     iterator itSig=begin();
  // 	double sum=0, sum2=0;
  //     while(itSig!=end()){
  //       sum += *itSig;
  // 	  sum2 += (*itSig) * (*itSig);	  
  //       itSig++;
  //     }   
  // 	double oldMean=sum/size();	
  // 	double oldStdDev=sqrt(sum2/size()-oldMean*oldMean);

  // 	double a = newStdDev/oldStdDev;
  // 	double b = newMean - a * oldMean;
  // 	itSig=begin();
  // 	while(itSig!=end()){
  // 	  *itSig = a *(*itSig) + b;
  //       itSig++;
  // 	}
  //   }



  //   void Signal::print(ostream & os, const int level) const {
  //     os << "Size:" << m_Data.size() << endl;
  //     const_iterator it=m_Data.begin();
  //     while(it!=m_Data.end()){
  //       os << *it << endl;
  //       it++;
  //     }
  //   }
    
    
  //   //   }
  
  //   //   istream& Signal::get(istream& is) {
  //   //     ERROR << "Signal::get NOT IMPLEMENTED";
  //   //     FATAL();
  //   //     return is;
  //   //   } ////
  
  //   //   /** @b GridBase::put os
  //   //    * @param os 
  //   //    * @return 
  //   //    ***************************************************/
  //   //   ostream& Signal::put(ostream& os) const {
  //   //     print(os);
  //   //     return os;
  //   //   } ////



  //   void Signal::Crop(unsigned int posmin, unsigned int posmax){
  //     if(posmin >= m_Data.size()) return;
  //     if(posmax >= m_Data.size()) posmax=m_Data.size();
  //     m_Data.erase(m_Data.begin()+posmax+1,m_Data.end());
  //     m_Data.erase(m_Data.begin(),m_Data.begin()+posmin);
  //   }

  //   void Signal::LinearResample(const unsigned int newSize){
  //     SIGNAL newData;
  //     newData.push_back(front());
  //     double posInOld,leftWeight,rightWeight;
  //     int leftPos, rightPos;    
  //     for(unsigned int i=1 ; i < newSize-1 ; i++){
  //       posInOld = (double)(i * (size()-1)) / (double)(newSize-1);
  //       leftPos = (int)floor(posInOld);
  //       rightPos = leftPos+1;
  //       leftWeight = (double)rightPos - posInOld;
  //       rightWeight =  posInOld - (double)leftPos;
  //       newData.push_back(m_Data[leftPos] * leftWeight + m_Data[rightPos] * rightWeight );
  //     }
    
  //     newData.push_back(back());
  //     m_Data=newData;
  //   }


  //   int Signal::FreqToSamp(double freq){
  //     if(m_SamplingPeriod==-1.)
  //       cerr << "ERROR: you did not initialize the sampling period" << endl;
  //     return lrint(freq*(double)size()*m_SamplingPeriod);
  //   }
  //   double Signal::SampToFreq(int samp){
  //     if(m_SamplingPeriod==-1.)
  //       cerr << "ERROR: you did not initialize the sampling period" << endl;
  //     return ((double)(samp)/((double)size()*m_SamplingPeriod));
  //   }

//   //---------------------------------------------------------------------
//   Signal Signal::limPhaseDE(eIPhaseValue, eEPhaseValue)
//   {
   
//     //Create an empty signal
//     phase.resize(size());

//     iterator phaseIt=initialPhaseValues.begin();
//     iterator monPhaseIt=monPhase.begin();
//     iterator extremaIt =begin();

//     while (extremaIt!= end())
//     {
//       if (*extremaIt==0.) *phaseIt=eIPhaseValue+floor(*monPhaseIt);
//       else if (*extremaIt==1.) *phaseIt=eEPhaseValue+floor(*monPhaseIt);
//       extremaIt++; phaseIt++;monPhaseIt++;
//     }

//   }


}

#endif //#define CLITKSIGNAL
