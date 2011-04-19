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
#include <algorithm>

#include <QApplication>

#include <gdcmFile.h>
#if GDCM_MAJOR_VERSION == 2
  #include <gdcmReader.h>
  #include <gdcmTag.h>
  #include <gdcmAttribute.h>
#else
  #include <gdcm.h>
  #include <gdcmSQItem.h>
#endif

#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkMetaImageWriter.h>
#include <vtkMetaImageReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkImageData.h>
#include <vtkImageCast.h>
#include <vtkImageGaussianSmooth.h>

#include "clitkCommon.h"
#include "vvMeshReader.h"
#include "vvProgressDialog.h"
#include <clitkCommon.h>

vvMeshReader::vvMeshReader() :
  vtk_mode(false)
{}

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

std::vector<std::pair<int,std::string> > vvMeshReader::GetROINames()
{
  assert(filename!="");
  std::vector<std::pair<int, std::string> > roi_names;
#if GDCM_MAJOR_VERSION == 2
  // duplicate code from  clitk::DicomRT_StructureSet::Read
  gdcm::Reader reader;
  reader.SetFileName( filename.c_str() );
  reader.Read();

  const gdcm::DataSet &ds = reader.GetFile().GetDataSet();

  gdcm::Tag tssroisq(0x3006,0x0020);
  const gdcm::DataElement &ssroisq = ds.GetDataElement( tssroisq );
  gdcm::SmartPointer<gdcm::SequenceOfItems> roi_seq = ssroisq.GetValueAsSQ();
  assert(roi_seq); // TODO error message
  for(unsigned int ridx = 0; ridx < roi_seq->GetNumberOfItems(); ++ridx)
    {
    gdcm::Item & item = roi_seq->GetItem( ridx + 1); // Item starts at 1
    const gdcm::DataSet& nestedds = item.GetNestedDataSet();
    if( nestedds.FindDataElement( gdcm::Tag(0x3006,0x22) ) )
      {
      gdcm::Attribute<0x3006,0x26> roiname;
      roiname.SetFromDataSet( nestedds );
      std::string name = roiname.GetValue();      // 0x3006,0x0026 = [ROI Name]
      gdcm::Attribute<0x3006,0x0022> roinumber;
      roinumber.SetFromDataSet( nestedds );
      int nb = roinumber.GetValue();  // 0x3006,0x0022 = [ROI Number]

      roi_names.push_back(make_pair(nb,name));
      }
    }
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
  return roi_names;
}

std::vector<vvMesh::Pointer> vvMeshReader::readSelectedContours()
{
  std::vector<vvMesh::Pointer> result;
#if GDCM_MAJOR_VERSION == 2
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

