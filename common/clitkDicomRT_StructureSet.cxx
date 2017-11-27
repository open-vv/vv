/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX

  Authors belongs to:
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://www.centreleonberard.fr
  - CREATIS CNRS laboratory      http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence
  - BSD       http://www.opensource.org/licenses/bsd-license.php
  - CeCILL-B  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html

  =========================================================================*/

#include "clitkDicomRT_StructureSet.h"
#include <vtksys/SystemTools.hxx>
#include "gdcmFile.h"

//--------------------------------------------------------------------
clitk::DicomRT_StructureSet::DicomRT_StructureSet()
{
  mStudyID = "NoStudyID";
  mStudyTime = "NoStudyTime";
  mStudyDate = "NoStudyDate";
  mLabel = "NoLabel";
  mName = "NoName";
  mDate = "NoDate";
  mTime = "NoTime";
  mFile = NULL;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRT_StructureSet::~DicomRT_StructureSet()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_StructureSet::GetStudyID() const
{
  return mStudyID;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_StructureSet::GetStudyTime() const
{
  return mStudyTime;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_StructureSet::GetStudyDate() const
{
  return mStudyDate;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_StructureSet::GetLabel() const
{
  return mLabel;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_StructureSet::GetName() const
{
  return mName;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_StructureSet::GetDate() const
{
  return mDate;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_StructureSet::SetTransformMatrix(vtkMatrix4x4* matrix)
{
  mTransformMatrix = matrix;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_StructureSet::GetTime() const
{
  return mTime;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// const std::vector<clitk::DicomRT_ROI::Pointer> & clitk::DicomRT_StructureSet::GetListOfROI() const
// {
//   return mListOfROI;
// }
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRT_ROI* clitk::DicomRT_StructureSet::GetROIFromROINumber(int n)
{
  if (mROIs.find(n) == mROIs.end()) {
    std::cerr << "No ROI number " << n << std::endl;
    return NULL;
  }
  return mROIs[n];
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
clitk::DicomRT_ROI* clitk::DicomRT_StructureSet::GetROIFromROIName(const std::string& name)
{
  std::map<int, std::string>::iterator it = mMapOfROIName.begin();
  int number = -1;
  while (it != mMapOfROIName.end() && number == -1) {
    if (it->second == name)
      number = it->first;
    else
      it++;
  }

  if (number == -1) {
    std::cerr << "No ROI name " << name << std::endl;
    return NULL;
  }
  
  return mROIs[number];
}
//--------------------------------------------------------------------
/*
// RP: 08/02/2013
// RegEx version shall be available when C++x11 supports it propely
//
//--------------------------------------------------------------------
clitk::DicomRT_ROI* clitk::DicomRT_StructureSet::GetROIFromROINameRegEx(const std::string& regEx)
{
  std::map<int, std::string>::iterator it = mMapOfROIName.begin();
  int number = -1;

  while (it != mMapOfROIName.end() && number == -1) {
    if (std::tr1::regex_match (it->second, std::tr1::regex(regEx)))
      number = it->first;
    else
      it++;
  }

  if (number == -1) {
    std::cerr << "No ROI name " << number << std::endl;
    return NULL;
  }
  
  return mROIs[number];
}
//--------------------------------------------------------------------
*/
//--------------------------------------------------------------------
clitk::DicomRT_ROI* clitk::DicomRT_StructureSet::GetROIFromROINameSubstr(const std::string& s)
{
  std::map<int, std::string>::iterator it = mMapOfROIName.begin();
  int number = -1;

  while (it != mMapOfROIName.end() && number == -1) {
    if (it->second.find(s) != std::string::npos)
      number = it->first;
    else
      it++;
  }

  if (number == -1) {
    std::cerr << "No ROI name " << s << std::endl;
    return NULL;
  }
  
  return mROIs[number];
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
clitk::DicomRT_StructureSet::ROIMapContainer * 
clitk::DicomRT_StructureSet::GetROIsFromROINameSubstr(const std::string& s)
{
  static ROIMapContainer rois;
  rois.clear();
  
  ROIMapContainer::iterator it = mROIs.begin();
  int number = -1;

  while (it != mROIs.end()) {
    if (it->second->GetName().find(s) != std::string::npos) {
      number = it->first;
      rois[number] = it->second;
    }
    it++;
  }

  if (number == -1) {
    std::cerr << "No ROI name " << s << std::endl;
    return NULL;
  }
  
  return &rois;
  
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::DicomRT_StructureSet::Print(std::ostream & os) const
{
  os << "Study ID      = " << mStudyID << std::endl
     << "Study Date    = " << mStudyDate << std::endl
     << "Study Time    = " << mStudyTime << std::endl
     << "Struct Label  = " << mLabel << std::endl
     << "Struct Name   = " << mName << std::endl
     << "Struct Time   = " << mTime << std::endl
     << "Number of ROI = " << mROIs.size() << std::endl;
  for(ROIConstIteratorType iter = mROIs.begin(); iter != mROIs.end(); iter++) {
    iter->second->Print(os);
  }
}
//--------------------------------------------------------------------


#if GDCM_MAJOR_VERSION == 2
//--------------------------------------------------------------------
int clitk::DicomRT_StructureSet::ReadROINumber(const gdcm::Item & item)
{
  // 0x3006,0x0022 = [ROI Number]
  const gdcm::DataSet & nestedds = item.GetNestedDataSet();
  gdcm::Attribute<0x3006,0x0022> roinumber;
  roinumber.SetFromDataSet( nestedds );
  return roinumber.GetValue();  
}
//--------------------------------------------------------------------
#endif

//--------------------------------------------------------------------
void clitk::DicomRT_StructureSet::Write(const std::string & filename)
{
#if GDCM_MAJOR_VERSION == 2

  // Assert that the gdcm file is still open (we can write only if it was readed)
  if (mFile == NULL) {
    //assert(mFile != NULL);
    FATAL("Sorry, I can write DICOM only if it was read first from a file with 'Read' function");
  }

  // Loop and update each ROI 
  int i=0;
  for(ROIIteratorType iter = mROIs.begin(); iter != mROIs.end(); iter++) {
    iter->second->UpdateDicomItem();
    i++;
  }

  // Write [ Structure Set ROI Sequence ] = 0x3006,0x0020
  gdcm::DataSet & ds = mFile->GetDataSet();
  gdcm::Tag tssroisq(0x3006,0x0020);
  const gdcm::DataElement &ssroisq = ds.GetDataElement( tssroisq );
  gdcm::DataElement de(ssroisq);
  de.SetValue(*mROIInfoSequenceOfItems);
  ds.Replace(de);
  
  // Write [ ROI Contour Sequence ] = 0x3006,0x0039 
  DD("ici");
  gdcm::Tag troicsq(0x3006,0x0039);
  const gdcm::DataElement &roicsq = ds.GetDataElement( troicsq );
  gdcm::DataElement de2(roicsq);
  de2.SetValue(*mROIContoursSequenceOfItems);
  ds.Replace(de2);

  //DEBUG
  gdcm::DataSet & a = mROIContoursSequenceOfItems->GetItem(1).GetNestedDataSet();
  gdcm::Tag tcsq(0x3006,0x0040);
  const gdcm::DataElement& csq = a.GetDataElement( tcsq );
  gdcm::SmartPointer<gdcm::SequenceOfItems> sqi2 = csq.GetValueAsSQ();
  gdcm::Item & j = sqi2->GetItem(1);
  gdcm::DataSet & b = j.GetNestedDataSet();
  gdcm::Attribute<0x3006,0x0050> at;
  gdcm::Tag tcontourdata(0x3006,0x0050);
  gdcm::DataElement contourdata = b.GetDataElement( tcontourdata );
  at.SetFromDataElement( contourdata );
  const double* points = at.GetValues();
  DD(points[0]);


  // Write dicom
  gdcm::Writer writer;
  //writer.CheckFileMetaInformationOff();
  writer.SetFileName(filename.c_str());
  writer.SetFile(*mFile);
  DD("before write");
  writer.Write();
  DD("End write");
#else
  FATAL("Sorry not compatible with GDCM1, use GDCM2");
#endif
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_StructureSet::Read(const std::string & filename)
{

//Try to avoid to use extern GDCM library
    
  //check the RS file is available before conversion
  gdcm::Reader RTreader;
  RTreader.SetFileName( filename.c_str() );
  if( !RTreader.Read() ) 
  {
    std::cout << "Problem reading file: " << filename << std::endl;
    return;
  }
  
  const gdcm::DataSet& ds = RTreader.GetFile().GetDataSet();
  
  gdcm::MediaStorage ms;
  ms.SetFromFile( RTreader.GetFile() );
    
  // (3006,0020) SQ (Sequence with explicit length #=4)      # 370, 1 StructureSetROISequence  
  gdcm::Tag tssroisq(0x3006,0x0020);
  if( !ds.FindDataElement( tssroisq ) )
  {
    std::cout << "Problem locating 0x3006,0x0020 - Is this a valid RT Struct file?" << std::endl;
    return;
  }
  gdcm::Tag troicsq(0x3006,0x0039);
  if( !ds.FindDataElement( troicsq ) )
  {
    std::cout << "Problem locating 0x3006,0x0039 - Is this a valid RT Struct file?" << std::endl;
    return;
  }

  const gdcm::DataElement &roicsq = ds.GetDataElement( troicsq );

  gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = roicsq.GetValueAsSQ();
  if( !sqi || !sqi->GetNumberOfItems() )
  {
    return;
  }
  const gdcm::DataElement &ssroisq = ds.GetDataElement( tssroisq );
  gdcm::SmartPointer<gdcm::SequenceOfItems> ssqi = ssroisq.GetValueAsSQ();
  if( !ssqi || !ssqi->GetNumberOfItems() )
  {
    return;
  }
  
  // Read global info
  gdcm::Attribute<0x20,0x10> studyid;
  studyid.SetFromDataSet( ds );
  gdcm::Attribute<0x8,0x20> studytime;
  studytime.SetFromDataSet( ds );
  gdcm::Attribute<0x8,0x30> studydate;
  studydate.SetFromDataSet( ds );
  gdcm::Attribute<0x3006,0x02> label;
  label.SetFromDataSet( ds );
  gdcm::Attribute<0x3006,0x04> atname;
  atname.SetFromDataSet( ds );
  gdcm::Attribute<0x3006,0x09> time;
  time.SetFromDataSet( ds );

  mStudyID   = studyid.GetValue();
  mStudyTime = studytime.GetValue();
  mStudyDate = studydate.GetValue();
  mLabel     = label.GetValue();
  mName      = atname.GetValue();
  mTime      = time.GetValue();

  // Temporary store the list of items
  std::map<int, gdcm::Item*> mMapOfROIInfo;
  std::map<int, gdcm::Item*> mMapOfROIContours;
  

  //----------------------------------
  // Read all ROI Names and number
  // 0x3006,0x0020 = [ Structure Set ROI Sequence ]
  //gdcm::Tag tssroisq(0x3006,0x0020);
  //const gdcm::DataElement &ssroisq = ds.GetDataElement( tssroisq );
  mROIInfoSequenceOfItems = ssroisq.GetValueAsSQ();
  gdcm::SmartPointer<gdcm::SequenceOfItems> & roi_seq = mROIInfoSequenceOfItems;
  assert(roi_seq); // TODO error message
  for(unsigned int ridx = 0; ridx < roi_seq->GetNumberOfItems(); ++ridx)
    {
    gdcm::Item & item = roi_seq->GetItem( ridx + 1); // Item starts at 1
    const gdcm::DataSet& nestedds = item.GetNestedDataSet();

    gdcm::Attribute<0x3006,0x26> roiname;
    roiname.SetFromDataSet( nestedds );
    std::string name = roiname.GetValue(); // 0x3006,0x0026 = [ROI Name]

    // 0x3006,0x0022 = [ROI Number]
    int nb = ReadROINumber(item);

    // Store the item
    mMapOfROIInfo[nb] = &item;

    // Check if such a number already exist
    if (mMapOfROIName.find(nb) != mMapOfROIName.end()) {
      std::cerr << "WARNING. A Roi already exist with the number "
        << nb << ". I replace." << std::endl;
    }
    // Add in map
    mMapOfROIName[nb] = name;
    }

  //----------------------------------
  // Read all ROI item
  // 0x3006,0x0039 = [ ROI Contour Sequence ]
  //gdcm::Tag troicsq(0x3006,0x0039);
  //const gdcm::DataElement &roicsq = ds.GetDataElement( troicsq );
  gdcm::SmartPointer<gdcm::SequenceOfItems> roi_contour_seq = roicsq.GetValueAsSQ();
  mROIContoursSequenceOfItems = roi_contour_seq;
  assert(roi_contour_seq); // TODO error message
  for(unsigned int ridx = 0; ridx < roi_contour_seq->GetNumberOfItems(); ++ridx) {
    gdcm::Item & item = roi_contour_seq->GetItem( ridx + 1); // Item starts at 1
    // ROI number [Referenced ROI Number]
    const gdcm::DataSet& nestedds = item.GetNestedDataSet();
    gdcm::Attribute<0x3006,0x0084> referencedroinumber;
    referencedroinumber.SetFromDataSet( nestedds );
    int nb = referencedroinumber.GetValue();
    // Store the item
    mMapOfROIContours[nb] = &item;
  }

  //----------------------------------
  // Create the ROIs
  for(std::map<int, gdcm::Item*>::iterator i = mMapOfROIInfo.begin(); i != mMapOfROIInfo.end(); i++) {
    int nb = i->first;//ReadROINumber(i);//mROIIndex[i];
    // Create the roi
    mROIs[nb] = DicomRT_ROI::New();
    mROIs[nb]->SetTransformMatrix(mTransformMatrix);
    mROIs[nb]->Read(mMapOfROIInfo[nb], mMapOfROIContours[nb]);
  }
    
  return;

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool clitk::DicomRT_StructureSet::IsDicomRTStruct(const std::string & filename)
{
  // Open DICOM
#if GDCM_MAJOR_VERSION == 2
  // Read gdcm file
  mReader = new gdcm::Reader;
  mReader->SetFileName(filename.c_str());
  mReader->Read();
  mFile = &(mReader->GetFile());
  const gdcm::DataSet & ds = mFile->GetDataSet();
  
  // Check file type
  //Verify if the file is a RT-Structure-Set dicom file
  gdcm::MediaStorage ms;
  ms.SetFromFile(*mFile);
  if( ms != gdcm::MediaStorage::RTStructureSetStorage ) return false;

  gdcm::Attribute<0x8,0x60> modality;
  modality.SetFromDataSet( ds );
  if( modality.GetValue() != "RTSTRUCT" ) return false;
  
  return true;

  //----------------------------------------------------------------------------------------
#else
  mFile = new gdcm::File;
  mFile->SetFileName(filename.c_str());
  mFile->SetMaxSizeLoadEntry(16384); // Needed ...
  mFile->SetLoadMode(gdcm::LD_NOSHADOW); // don't load shadow tags (in order to save memory)
  mFile->Load();
  
  // Check file type
  //Verify if the file is a RT-Structure-Set dicom file
  if (!gdcm::Util::DicomStringEqual(mFile->GetEntryValue(0x0008,0x0016),"1.2.840.10008.5.1.4.1.1.481.3")) 
    return false;
  if (!gdcm::Util::DicomStringEqual(mFile->GetEntryValue(0x0008,0x0060),"RTSTRUCT"))
    return false;

  return true;

#endif
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
int clitk::DicomRT_StructureSet::AddBinaryImageAsNewROI(vvImage * im, std::string n)
{
  // Search max ROI number
  int max = -1;
  for(ROIConstIteratorType iter = mROIs.begin(); iter != mROIs.end(); iter++) {
    //  for(unsigned int i=0; i<mListOfROI.size(); i++) {
    clitk::DicomRT_ROI::Pointer roi = iter->second;
    if (roi->GetROINumber() > max)
      max = roi->GetROINumber();
  }
  ++max;

  // Compute name
  std::ostringstream oss;
  oss << vtksys::SystemTools::GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(n));
  mMapOfROIName[max] = oss.str();

  // Set color
  std::vector<double> color;
  color.push_back(1);
  color.push_back(0);
  color.push_back(0);

  // Create ROI
  DicomRT_ROI::Pointer roi = DicomRT_ROI::New();
  roi->SetFromBinaryImage(im, max, oss.str(), color, n);
  mROIs[max] = roi;
  return max;
}
//--------------------------------------------------------------------


