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

#include "vtkOBJReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtkAnimationCue.h"
#include "vtkAnimationScene.h"
#include "vtkProperty.h"
#include "vtkInteractorStyle.h"

#include <string>
#include <iostream>
#include <vector>

typedef vtkSmartPointer<vtkOBJReader> ObjReaderType;
typedef vtkSmartPointer<vtkPolyDataMapper> MapperType;
typedef vtkSmartPointer<vtkActor> ActorType;


void run(int argc, char** argv);

// Adapted from vtkAnimationCue example...
class CueAnimator
{
public:
  CueAnimator(std::vector<ActorType>& rActors) : m_Fps(1), m_CurrentActor(0), m_rActors(rActors) {
    m_rActors[0]->SetVisibility(1);
    for (unsigned int i = 1; i < m_rActors.size(); i++) {
      m_rActors[i]->SetVisibility(0);
    }
  }
  
  ~CueAnimator() {
   }
  
  void SetFps(double fps) {
    m_Fps = fps;
  }
  
  void StartCue(vtkAnimationCue::AnimationCueInfo *vtkNotUsed(info),
                vtkRenderer *ren)
    {
      //std::cout << "StartCue" << std::endl;
    }
  
  void Tick(vtkAnimationCue::AnimationCueInfo *info,
            vtkRenderer *ren)
    {
      //std::cout << "Tick AT:" << info->AnimationTime << " DT:" << info->DeltaTime << " CT:" << info->ClockTime << std::endl;
      
      m_rActors[m_CurrentActor]->SetVisibility(0);
      
      int step = round(m_Fps * info->AnimationTime);
      int actor = step % m_rActors.size();
      
      //if (actor != m_CurrentActor) std::cout << "Showing frame: " << m_CurrentActor << std::endl;
      m_CurrentActor = actor;
      m_rActors[m_CurrentActor]->SetVisibility(1);
      
      ren->Render();
    }
  
  void EndCue(vtkAnimationCue::AnimationCueInfo *info,
              vtkRenderer *ren)
    {
      //std::cout << "EndCue" << std::endl;
    }
  
protected:
  
  double m_Fps;
  clock_t m_LastTick;
  double m_TotalTicks;
  int m_CurrentActor;
  std::vector<ActorType>& m_rActors;
};

class vtkAnimationCueObserver : public vtkCommand
{
public:
  static vtkAnimationCueObserver *New()
    {
      return new vtkAnimationCueObserver;
    }
  
  virtual void Execute(vtkObject *vtkNotUsed(caller),
                       unsigned long event,
                       void *calldata)
    {
      if(this->Animator!=0 && this->Renderer!=0)
        {
        vtkAnimationCue::AnimationCueInfo *info=
          static_cast<vtkAnimationCue::AnimationCueInfo *>(calldata);
        switch(event)
          {
          case vtkCommand::StartAnimationCueEvent:
            this->Animator->StartCue(info,this->Renderer);
            break;
          case vtkCommand::EndAnimationCueEvent:
            this->Animator->EndCue(info,this->Renderer);
            break;
          case vtkCommand::AnimationCueTickEvent:
            this->Animator->Tick(info,this->Renderer);
            break;
          }
        }
      if(this->RenWin!=0)
        {
        this->RenWin->Render();
        }
    }
  
  vtkRenderer *Renderer;
  vtkRenderWindow *RenWin;
  CueAnimator *Animator;
  
protected:
  vtkAnimationCueObserver()
    {
      this->Renderer=0;
      this->Animator=0;
      this->RenWin=0;
    }
};

class vtkWindowObserver : public vtkCommand
{
public:
  static vtkWindowObserver *New()
    {
      return new vtkWindowObserver;
    }

  virtual void Execute(vtkObject *caller,
                       unsigned long event,
                       void *calldata)
    {
      vtkRenderWindowInteractor *isi = dynamic_cast<vtkRenderWindowInteractor *>(caller);
      //std::cout << "Execute" << std::endl;
      switch (event)
      {
        case vtkCommand::KeyPressEvent:
        {
          std::string key = isi->GetKeySym();
          //std::cout << key[0] << std::endl;
          switch (key[0])
          {
            case 'P':
            case 'p':
              if (this->m_Scene)
                this->m_Scene->Play();
              break;
          
            case 'M':
            case 'm':
              if (this->m_Scene)
                this->m_Scene->Stop();
              break;
          
            default:
              break;
          }
        }
          
        default:
          break;
          
      }
      
    }
    
