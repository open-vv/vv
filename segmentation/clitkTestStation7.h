

//-----------------------------------

/*
  - use DB from step to step or ExtractStation_7 mother class
  - add "temporary flag" to DB to avoid write image 


 */

//-----------------------------------
#include "clitkAnatomicalFeatureDatabase.h"

class SegmentationStepBase;

class LymphStationSegmentationFilterBase: 
  public virtual clitk::FilterBase, 
  public clitk::FilterWithAnatomicalFeatureDatabaseManagement {
public:
  LymphStationSegmentationFilterBase() { DD("LymphStationSegmentationFilterBase const"); }

  void AddStep(SegmentationStepBase * step) {
    m_Steps.push_back(step);
  }
  
  std::vector<SegmentationStepBase*> m_Steps;
  // AnatomicalFeatureDatabase * GetAFDB();
  
  // HERE GetAFDB

};


//-----------------------------------


class SegmentationStepBase
{
public:
  typedef itk::Image<unsigned char, 3>  ImageType;
  typedef ImageType::PointType ImagePointType;

  SegmentationStepBase(std::string name);
  virtual void StartStep();  
  void InitStep(LymphStationSegmentationFilterBase * p){
    DD("SegmentationStepBase::InitStep");
    parent = p;
  }
  std::string name;
  LymphStationSegmentationFilterBase * parent;
};

SegmentationStepBase::SegmentationStepBase(std::string n) {
  DD("SegmentationStepBase constructor");
  name = n;
  DD(name);
}

void SegmentationStepBase::StartStep() {
  DD("Start step (verbose debug skip)");
  DD(name);
  // VERBOSE, DEBUG
  // IF SKIP
}

//-----------------------------------

class ExtractStation7_APLimits:public SegmentationStepBase {
public:
  typedef SegmentationStepBase Super;
  ExtractStation7_APLimits():SegmentationStepBase("Step AP Limits") {}
  virtual void StartStep();
  // Accès au types  ==> Super::
  // Accès au AFDB   ==> parent->

  // TODO
  // , support, image // herit ; how to set ? 
  // ggo
};


void ExtractStation7_APLimits::StartStep() {
  Super::StartStep(); // -> needed to verbose, debug, skip etc
  DD("ExtractStation7_APLimits StartStep");
  Super::ImagePointType carina;
  parent->GetAFDB()->GetPoint3D("carina", carina);
  DD(carina);
  
  // Writeimage
}

//-----------------------------------


// template <class TImageType>
class ExtractLymphStation_7:public LymphStationSegmentationFilterBase {
public:
  ExtractLymphStation_7() {
    DD("ExtractStation_7 constructor");    
    // insert step (could be autolist if needed);
    AddStep(new ExtractStation7_APLimits);
  }

  // SET GGO dont (AFDB);

  void StartSegmentation() {
    DD("ExtractStation_7 Start");    
    // Loop on list of SegmentationStepBase
    m_Steps[0]->InitStep(this); // parent (donc AFFB)
    m_Steps[0]->StartStep();
    // things todo after a step ? -> if writeimage, to be done inside
    
  }
};

