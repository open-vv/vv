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

#include "clitkDicomRT_ROI.h"
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkImageClip.h>
#include <vtkMarchingSquares.h>
#include <vtkPolyDataWriter.h>

#if GDCM_MAJOR_VERSION == 2
#include "gdcmAttribute.h"
#include "gdcmItem.h"
#endif

//--------------------------------------------------------------------
clitk::DicomRT_ROI::DicomRT_ROI()
{
  mName = "NoName";
  mNumber = -1;
  mImage = NULL;
  mColor.resize(3);
  mColor[0] = mColor[1] = mColor[2] = 0;
  mMeshIsUpToDate = false;
  mBackgroundValue = 0;
  mForegroundValue = 1;
  SetDicomUptodateFlag(false);
  mFilename = "";
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRT_ROI::~DicomRT_ROI()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetDisplayColor(double r, double v, double b)
{
  mColor.resize(3);
  mColor[0] = r;
  mColor[1] = v;
  mColor[2] = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
int clitk::DicomRT_ROI::GetROINumber() const
{
  return mNumber;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_ROI::GetName() const
{
  return mName;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::DicomRT_ROI::GetFilename() const
{
  return mFilename;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::vector<double> & clitk::DicomRT_ROI::GetDisplayColor() const
{
  return mColor;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::Print(std::ostream & os) const
{
  os << "ROI " << mNumber << "\t" << mName
     << "\t(" << mColor[0] << " " << mColor[1] << " " << mColor[2] << ")"
     << "\t Contours = " << mListOfContours.size() << std::endl;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetBackgroundValueLabelImage(double bg)
{
  mBackgroundValue = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double clitk::DicomRT_ROI::GetBackgroundValueLabelImage() const
{
  return mBackgroundValue;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetForegroundValueLabelImage(double bg)
{
  mForegroundValue = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double clitk::DicomRT_ROI::GetForegroundValueLabelImage() const
{
  return mForegroundValue;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
#if GDCM_MAJOR_VERSION == 2
bool clitk::DicomRT_ROI::Read(gdcm::Item * itemInfo, gdcm::Item * itemContour)
{
  FATAL("Error : compile vv with itk4 + external gdcm");
  // Keep dicom item
  mItemInfo = itemInfo;
  mItemContour = itemContour;
  // DD(mItemInfo);
  
  // ROI number [Referenced ROI Number]
  const gdcm::DataSet & nesteddsInfo = mItemInfo->GetNestedDataSet();
  gdcm::Attribute<0x3006,0x0022> roinumber;
  roinumber.SetFromDataSet( nesteddsInfo );
  int nb1 = roinumber.GetValue();
  
  // Check this is the same with the other item
  const gdcm::DataSet & nestedds = mItemContour->GetNestedDataSet();
  gdcm::Attribute<0x3006,0x0084> referencedroinumber;
  referencedroinumber.SetFromDataSet( nestedds );
  int nb2 = referencedroinumber.GetValue();
  
  // Must never be different
  if (nb1 != nb2) {
    DD(nb2);
    DD(nb1);
    FATAL("nb1 must equal nb2" << std::endl);
  }
  mNumber = nb1;

  // Retrieve ROI Name (in the info item)
  gdcm::Attribute<0x3006,0x26> roiname;
  roiname.SetFromDataSet( nesteddsInfo );
  mName = roiname.GetValue();
  // DD(mName);

  // ROI Color [ROI Display Color]
  gdcm::Attribute<0x3006,0x002a> color = {};
  color.SetFromDataSet( nestedds );
  assert( color.GetNumberOfValues() == 3 );
  mColor[0] = color.GetValue(0);
  mColor[1] = color.GetValue(1);
  mColor[2] = color.GetValue(2);

  // Read contours [Contour Sequence]
  gdcm::Tag tcsq(0x3006,0x0040);
  if( !nestedds.FindDataElement( tcsq ) )
    {
      std::cerr << "Warning. Could not read contour for structure <" << mName << ">, number" << mNumber << " ? I ignore it" << std::endl;
      SetDicomUptodateFlag(true);
      return false;
    }
  const gdcm::DataElement& csq = nestedds.GetDataElement( tcsq );
  mContoursSequenceOfItems = csq.GetValueAsSQ();
  gdcm::SmartPointer<gdcm::SequenceOfItems> & sqi2 = mContoursSequenceOfItems;
  if( !sqi2 || !sqi2->GetNumberOfItems() )
    {
    }
  unsigned int nitems = sqi2->GetNumberOfItems();

  for(unsigned int i = 0; i < nitems; ++i)
    {
      gdcm::Item & j = sqi2->GetItem(i+1); // Item start at #1
      DicomRT_Contour::Pointer c = DicomRT_Contour::New();
      bool b = c->Read(&j);
      if (b) {
        mListOfContours.push_back(c);
      }
    }
  SetDicomUptodateFlag(true);
  return true;
}
#else
void clitk::DicomRT_ROI::Read(std::map<int, std::string> & rois, gdcm::SQItem * item)
{
  // ROI number [Referenced ROI Number]
  mNumber = atoi(item->GetEntryValue(0x3006,0x0084).c_str());

  // Retrieve ROI Name
  mName = rois[mNumber];

  // ROI Color [ROI Display Color]
  mColor = clitk::parse_string<double>(item->GetEntryValue(0x3006,0x002a),'\\');

  // Read contours [Contour Sequence]
  gdcm::SeqEntry * contours=item->GetSeqEntry(0x3006,0x0040);
  if (contours) {
    int i=0;
    for(gdcm::SQItem* j=contours->GetFirstSQItem(); j!=0; j=contours->GetNextSQItem()) {
      DicomRT_Contour::Pointer c = DicomRT_Contour::New();
      bool b = c->Read(j);
      if (b) {
        mListOfContours.push_back(c);
      }
      ++i;
    }
  }
  else {
    std::cerr << "Warning. Could not read contour for structure <" << mName << ">, number" << mNumber << " ? I ignore it" << std::endl;
  }
  SetDicomUptodateFlag(true);
}
#endif
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetImage(vvImage::Pointer image)
{
  mImage = image;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkPolyData * clitk::DicomRT_ROI::GetMesh()
{
  if (!mMeshIsUpToDate) {
    ComputeMeshFromContour();
  }
  return mMesh;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRT_Contour * clitk::DicomRT_ROI::GetContour(int n)
{
  return mListOfContours[n];
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::ComputeMeshFromContour()
{
  vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  for(unsigned int i=0; i<mListOfContours.size(); i++) {
    append->AddInput(mListOfContours[i]->GetMesh());
  }
  append->Update();
 
  mMesh = vtkSmartPointer<vtkPolyData>::New();
  mMesh->DeepCopy(append->GetOutput());
  mMeshIsUpToDate = true;
}
//--------------------------------------------------------------------


#if GDCM_MAJOR_VERSION == 2
//--------------------------------------------------------------------
void clitk::DicomRT_ROI::UpdateDicomItem()
{
  FATAL("Error : compile vv with itk4 + external gdcm");

  if (GetDicomUptoDateFlag()) return;
  DD("ROI::UpdateDicomItem");
  DD(GetName());  

  // From now, only some item can be modified

  // Set ROI Name 0x3006,0x26> 
  gdcm::Attribute<0x3006,0x26> roiname;
  roiname.SetValue(GetName());
  gdcm::DataElement de = roiname.GetAsDataElement();
  gdcm::DataSet & ds = mItemInfo->GetNestedDataSet();  
  ds.Replace(de);

  // From MESH to CONTOURS
  ComputeContoursFromImage();

  // Update contours
  DD(mListOfContours.size());
  for(uint i=0; i<mListOfContours.size(); i++) {
    DD(i);
    DicomRT_Contour::Pointer contour = mListOfContours[i];
    contour->UpdateDicomItem();//mItemContour);
  }

  // Nb of contours
  unsigned int nitems = mContoursSequenceOfItems->GetNumberOfItems();
  DD(nitems);

  // Write [Contour Sequence] = 0x3006,0x0040)
  gdcm::DataSet & dsc = mItemContour->GetNestedDataSet();
  gdcm::Tag tcsq(0x3006,0x0040);
  const gdcm::DataElement& csq = dsc.GetDataElement( tcsq );
  gdcm::DataElement dec(csq);
  dec.SetValue(*mContoursSequenceOfItems);
  dsc.Replace(dec);

  gdcm::DataSet & a = mContoursSequenceOfItems->GetItem(1).GetNestedDataSet();
  gdcm::Attribute<0x3006,0x0050> at;
  gdcm::Tag tcontourdata(0x3006,0x0050);
  gdcm::DataElement contourdata = a.GetDataElement( tcontourdata );
  at.SetFromDataElement( contourdata );
  const double* points = at.GetValues();
  DD(points[0]);

}
//--------------------------------------------------------------------
#endif

//--------------------------------------------------------------------
void clitk::DicomRT_ROI::SetFromBinaryImage(vvImage::Pointer image, int n,
                                            std::string name,
                                            std::vector<double> color, 
                                            std::string filename)
{

  // ROI number [Referenced ROI Number]
  mNumber = n;

  // ROI Name
  mName = name;
  mFilename = filename;

  // ROI Color [ROI Display Color]
  mColor = color;

  // No contours [Contour Sequence]
  mListOfContours.clear();

  // Set image
  mImage = image;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vvImage * clitk::DicomRT_ROI::GetImage() const
{
  return mImage;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI::ComputeContoursFromImage()
{
  FATAL("ComputeContoursFromImage should not be call. To be replace");
  DD("ComputeMeshFromImage");

  // Check that an image is loaded
  if (!mImage) return;

  // Only consider 3D here
  if (mImage->GetNumberOfDimensions() != 3) {
    FATAL("DicomRT_ROI::ComputeMeshFromImage only work with 3D images");
  }

  // Get the VTK image
  vtkImageData * image = mImage->GetVTKImages()[0];
  
  // Get initial extend for the clipping
  vtkSmartPointer<vtkImageClip> clipper = vtkSmartPointer<vtkImageClip>::New();
  clipper->SetInput(image);
  int* extent = image->GetExtent();
  DDV(extent, 6);
  //  std::vector<int> extend;


  // Loop on slice
  uint n = image->GetDimensions()[2];
  DD(n);
  DD(mListOfContours.size());
  mListOfContours.resize(n); /// ???FIXME
  DD(mListOfContours.size());
  std::vector<vtkSmartPointer<vtkPolyData> > contours;
  for(uint i=0; i<n; i++) {
    DD(i);

    // FIXME     vtkDiscreteMarchingCubes INSTEAD


    vtkSmartPointer<vtkMarchingSquares> squares = vtkSmartPointer<vtkMarchingSquares>::New();
    squares->SetInput(image);
    squares->SetImageRange(extent[0], extent[1], extent[2], extent[3], i, i);
    squares->SetValue(1, 1.0);
    squares->Update();
    DD(squares->GetNumberOfContours());
    
    //clitk::DicomRT_Contour * contour = new clitk::DicomRT_Contour();
    //mListOfContours[i]->SetMesh(squares->GetOutput());

    
    vtkSmartPointer<vtkPolyData> m = squares->GetOutput();
    contours.push_back(m);

    /*
    // Clip to the current slice
    extent[4] = extent[5] = image->GetOrigin()[2]+i*image->GetSpacing()[2];
    DDV(extent, 6);
    // Prepare the marching squares
    vtkSmartPointer<vtkMarchingSquares> squares = vtkSmartPointer<vtkMarchingSquares>::New();
    clipper->SetOutputWholeExtent(extent[0],extent[1],extent[2],
                                extent[3],extent[4],extent[5]); 

    squares->SetInput(clipper->GetOutput());
    squares->Update();
    DD(squares->GetNumberOfContours());
    mListOfContours[i]->SetMesh(squares->GetOutput());
    */
  }
  DD("done");
 
  vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  for(unsigned int i=0; i<n; i++) {
    append->AddInput(contours[i]);
  }
  append->Update();
 
  mMesh = vtkSmartPointer<vtkPolyData>::New();
  mMesh->DeepCopy(append->GetOutput());
  
  // Write vtk
  vtkPolyDataWriter * w = vtkPolyDataWriter::New();
  w->SetInput(mMesh);
  w->SetFileName("toto.vtk");
  w->Write();

  DD("done");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
#if CLITK_USE_SYSTEM_GDCM == 1
void clitk::DicomRT_ROI::Read(vtkSmartPointer<vtkGDCMPolyDataReader> & reader, int roiindex)
{
  vtkRTStructSetProperties * p = reader->GetRTStructSetProperties();
  
  mName = p->GetStructureSetROIName(roiindex);
  mNumber = p->GetStructureSetROINumber(roiindex);

  //mColor = //FIXME !!  

  // gdcm::Attribute<0x3006,0x002a> color = {};
  
  // const gdcm::DataSet & nestedds = mItemContour->GetNestedDataSet();
  // color.SetFromDataSet( nestedds );
  // assert( color.GetNumberOfValues() == 3 );
  // mColor[0] = color.GetValue(0);
  // mColor[1] = color.GetValue(1);
  // mColor[2] = color.GetValue(2);


  SetDicomUptodateFlag(true);
  // Get the contour
  mMesh =  reader->GetOutput(roiindex);  
  DicomRT_Contour::Pointer c = DicomRT_Contour::New();
  c->SetMesh(mMesh); // FIXME no GetZ, not GetPoints  
  mMeshIsUpToDate = true;
  mListOfContours.push_back(c);
}
#endif
//--------------------------------------------------------------------

