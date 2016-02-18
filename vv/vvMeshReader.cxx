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

// std
#include <algorithm>

// qt
#include <QApplication>

// gdcm 
#include <gdcmFile.h>
#if GDCM_MAJOR_VERSION == 2
#include <gdcmReader.h>
#include <gdcmTag.h>
#include <gdcmAttribute.h>
#else
#include <gdcm.h>
#include <gdcmSQItem.h>
#endif

// vtk
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkMetaImageWriter.h>
#include <vtkMetaImageReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkImageData.h>
#include <vtkImageCast.h>
#include <vtkImageGaussianSmooth.h>

// clitk
#include "clitkCommon.h"
#include "clitkCommon.h"
#include "clitkDicomRT_StructureSet.h"
#include "vvMeshReader.h"
#include "vvProgressDialog.h"

//------------------------------------------------------------------------------
vvMeshReader::vvMeshReader() :
vtk_mode(false)
{}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMeshReader::Update()
{
  //Show a progress bar only when opening a DC-struct (ie. multiple contours)
  vvProgressDialog progress("Opening " + filename,(!vtk_mode) && (selected_contours.size()>1));
  this->start();
  while (this->isRunning()) {
    progress.SetProgress(result.size(),selected_contours.size());
    this->wait(50);
    qApp->processEvents();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvMeshReader::run()
{
  ///Verify the important stuff has been set
  assert(filename != "");
  if (vtk_mode) { //Read vtkPolyData
    vvMesh::Pointer m=vvMesh::New();
    m->ReadFromVTK(filename.c_str());
    if (vf) m->propagateContour(vf);
    m->ComputeMasks(image->GetVTKImages()[0],false); //don't extrude the contour
    result.push_back(m);
  } else { //Read a Dicom-struct file
    assert(selected_contours.size() > 0);
    assert(image);
    std::vector<vvMesh::Pointer> contour_stacks=readSelectedContours();
    for (std::vector<vvMesh::Pointer>::iterator i=contour_stacks.begin();
         i!=contour_stacks.end(); i++) {
      (*i)->ComputeMasks(image->GetVTKImages()[0],true); //Remesh the contour
      (*i)->ComputeMeshes();
      if (vf) (*i)->propagateContour(vf);
      result.push_back(*i);
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::vector<std::pair<int,std::string> > vvMeshReader::GetROINames()
{
  assert(filename!="");
  std::vector<std::pair<int, std::string> > roi_names;

#if CLITK_USE_SYSTEM_GDCM == 1

  // Read RT-struct data
  vtkSmartPointer<vtkGDCMPolyDataReader> areader = vtkGDCMPolyDataReader::New();
  areader->SetFileName(filename.c_str());
  areader->Update();

  // get info on roi names
  vtkRTStructSetProperties * p = areader->GetRTStructSetProperties();
  int n = p->GetNumberOfStructureSetROIs();
  
  for(unsigned int i=0; i<n; i++) {
    std::string name = p->GetStructureSetROIName(i);
    int nb = p->GetStructureSetROINumber(i);
    roi_names.push_back(make_pair(nb,name));
  }

#else
#if GDCM_MAJOR_VERSION == 2 

    // duplicate code from  clitk::DicomRT_StructureSet::Read
    gdcm::Reader * reader = new gdcm::Reader;
    reader->SetFileName( filename.c_str() );
    reader->Read();

    const gdcm::DataSet &ds = reader->GetFile().GetDataSet();

    // Check file type
    //Verify if the file is a RT-Structure-Set dicom file
    gdcm::File * mFile = &(reader->GetFile());
    gdcm::MediaStorage ms;
    ms.SetFromFile(*mFile);
    if( ms != gdcm::MediaStorage::RTStructureSetStorage )
      {
        std::cerr << "Error. the file " << filename
                  << " is not a Dicom Struct ? (must have a SOP Class UID [0008|0016] = 1.2.840.10008.5.1.4.1.1.481.3 ==> [RT Structure Set Storage])"
                  << std::endl;
        exit(0);
      }

    gdcm::Attribute<0x8,0x60> modality;
    modality.SetFromDataSet( ds );
    if( modality.GetValue() != "RTSTRUCT" )
      {
        std::cerr << "Error. the file " << filename
                  << " is not a Dicom Struct ? (must have 0x0008,0x0060 = RTSTRUCT [RT Structure Set Storage])"
                  << std::endl;
        exit(0);
      }

    gdcm::Attribute<0x20,0x10> studyid;
    studyid.SetFromDataSet( ds );

    gdcm::Tag tssroisq(0x3006,0x0020);
    // 0x3006,0x0020 = [ Structure Set ROI Sequence ]
    bool b = ds.FindDataElement(tssroisq);
    if (!b) { // FIXME
      clitkExceptionMacro("Error: tag 0x3006,0x0020 [ Structure Set ROI Sequence ] not found");
    }
  
    const gdcm::DataElement &ssroisq = ds.GetDataElement( tssroisq );
    gdcm::SmartPointer<gdcm::SequenceOfItems> roi_seq = ssroisq.GetValueAsSQ();
    assert(roi_seq); // FIXME error message
  
    for(unsigned int ridx = 0; ridx < roi_seq->GetNumberOfItems(); ++ridx)
      {
         gdcm::Item & item = roi_seq->GetItem( ridx + 1); // Item starts at 1

        const gdcm::Item & sitem = roi_seq->GetItem(ridx+1); // Item start at #1   

        const gdcm::DataSet& snestedds = sitem.GetNestedDataSet();
        const gdcm::DataSet& nestedds = item.GetNestedDataSet();

        if( snestedds.FindDataElement( gdcm::Tag(0x3006,0x22) ) )
          {
            // const gdcm::DataElement & a = nestedds.GetDataElement(gdcm::Tag(0x3006,0x26));
            // DD(a.GetValue());

            gdcm::Attribute<0x3006,0x26> roiname;
            roiname.SetFromDataSet( snestedds );
            std::string name = roiname.GetValue();      // 0x3006,0x0026 = [ROI Name]
            gdcm::Attribute<0x3006,0x0022> roinumber;
            roinumber.SetFromDataSet( snestedds );
            int nb = roinumber.GetValue();  // 0x3006,0x0022 = [ROI Number]
          
            roi_names.push_back(make_pair(nb,name));
          }
      }
  
    delete reader;

#else
  gdcm::File reader;
  reader.SetFileName(filename.c_str());
  reader.SetMaxSizeLoadEntry(16384);
  reader.Load();

  gdcm::SeqEntry * roi_info=reader.GetSeqEntry(0x3006,0x0020);
  assert(roi_info);
  // DD("ici");
  //int n=0;
  for (gdcm::SQItem* i=roi_info->GetFirstSQItem(); i!=0; i=roi_info->GetNextSQItem())
    if (i->GetEntryValue(0x3006,0x0022)!= gdcm::GDCM_UNFOUND)
      roi_names.push_back(make_pair(atoi(i->GetEntryValue(0x3006,0x0022).c_str()),i->GetEntryValue(0x3006,0x0026)));
#endif
#endif

  return roi_names;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::vector<vvMesh::Pointer> vvMeshReader::readSelectedContours()
{
  std::vector<vvMesh::Pointer> result;
#if GDCM_MAJOR_VERSION == 2
  gdcm::Reader reader;
  reader.SetFileName(filename.c_str());
  reader.Read();

  const gdcm::DataSet &ds = reader.GetFile().GetDataSet();

  gdcm::SmartPointer<gdcm::SequenceOfItems> rois = ds.GetDataElement(gdcm::Tag(0x3006,0x39)).GetValueAsSQ();
  gdcm::SmartPointer<gdcm::SequenceOfItems> roi_info = ds.GetDataElement(gdcm::Tag(0x3006,0x20)).GetValueAsSQ();
  assert(rois); // TODO error message
  assert(roi_info); // TODO error message
  assert(rois->GetNumberOfItems() == roi_info->GetNumberOfItems());

  for (unsigned ridx = 0; ridx < rois->GetNumberOfItems(); ++ridx)
    {
      vtkSmartPointer<vtkAppendPolyData> append=vtkSmartPointer<vtkAppendPolyData>::New();
      const gdcm::DataSet& ds_rois = rois->GetItem( ridx + 1).GetNestedDataSet();
      const gdcm::DataSet& ds_roi_info = roi_info->GetItem( ridx + 1).GetNestedDataSet();

      gdcm::Attribute<0x3006,0x84> roinumber;
      roinumber.SetFromDataSet(ds_rois);
      if (std::find(selected_contours.begin(), selected_contours.end(), roinumber.GetValue()) != selected_contours.end()) //Only read selected ROIs
        {
          gdcm::Attribute<0x3006,0x2a> trgb;
          trgb.SetFromDataSet(ds_rois);
          vvMesh::Pointer current_roi=vvMesh::New();
          current_roi->r = trgb[0] / 255.0;
          current_roi->g = trgb[1] / 255.0;
          current_roi->b = trgb[2] / 255.0;

          gdcm::Attribute<0x3006,0x26> tstructure_name;
          tstructure_name.SetFromDataSet(ds_roi_info);
          current_roi->structure_name = tstructure_name.GetValue();

          gdcm::SmartPointer<gdcm::SequenceOfItems> roi_seq = ds_rois.GetDataElement(gdcm::Tag(0x3006,0x40)).GetValueAsSQ();
          double z0=-1; //Used to determine spacing between slices, assumed to be constant
          for (unsigned j = 0; j < roi_seq->GetNumberOfItems(); ++j)
            {
              gdcm::Item & item_roi_seq = roi_seq->GetItem(j + 1); // Item starts at 1
              const gdcm::DataSet& ds_roi_seq = item_roi_seq.GetNestedDataSet();
              gdcm::Attribute<0x3006,0x42> tcontour_type;
              tcontour_type.SetFromDataSet(ds_roi_seq);
              std::string contour_type = tcontour_type.GetValue();
              if (contour_type=="CLOSED_PLANAR ")
                {
                  gdcm::Attribute<0x3006,0x46> tpoint_number;
                  tpoint_number.SetFromDataSet(ds_roi_seq);
                  const gdcm::DataElement & points_data = ds_roi_seq.GetDataElement(gdcm::Tag(0x3006,0x50));
                  gdcm::Attribute<0x3006,0x50> tpoints;
                  tpoints.SetFromDataElement(points_data);
                  assert(tpoints.GetNumberOfValues() == static_cast<unsigned int>(tpoint_number.GetValue()) * 3);
                  const double* points = tpoints.GetValues();
                  if (z0 == -1) //First contour
                    z0=points[2];
                  else
                    if (current_roi->GetSpacing()==-1 && points[2] != z0 )
                      current_roi->SetSpacing(points[2]-z0);
                  vtkPolyData * contour=vtkPolyData::New();
                  contour->Allocate(); //for cell structures
                  contour->SetPoints(vtkPoints::New());
                  vtkIdType ids[2];
                  for (unsigned idx = 0; idx < tpoints.GetNumberOfValues(); idx += 3)
                    {
                      contour->GetPoints()->InsertNextPoint(points[idx], points[idx+1], points[idx+2]);
                      ids[0] = idx / 3;
                      ids[1] = (ids[0] + 1) % tpoint_number.GetValue(); //0-1,1-2,...,n-1-0
                      contour->GetLines()->InsertNextCell(2, ids);
                    }
#if VTK_MAJOR_VERSION <= 5
                  append->AddInput(contour);
#else
                  append->AddInputData(contour);
#endif
                }
              else
                if (contour_type == "POINT ")
                  ; // silently ignore POINT type since we don't need them at the moment
                else
                  std::cerr << "Warning: contour type " << contour_type << " not handled!" << std::endl;
            }
          append->Update();
          current_roi->AddMesh(append->GetOutput());
          result.push_back(current_roi);
        }
      else
        {
          //std::cerr << "Warning: ignoring ROI #" << roi_number << std::endl;
        }
    }
#else
  gdcm::File reader;
  reader.SetFileName(filename.c_str());
  reader.SetMaxSizeLoadEntry(16384);
  reader.Load();

  gdcm::SeqEntry * rois=reader.GetSeqEntry(0x3006,0x0039);
  ///We need to iterate both on the contours themselves, and on the contour info
  gdcm::SeqEntry * roi_info=reader.GetSeqEntry(0x3006,0x0020);
  gdcm::SQItem* k=roi_info->GetFirstSQItem();
  for(gdcm::SQItem* i=rois->GetFirstSQItem(); i!=0; i=rois->GetNextSQItem()) { //loop over ROIS
    assert(k!=0);
    vtkSmartPointer<vtkAppendPolyData> append=vtkSmartPointer<vtkAppendPolyData>::New();
    std::istringstream ss(i->GetEntryValue(0x3006,0x0084));
    int roi_number;
    ss >> roi_number;
    if (std::find(selected_contours.begin(),selected_contours.end(),roi_number) != selected_contours.end()) { //Only read selected ROIs
      vvMesh::Pointer current_roi=vvMesh::New();
      std::vector<double> rgb=clitk::parse_string<double>(i->GetEntryValue(0x3006,0x002a),'\\');
      assert(rgb.size()==3);
      current_roi->r=rgb[0]/255;
      current_roi->g=rgb[1]/255;
      current_roi->b=rgb[2]/255;
      current_roi->structure_name=k->GetEntryValue(0x3006,0x0026);
      gdcm::SeqEntry * contours=i->GetSeqEntry(0x3006,0x0040);
      double z0=-1; //Used to determine spacing between slices, assumed to be constant
      for(gdcm::SQItem* j=contours->GetFirstSQItem(); j!=0; j=contours->GetNextSQItem()) { //loop over 2D contours
        std::string contour_type=j->GetEntryValue(0x3006,0x0042);
        if (contour_type=="CLOSED_PLANAR ") {
          int point_number=clitk::parse_value<int>(j->GetEntryValue(0x3006,0x0046));
          std::vector<float> points=clitk::parse_string<float>(j->GetEntryValue(0x3006,0x0050),'\\');
          assert(points.size() == static_cast<unsigned int>(point_number)*3);
          if (z0 == -1) //First contour
            z0=points[2];
          //2nd contour, spacing not yet set. Need to be sure we are on a different slice,
          //sometimes there is more than one closed contour per slice
          else if (current_roi->GetSpacing()==-1 && points[2] != z0 )
            current_roi->SetSpacing(points[2]-z0);
          vtkPolyData * contour=vtkPolyData::New();
          contour->Allocate(); //for cell structures
          contour->SetPoints(vtkPoints::New());
          vtkIdType ids[2];
          for (unsigned int idx=0; idx<points.size(); idx+=3) {
            contour->GetPoints()->InsertNextPoint(points[idx],points[idx+1],points[idx+2]);
            ids[0]=idx/3;
            ids[1]=(ids[0]+1)%point_number; //0-1,1-2,...,n-1-0
            contour->GetLines()->InsertNextCell(2,ids);
          }
          append->AddInput(contour);
        } else if (contour_type == "POINT ")
          ; // silently ignore POINT type since we don't need them at the moment
        else
          std::cerr << "Warning: contour type " << contour_type << " not handled!" << std::endl;
      }
      append->Update();
      current_roi->AddMesh(append->GetOutput());
      result.push_back(current_roi);
    } else {
      //std::cerr << "Warning: ignoring ROI #" << roi_number << std::endl;
    }
    k=roi_info->GetNextSQItem(); //increment the second loop variable
  }
#endif
  return result;
}

