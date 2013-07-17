#ifndef __vvDicomServerQueryFactory_h_INCLUDED__
#define __vvDicomServerQueryFactory_h_INCLUDED__

#include "gdcmCompositeNetworkFunctions.h"


struct vvQuery{
	gdcm::ERootType theRoot;
	gdcm::EQueryLevel theLevel;
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
};

class vvDicomServerQueryFactory {

public:

	vvDicomServerQueryFactory(){}
	~vvDicomServerQueryFactory(){}

	gdcm::SmartPointer<gdcm::BaseRootQuery> composeQuerySeries(	std::vector< std::pair<gdcm::Tag, std::string> > keys);
	gdcm::SmartPointer<gdcm::BaseRootQuery> composeQueryStudy(	std::string m_patient);

	vvQuery getQueryforImages(const std::string patient_id, const std::string study_id, const std::string series_id,bool bdisplay);
	 vvQuery getQueryforSeries(const std::string patient_id, const std::string study_id, bool bdisplay);
	vvQuery getQueryforStudy(const std::string patient_id, bool bdisplay);
	 vvQuery getQueryPatient(const std::string i_patname, const std::string i_patid);



   std::vector< std::pair<gdcm::Tag, std::string> > getQueryKeysforStudy(const std::string patient_id, bool bdisplay);
   	std::vector< std::pair<gdcm::Tag, std::string> > getPatientKeys(const std::string , const std::string );
   	std::vector< std::pair<gdcm::Tag, std::string> > getSeriesKeys(const std::string patient_id, const std::string study_id, bool bdisplay);
	std::vector< std::pair<gdcm::Tag, std::string> > getQueryKeysforImages(const std::string patient_id, const std::string study_id, const std::string series_id,bool bdisplay);

	vvQuery getMoveQuery() { return m_query;}

private:

	vvQuery m_query;


};
#endif // __vvDicomServerQueryFactory_h_INCLUDED__
