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
===========================================================================*/
#include "clitkBinaryImageToMesh_ggo.h"
#include "clitkCommon.h"

#include "vtkMetaImageReader.h"
#include "vtkContourFilter.h"
#include "vtkDecimatePro.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkOBJExporter.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"

#include "itksys/SystemTools.hxx"

#include "vtkPolyDataWriter.h"
#include "vtkSmoothPolyDataFilter.h"

void run(const args_info_clitkBinaryImageToMesh& argsInfo);

int main(int argc, char** argv)
{
  GGO(clitkBinaryImageToMesh, args_info);

  run(args_info);
  
  return EXIT_SUCCESS;
}

void run(const args_info_clitkBinaryImageToMesh& argsInfo)
{
    std::string file = argsInfo.input_arg;
   
    vtkSmartPointer<vtkMetaImageReader> pbmp_reader = vtkMetaImageReader::New();
    pbmp_reader->SetFileName(file.c_str());
    pbmp_reader->Update();

    printf("Filtering...\n");
    vtkSmartPointer<vtkContourFilter> pcontour = vtkContourFilter::New();
    pcontour->SetValue(0, 0.5);
    pcontour->SetInputConnection(pbmp_reader->GetOutputPort());

    vtkAlgorithmOutput *data = pcontour->GetOutputPort();

    if ( (argsInfo.decimate_arg>=0) && (argsInfo.decimate_arg<=1) ) {
      vtkSmartPointer<vtkDecimatePro> psurface = vtkDecimatePro::New();
      psurface->SetInputConnection(pcontour->GetOutputPort());
      psurface->SetTargetReduction(argsInfo.decimate_arg);

      data = psurface->GetOutputPort();
    }

	
    vtkSmartPointer<vtkPolyDataMapper> skinMapper = vtkPolyDataMapper::New();
    skinMapper->SetInputConnection(data); //psurface->GetOutputPort()
    skinMapper->ScalarVisibilityOff();
      
    vtkSmartPointer<vtkActor> skin = vtkActor::New();
    skin->SetMapper(skinMapper);
      
    vtkSmartPointer<vtkCamera> aCamera = vtkCamera::New();
    aCamera->SetViewUp (0, 0, -1);
    aCamera->SetPosition (0, 1, 0);
    aCamera->SetFocalPoint (0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    aCamera->Dolly(1.5);

    vtkSmartPointer<vtkRenderer> aRenderer = vtkRenderer::New();
    aRenderer->AddActor(skin);
    aRenderer->SetActiveCamera(aCamera);
    aRenderer->ResetCamera ();
    aRenderer->SetBackground(0,0,0);
    aRenderer->ResetCameraClippingRange ();

    vtkSmartPointer<vtkRenderWindow> renWin = vtkRenderWindow::New();
    renWin->AddRenderer(aRenderer);
    renWin->SetSize(640, 480);
 
    std::string output;
    if (argsInfo.output_given) { output = argsInfo.output_arg; }

    bool writeVTK = false;
    if (output.length()>4) {
      if (output.compare(output.length()-4, 4, ".vtk")==0) {
        writeVTK=true;
      }
    }
    if (writeVTK) {
      vtkSmartPointer<vtkPolyDataWriter> wr = vtkSmartPointer<vtkPolyDataWriter>::New();
      wr->SetInputConnection(data); //psurface->GetOutputPort()
      wr->SetFileName(output.c_str());
      wr->Update();
      wr->Write();
    }
    else {
      vtkSmartPointer<vtkOBJExporter> pwriter2 = vtkOBJExporter::New();
      pwriter2->SetRenderWindow(renWin);
  
      if (argsInfo.output_given) {
        output = argsInfo.output_arg;
        if (itksys::SystemTools::FileIsDirectory(output.c_str())) {
          file = itksys::SystemTools::GetFilenameName(file.c_str());
          file = itksys::SystemTools::GetFilenameWithoutExtension(file.c_str());
          file = itksys::SystemTools::CollapseFullPath(file.c_str(), output.c_str());
        }
        else {
          file = output;
        }
      }
      else { 
        file = itksys::SystemTools::GetFilenameWithoutExtension(file);
      }
      pwriter2->SetFilePrefix(file.c_str());
      pwriter2->Write();
    }


    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    skinMapper->Update();
    bool interact = argsInfo.view_flag;
    if (interact)
    {
      iren->Initialize();
      iren->Start(); 
    }
    else
      renWin->Render();
}


