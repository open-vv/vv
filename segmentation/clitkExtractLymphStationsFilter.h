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

#ifndef CLITKEXTRACTLYMPHSTATIONSFILTER_H
#define CLITKEXTRACTLYMPHSTATIONSFILTER_H

// clitk
#include "clitkStructuresExtractionFilter.h"
#include "clitkLabelImageOverlapMeasureFilter.h"

// vtk
#include <vtkPolyData.h>

namespace clitk {
  
  class SupportLimitsType {
  public:
    std::string station_limit;
    std::string station;
    std::string structure_limit;
    std::string structure;
    double offset;
    void Read(istream & is) {
      is >> station_limit;
      is >> station;
      is >> structure_limit;
      is >> structure;
      std::string s;
      is >> s;
      offset = atof(s.c_str());
    }
  };

  //--------------------------------------------------------------------
  /*
    Try to extract the LymphStations part of a thorax CT.
    Need a set of Anatomical Features (AFDB)
  */
  //--------------------------------------------------------------------
  
  template <class TImageType>
  class ITK_EXPORT ExtractLymphStationsFilter: 
    public clitk::StructuresExtractionFilter<TImageType>
  {

  public:
    /** Standard class typedefs. */
    typedef clitk::StructuresExtractionFilter<TImageType> Superclass;
    typedef ExtractLymphStationsFilter          Self;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractLymphStationsFilter, ImageToImageFilter);

    /** Some convenient typedefs. */
    typedef TImageType                       ImageType;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   ImageRegionType; 
    typedef typename ImageType::PixelType    ImagePixelType; 
    typedef typename ImageType::SizeType     ImageSizeType; 
    typedef typename ImageType::IndexType    ImageIndexType; 
    typedef typename ImageType::PointType    ImagePointType; 
        
    typedef uchar MaskImagePixelType;
    typedef itk::Image<MaskImagePixelType, 3>    MaskImageType;  
    typedef typename MaskImageType::Pointer      MaskImagePointer;
    typedef typename MaskImageType::RegionType   MaskImageRegionType; 
    typedef typename MaskImageType::SizeType     MaskImageSizeType; 
    typedef typename MaskImageType::IndexType    MaskImageIndexType; 
    typedef typename MaskImageType::PointType    MaskImagePointType; 

    typedef itk::Image<MaskImagePixelType, 2>    MaskSliceType;
    typedef typename MaskSliceType::Pointer      MaskSlicePointer;
    typedef typename MaskSliceType::PointType    MaskSlicePointType;
    typedef typename MaskSliceType::RegionType   MaskSliceRegionType; 
    typedef typename MaskSliceType::SizeType     MaskSliceSizeType; 
    typedef typename MaskSliceType::IndexType    MaskSliceIndexType; 

    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    FILTERBASE_INIT;
     
    itkGetConstMacro(BackgroundValue, MaskImagePixelType);
    itkGetConstMacro(ForegroundValue, MaskImagePixelType);
    itkSetMacro(BackgroundValue, MaskImagePixelType);
    itkSetMacro(ForegroundValue, MaskImagePixelType);

    // Station 8
    itkSetMacro(EsophagusDiltationForAnt, MaskImagePointType);
    itkGetConstMacro(EsophagusDiltationForAnt, MaskImagePointType);
    itkSetMacro(EsophagusDiltationForRight, MaskImagePointType);
    itkGetConstMacro(EsophagusDiltationForRight, MaskImagePointType);
    itkSetMacro(InjectedThresholdForS8, double);
    itkGetConstMacro(InjectedThresholdForS8, double);

    // Station 7
    itkGetConstMacro(S7_UseMostInferiorPartOnlyFlag, bool);
    itkSetMacro(S7_UseMostInferiorPartOnlyFlag, bool);
    itkBooleanMacro(S7_UseMostInferiorPartOnlyFlag);

    // All stations
    bool GetComputeStation(std::string s);
    void AddComputeStation(std::string station) ;
    void SetFuzzyThreshold(std::string station, std::string tag, double value);
    double GetFuzzyThreshold(std::string station, std::string tag);
    void SetThreshold(std::string station, std::string tag, double value);
    double GetThreshold(std::string station, std::string tag);
    itkGetConstMacro(ForceSupportsFlag, bool);
    itkSetMacro(ForceSupportsFlag, bool);
    itkBooleanMacro(ForceSupportsFlag);

