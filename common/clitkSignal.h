#ifndef CLITKSIGNAL_H
#define CLITKSIGNAL_H

//Adapted from Signal.hh in ilr (Simon)

#include "clitkCommon.h"
#include "clitkIO.h"

//include external library
//#include <fftw3.h>
#include <complex>

//itk include
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkPointSet.h"
// #include "itkBSplineScatteredDataPointSetToImageFilter.h"

using namespace std;

namespace clitk{

class Signal{
  public:

  //=====================================================================================
  //Typedefs
  typedef double SignalValueType;
  typedef vector< SignalValueType > SignalType;
  typedef SignalType::iterator iterator;
  typedef SignalType::const_iterator const_iterator;
  //typedef vector< complex<double> > SIGNAL_FFT_TYPE;
  
  typedef itk::Image<double,1> ImageType;
  typedef itk::Vector<double,1> VectorType;
  typedef itk::Image<VectorType,1> VectorImageType;
 
  //=====================================================================================
  //Constructors - Destructors
  Signal(){SetSamplingPeriod(1.);}
  Signal(double sp):m_SamplingPeriod(sp){;}
  Signal(const string fileName){Read(fileName);}
  Signal(unsigned int size, SignalValueType voidValue=0.)
  {
    m_Data.resize(size,voidValue);
    SetSamplingPeriod(1);
  };
  
  ~Signal(){}

  //=====================================================================================  
  //IO
  void Read(string fileName);
  void Read(string fileName, int col);
  void ReadXDR(string fileName);
  void Write(const string fileName);
  
  //=====================================================================================
  //Common vector properties for signals
  unsigned int size() const {return m_Data.size();}
  iterator begin(){return m_Data.begin();}
  iterator end() {return m_Data.end();}
  const_iterator begin() const {return m_Data.begin();}
  const_iterator end() const {return m_Data.end();}
  SignalValueType front() {return m_Data.front();}
  SignalValueType back() {return m_Data.back();}
  void push_back(SignalValueType value){return m_Data.push_back(value);}    
  void resize(unsigned int newSize){return m_Data.resize(newSize);}
  void clear(){m_Data.clear();}
  
  void print(ostream & os = cout, const int level = 0) const;
    
  //=====================================================================================
  //Operators
  SignalValueType& operator[](int index){return m_Data[index];}
  const SignalValueType& operator[](int index) const {return m_Data[index];}
  Signal & operator/=(Signal & d);
  Signal & operator*=(Signal & d);
  
  //Functions
  Signal Normalize(double newMin=0.,double newMax=1.);
  vector<double> GetGlobalMinMax() const;
  double GetGlobalMean() const;
  Signal MovingAverageFilter ( unsigned int length);
  Signal GaussLikeFilter ();
  Signal NormalizeMeanStdDev(double newMean=0.5,double newStdDev=0.5);
  //Signal HighPassFilter (double sampPeriod, double cutOffFrequency );
  //Signal LowPassFilter (double sampPeriod, double cutOffFrequency );
  //double MaxFreq(const Signal &sig,SIGNAL_FFT_TYPE & fft);
  //void OneDForwardFourier(const Signal& input,SIGNAL_FFT_TYPE & fft);
  //void OneDBackwardFourier(SIGNAL_FFT_TYPE & fft, Signal &output);
  Signal DetectLocalExtrema(unsigned int width);
  Signal LimPhase();
  Signal MonPhase();
  Signal MonPhaseDE(double ee, double ei);
  double SSD(const Signal &sig2) const;
  Signal LinearlyInterpolateScatteredValues();
  //   Signal ApproximateScatteredValuesWithBSplines (unsigned int splineOrder, unsigned int numberOfControlPoints); 
  Signal LimitSignalRange();  
  
  void AddValue(double v);
  void ComputeAugmentedSpace(Signal & outputX, Signal & outputY, unsigned int delay) const;

  //     double Compare(Signal & sigRef);
  //     int DerivateSigne( const_iterator & it) const;
  //     void CenteredFiniteDifferences(Signal & result,int order,int* weights);
  //     void FirstDerivate(Signal & result,int order);
  //     void SecondDerivate(Signal & result,int order);
  
  //     void Crop(unsigned int posmin, unsigned int posmax);
  //     void LinearResample(const unsigned int newSize);

 
  //=====================================================================================
  // Get and Set function
  double GetSamplingPeriod() const {return m_SamplingPeriod;}
  void SetSamplingPeriod(double sp){m_SamplingPeriod=sp;}

  //=====================================================================================
  //Conversion for using itk filters
  Signal ConvertImageToSignal(ImageType::Pointer image);
  ImageType::Pointer ConvertSignalToImage( Signal);
  Signal ConvertVectorImageToSignal (VectorImageType::Pointer m);   
  
protected:  
  SignalType m_Data;
  double m_SamplingPeriod;
};
}
#endif
