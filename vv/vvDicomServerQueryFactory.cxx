#include "vvDicomServerQueryFactory.h"


gdcm::SmartPointer<gdcm::BaseRootQuery> vvDicomServerQueryFactory::composeQuerySeries( 	std::vector< std::pair<gdcm::Tag, std::string> > keys )
{
	gdcm::EQueryLevel theLevel = gdcm::eSeries;
	gdcm::ERootType theRoot  = gdcm::ePatientRootType;//ePatientRootType;
	 return gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);
}


vvQuery vvDicomServerQueryFactory::getQueryforStudy(const  std::string patient_id, bool bdisplay)
{
	vvQuery query;
	query.keys = getQueryKeysforStudy( patient_id, bdisplay);
	query.theRoot = gdcm::ePatientRootType;
	query.theLevel = gdcm::eStudy;
	m_query = query;
	return query;
}

std::vector< std::pair<gdcm::Tag, std::string> > vvDicomServerQueryFactory::getQueryKeysforStudy(const std::string patient_id, bool bdisplay)
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;


	// Study Description
	gdcm::Tag tagsdc(0x0008,0x1030);
	keys.push_back(std::make_pair(tagsdc, ""));
	// Study date
	gdcm::Tag tagdb(0x0008,0x0020);
	keys.push_back(std::make_pair(tagdb, ""));
	// Study Hour
	gdcm::Tag tagsdh(0x0008,0x0030);
	keys.push_back(std::make_pair(tagsdh, ""));

	// Study UID
	gdcm::Tag tagsid(0x020,0x000d);
	keys.push_back(std::make_pair(tagsid, ""));

	if (!bdisplay)
	{
			// Patient ID
	gdcm::Tag tagsd(0x0010,0x0020);
	keys.push_back(std::make_pair(tagsd, patient_id));
	}

	return keys;
}

vvQuery vvDicomServerQueryFactory::getQueryforImages(const std::string patient_id, const std::string study_id, const std::string series_id,bool bdisplay)
{
	vvQuery query;
	query.keys = getQueryKeysforImages( patient_id, study_id, series_id, bdisplay);
	query.theRoot = gdcm::ePatientRootType;
	query.theLevel = gdcm::eImage;
	m_query = query;
	return query;
}

std::vector< std::pair<gdcm::Tag, std::string> > vvDicomServerQueryFactory::getQueryKeysforImages(const std::string patient_id, const std::string study_id, const std::string series_id,bool bdisplay)
{

	std::vector< std::pair<gdcm::Tag, std::string> > keys;

	if (!bdisplay)
	{
		//Patient UID
		keys.push_back(std::make_pair(gdcm::Tag (0x0010,0x0020), patient_id));

		//Study UID
	//	keys.push_back(std::make_pair(gdcm::Tag(0x0020,0x000d), study_id));
	
		//Series UID
		keys.push_back(std::make_pair(gdcm::Tag(0x0020,0x000e), series_id));
	}
	// Image Description

	keys.push_back(std::make_pair(gdcm::Tag(0x0020,0x0013), ""));
	//SOP Instance UID
	keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x0018), ""));
	return keys;
}



vvQuery vvDicomServerQueryFactory::getQueryPatient(const std::string i_patname, const std::string i_patid)
{
	vvQuery query;
	query.theRoot = gdcm::ePatientRootType;
	query.theLevel = gdcm::ePatient;
	query.keys = getPatientKeys(i_patname, i_patid);
	m_query = query;
	return query;

}

std::vector< std::pair<gdcm::Tag, std::string> > vvDicomServerQueryFactory::getPatientKeys(const std::string i_patname, const std::string i_patid)
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
	// Patient Name
	gdcm::Tag tag(0x0010,0x0010);
	keys.push_back(std::make_pair(tag, i_patname));

	//// Patient ID
	gdcm::Tag tagpid(0x0010,0x0020);
	keys.push_back(std::make_pair(tagpid, i_patid));
	return keys;
}


vvQuery vvDicomServerQueryFactory::getQueryforSeries(const std::string patient_id, const std::string series_id, bool bdisplay)
{
	vvQuery query;
	query.theRoot = gdcm::ePatientRootType;
	query.theLevel = gdcm::eSeries;
	query.keys = getSeriesKeys(patient_id, series_id, bdisplay);
	m_query = query;
	return query;
}
std::vector< std::pair<gdcm::Tag, std::string> > vvDicomServerQueryFactory::getSeriesKeys(const std::string patient_id, const std::string study_id, bool bdisplay)
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
	// Modality
  keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x0060), ""));
	// Study date
	
	keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x103e),""));
// Series Instance UID
	keys.push_back(std::make_pair(gdcm::Tag(0x0020,0x000e), ""));



	if(!bdisplay)
	{
		// Study Instance UID
		gdcm::Tag tagsid(0x0020,0x1209);
		keys.push_back(std::make_pair(tagsid, study_id));
	}

	return keys;
}