    itkGetConstMacro(CheckSupportFlag, bool);
    itkSetMacro(CheckSupportFlag, bool);
    itkBooleanMacro(CheckSupportFlag);

    itkGetConstMacro(SupportLimitsFilename, std::string);
    itkSetMacro(SupportLimitsFilename, std::string);

  protected:
    ExtractLymphStationsFilter();
    virtual ~ExtractLymphStationsFilter() {}
    
    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();
    
    // To avoid repeat "this->"
    AnatomicalFeatureDatabase * GetAFDB() { return clitk::FilterWithAnatomicalFeatureDatabaseManagement::GetAFDB(); }
    void WriteAFDB() { clitk::FilterWithAnatomicalFeatureDatabaseManagement::WriteAFDB(); }
    void LoadAFDB() {  clitk::FilterWithAnatomicalFeatureDatabaseManagement::LoadAFDB(); }  
    void StartNewStep(std::string s) { clitk::FilterBase::StartNewStep(s); }
    void StartSubStep() { clitk::FilterBase::StartSubStep(); }
    template<class TInternalImageType>
      void StopCurrentStep(typename TInternalImageType::Pointer p, std::string txt="") { clitk::FilterBase::StopCurrentStep<TInternalImageType>(p, txt); }
    void StopCurrentStep() {clitk::FilterBase::StopCurrentStep(); }
    void StopSubStep() {clitk::FilterBase::StopSubStep(); }

    ImageConstPointer  m_Input;
    MaskImagePointer   m_Mediastinum;
    MaskImagePointer   m_Working_Support;
    std::map<std::string, MaskImagePointer> m_ListOfStations;
    std::map<std::string, MaskImagePointer> m_ListOfSupports;
    MaskImagePixelType m_BackgroundValue;
    MaskImagePixelType m_ForegroundValue;
    std::map<std::string, bool> m_ComputeStationMap;
    std::string m_SupportLimitsFilename;
    std::vector<SupportLimitsType> m_ListOfSupportLimits;

    bool CheckForStation(std::string station);
    void Remove_Structures(std::string station, std::string s);
    void WriteImageSupport(std::string support);
    void WriteImageStation(std::string station);
    void ComputeOverlapWithRef(std::string station);
    void Support_SI_Limit(const std::string station_limit, const std::string station, 
                          const std::string structure_limit, const std::string structure, 
                          const double offset);
    void ReadSupportLimits(std::string filename);

    // Functions common to several stations
    double FindCarina();
    double FindApexOfTheChest();
    double FindSuperiorBorderOfAorticArch();
    double FindInferiorBorderOfAorticArch();
    void FindLeftAndRightBronchi();
    void FindLineForS7S8Separation(MaskImagePointType & A, MaskImagePointType & B);
    MaskImagePointer FindAntPostVesselsOLD();
    MaskImagePointer FindAntPostVessels2();

    // Global parameters
    typedef std::map<std::string, double> FuzzyThresholdByStructureType;
    std::map<std::string, FuzzyThresholdByStructureType> m_FuzzyThreshold;    
    typedef std::map<std::string, double> ThresholdByStructureType;
    std::map<std::string, ThresholdByStructureType> m_Threshold;    

    // Station's supports
    void ExtractStationSupports();
    void Support_LeftRight_S1R_S1L();
    void Support_LeftRight_S2R_S2L();
    void Support_LeftRight_S4R_S4L();
    void Support_Post_S1S2S4();

    MaskImagePointer LimitsWithTrachea(MaskImageType * input, 
                                       int extremaDirection, int lineDirection, 
                                       double offset, double maxSupPosition);
    MaskImagePointer LimitsWithTrachea(MaskImageType * input, 
                                       int extremaDirection, int lineDirection, 
                                       double offset);
    // Station 8
    double m_DiaphragmInferiorLimit;
    double m_OriginOfRightMiddleLobeBronchusZ;
    double m_InjectedThresholdForS8;
    MaskImagePointer m_Esophagus;
    MaskImagePointType m_EsophagusDiltationForAnt;
    MaskImagePointType m_EsophagusDiltationForRight;