    vtkAnimationScene* m_Scene;
    
  protected:
    
    vtkWindowObserver() : m_Scene(0) {}
};

int main(int argc, char** argv)
{
  if (argc == 0)
    std::cout << "Usage: clitkMeshViewer FILE1 FILE2 ... [--animate]" << std::endl;

  run(argc, argv);
  
  return EXIT_SUCCESS;
}

void run(int argc, char** argv)
{
  std::vector<ObjReaderType> objs;
  std::vector<MapperType> mappers;
  std::vector<ActorType> actors;
  
  int nfiles = argc;
  std::string animate = argv[argc-1];
  if (animate == "--animate")
    nfiles = argc-1;
  
  vtkSmartPointer<vtkRenderer> aRenderer = vtkRenderer::New();
  for (int i = 1; i < nfiles; i++) {    
    std::string file = argv[i];
    
    //std::cout << "Reading " << file << std::endl;
    
    vtkSmartPointer<vtkOBJReader> preader = vtkOBJReader::New();
    preader->SetFileName(file.c_str());
    preader->Update();
    objs.push_back(preader);
    
    vtkSmartPointer<vtkPolyDataMapper> skinMapper = vtkPolyDataMapper::New();
    skinMapper->SetInputConnection(preader->GetOutputPort());
    skinMapper->ScalarVisibilityOff();
    mappers.push_back(skinMapper);

    vtkSmartPointer<vtkActor> skin = vtkActor::New();
    skin->SetMapper(skinMapper);
    actors.push_back(skin);

    aRenderer->AddActor(skin);
  }

  vtkSmartPointer<vtkCamera> aCamera = vtkCamera::New();
  aCamera->SetViewUp (0, 0, -1);
  aCamera->SetPosition (0, 1, 0);
  aCamera->SetFocalPoint (0, 0, 0);
  aCamera->ComputeViewPlaneNormal();
  aCamera->Dolly(1.5);

  aRenderer->SetActiveCamera(aCamera);
  aRenderer->ResetCamera ();
  aRenderer->SetBackground(0,0,0);
  aRenderer->ResetCameraClippingRange ();

  vtkSmartPointer<vtkRenderWindow> renWin = vtkRenderWindow::New();
  renWin->AddRenderer(aRenderer);
  renWin->SetSize(640, 480);
  
  vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  iren->Initialize();
  
  vtkSmartPointer<vtkAnimationScene> scene;
  vtkSmartPointer<vtkAnimationCue> cue1;
  vtkSmartPointer<vtkAnimationCueObserver> anim_observer;
  vtkSmartPointer<vtkWindowObserver> window_observer;
  CueAnimator animator(actors);
  
  double fps = 4;
  animator.SetFps(fps);

  if (animate == "--animate") {
    // Create an Animation Scene
    scene=vtkAnimationScene::New();
    scene->SetModeToRealTime();

    scene->SetLoop(1);
    scene->SetTimeModeToRelative();
    scene->SetStartTime(0);
    scene->SetEndTime(actors.size()/fps);
    
    // Create an Animation Cue.
    cue1=vtkAnimationCue::New();
    cue1->SetTimeModeToRelative();
    cue1->SetStartTime(0);
    cue1->SetEndTime(actors.size()/fps);
    scene->AddCue(cue1);
    
    // Create Cue anim_observer.
    anim_observer=vtkAnimationCueObserver::New();
    anim_observer->Renderer=aRenderer;
    anim_observer->Animator=&animator;
    anim_observer->RenWin=renWin;
    cue1->AddObserver(vtkCommand::StartAnimationCueEvent,anim_observer);
    cue1->AddObserver(vtkCommand::EndAnimationCueEvent,anim_observer);
    cue1->AddObserver(vtkCommand::AnimationCueTickEvent,anim_observer);

    window_observer = vtkWindowObserver::New();
    window_observer->m_Scene = scene;
    iren->AddObserver(vtkCommand::KeyPressEvent, window_observer);
    
  }

  iren->Start(); 
}