    void ExtractStation_8();
    void ExtractStation_8_SetDefaultValues();
    void ExtractStation_8_SI_Limits();
    void ExtractStation_8_Ant_Limits();
    void ExtractStation_8_Left_Sup_Limits();
    void ExtractStation_8_Left_Inf_Limits();
    void ExtractStation_8_Single_CCL_Limits();
    void ExtractStation_8_Remove_Structures();

    // Station 3P
    void ExtractStation_3P();
    void ExtractStation_3P_SetDefaultValues();
    void ExtractStation_3P_LR_inf_Limits();
    void ExtractStation_3P_LR_sup_Limits_2();
    void ExtractStation_3P_Remove_Structures();
    void ExtractStation_3P_LR_sup_Limits();

    // Station 3A
    void ExtractStation_3A();
    void ExtractStation_3A_SetDefaultValues();
    void ExtractStation_3A_Post_Left_Limits_With_Aorta_S5_Support();
    void ExtractStation_3A_Post_Limits_With_Dilated_Aorta_S6_Support();
    void ExtractStation_3A_AntPost_Superiorly();
    void ExtractStation_3A_Remove_Structures();

    // Station 2RL
    void ExtractStation_2RL();
    void ExtractStation_2RL_SetDefaultValues();
    void ExtractStation_2RL_Ant_Limits(std::string s);
    void ExtractStation_2RL_Remove_Structures(std::string s);
    void ExtractStation_2RL_Cut_BrachioCephalicVein_superiorly_when_it_split();
    vtkSmartPointer<vtkPolyData> Build3DMeshFrom2DContour(const std::vector<ImagePointType> & points);

    // Station 1RL
    void ExtractStation_1RL();
    void ExtractStation_1RL_SetDefaultValues();
    void ExtractStation_1RL_Ant_Limits();
    void ExtractStation_1RL_Post_Limits();

    // Station 4RL
    void ExtractStation_4RL_SetDefaultValues();
    void ExtractStation_4L();
    void ExtractStation_4R();
    void ExtractStation_S4L_S5_Limits_Aorta_LeftPulmonaryArtery(int KeepPoint);

    // Station 5
    void ExtractStation_5_SetDefaultValues();
    void ExtractStation_5();
    void ExtractStation_5_Limits_AscendingAorta_Ant();

    // Station 6
    void ExtractStation_6_SetDefaultValues();
    void ExtractStation_6();


    // Station 7
    void ExtractStation_7();
    void ExtractStation_7_SetDefaultValues();
    void ExtractStation_7_SI_Limits();
    void ExtractStation_7_RL_Interior_Limits();

    void ExtractStation_7_RL_Limits_OLD();
    void ExtractStation_7_Posterior_Limits();   
    void ExtractStation_7_Remove_Structures();
    bool m_S7_UseMostInferiorPartOnlyFlag;
    bool m_ForceSupportsFlag;
    bool m_CheckSupportFlag;
    MaskImagePointer m_Working_Trachea;
    MaskImagePointer m_LeftBronchus;
    MaskImagePointer m_RightBronchus;
    typedef std::vector<MaskImageType::PointType> ListOfPointsType;
    ListOfPointsType  m_RightMostInLeftBronchus;
    ListOfPointsType  m_AntMostInLeftBronchus;
    ListOfPointsType  m_PostMostInLeftBronchus;
    ListOfPointsType  m_LeftMostInRightBronchus;
    ListOfPointsType  m_AntMostInRightBronchus;
    ListOfPointsType  m_PostMostInRightBronchus;

    void FindExtremaPointsInBronchus(MaskImagePointer input, 
				     int direction,
				     double distance_max_from_center_point, 
				     ListOfPointsType & LR, 
				     ListOfPointsType & Ant, 
				     ListOfPointsType & Post);
  private:
    ExtractLymphStationsFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractLymphStationsFilter.txx"
#include "clitkExtractLymphStation_Supports.txx"
#include "clitkExtractLymphStation_3P.txx"
#include "clitkExtractLymphStation_2RL.txx"
#include "clitkExtractLymphStation_3A.txx"
#include "clitkExtractLymphStation_4RL.txx"
#include "clitkExtractLymphStation_1RL.txx"
#include "clitkExtractLymphStation_5.txx"
#include "clitkExtractLymphStation_6.txx"

#include "clitkExtractLymphStation_8.txx"
#include "clitkExtractLymphStation_7.txx"
#endif

#endif
