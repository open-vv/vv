    /*=========================================================================

      Program:   vv
      Module:    $RCSfile: vvToolMedianFilter.cxx,v $
      Language:  C++
      Date:      $Date: 2010/04/26 18:21:55 $
      Version:   $Revision: 1.2 $
      Author :   Bharath Navalpakkam (Bharath.Navalpakkam@creatis.insa-lyon.fr)

      Copyright (C) 2010
      Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
      CREATIS                   http://www.creatis.insa-lyon.fr

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, version 3 of the License.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>.

      =========================================================================*/

    #include "vvToolMedianFilter.h"
    #include "vvSlicerManager.h"
    #include "vvSlicer.h"
    #include "vvToolInputSelectorWidget.h"
    #include <clitkMedianImageGenericFilter.h>


    //------------------------------------------------------------------------------
    // Create the tool and automagically 
    ADD_TOOL(vvToolMedianFilter);
    //------------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    vvToolMedianFilter::vvToolMedianFilter(vvMainWindowBase * parent, Qt::WindowFlags f) 
      :vvToolWidgetBase(parent,f), 
      vvToolBase<vvToolMedianFilter>(parent), 
      Ui::vvToolMedianFilter()
      {		
	
      // Setup the UI
    
    Ui_vvToolMedianFilter::setupUi(mToolWidget);
    
    mFilter = new clitk::MedianImageGenericFilter<args_info_clitkMedianImageFilter>;
    


      // Main filter 

      // Set how many inputs are needed for this tool
  AddInputSelector("Select one image", mFilter);
    }

    //------------------------------------------------------------------------------
    vvToolMedianFilter::~vvToolMedianFilter() {
    }
    //------------------------------------------------------------------------------
    void vvToolMedianFilter::Initialize() {
      SetToolName("MedianFilter");
      SetToolMenuName("MedianFilter");
      SetToolIconFilename(":common/icons/ducky.png");
      SetToolTip("Make 'MedianFilter' on an image.");
    }
    //------------------------------------------------------------------------------

  void vvToolMedianFilter::apply() {

     GetArgsInfoFromGUI();
    if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  // Main filter
  clitk::MedianImageGenericFilter<args_info_clitkMedianImageFilter>::Pointer filter = 
     clitk::MedianImageGenericFilter<args_info_clitkMedianImageFilter>::New();
   filter->SetInputVVImage(mCurrentImage);
   filter->SetArgsInfo(mArgsInfo);
   filter->EnableReadOnDisk(false);
   filter->Update();
  // Output
  vvImage::Pointer output = filter->GetOutputVVImage();
  std::ostringstream osstream;
  osstream << "MedianFiltered_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(output,osstream.str()); 
  QApplication::restoreOverrideCursor();
  close();
  }
    //------------------------------------------------------------------------------
  void vvToolMedianFilter::GetArgsInfoFromGUI() {
    /* //KEEP THIS FOR READING GGO FROM FILE
      int argc=1;
      std::string a = "toto";
      char * const* argv = new char*;
      //a.c_str();
      struct cmdline_parser_params p;
      p.check_required = 0;
      int good = cmdline_parser_ext(argc, argv, &args_info, &p);
    DD(good);
    */
    mArgsInfo.radius_given=0;
    mArgsInfo.verbose_flag = false;
   // mArgsInfo.radius_arg = new int[3];
  // Required (even if not used)
    mArgsInfo.input_given = 0;
    mArgsInfo.output_given = 0;
    mArgsInfo.input_arg=new char;
    mArgsInfo.output_arg = new char;
    mArgsInfo.config_given=0;
    mArgsInfo.verbose_given=0;
  }
  //------------------------------------------------------------------------------
void vvToolMedianFilter::InputIsSelected(vvSlicerManager *m){
  mCurrentSlicerManager =m;
  // Specific for this gui
    mArgsInfo.radius_arg = new int[3];
  int checkdimensions=mCurrentSlicerManager->GetDimension();
  if(checkdimensions<3)
  {
   horizontalSlider_3->hide();
   spinBox_3->hide();
   mArgsInfo.radius_arg[2]=0;
   connect(horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateH1slider()));
   connect(horizontalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(UpdateH2slider())); 
  }
  else
  {
  horizontalSlider->show();
  horizontalSlider_2->show();  
  horizontalSlider_3->show();
  connect(horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateH1slider()));
  connect(horizontalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(UpdateH2slider()));
  connect(horizontalSlider_3, SIGNAL(valueChanged(int)), this, SLOT(UpdateH3slider()));   
  }
}

//-----------------------------------------------------------------------------

void vvToolMedianFilter::UpdateH1slider()
{
  
  mArgsInfo.radius_arg[0]=horizontalSlider->value();
  spinBox->setValue(mArgsInfo.radius_arg[0]);
}
void vvToolMedianFilter::UpdateH2slider()
{
  mArgsInfo.radius_arg[1]=horizontalSlider_2->value(); 
  spinBox_2->setValue(mArgsInfo.radius_arg[1]);
}
void vvToolMedianFilter::UpdateH3slider()
{
  mArgsInfo.radius_arg[2]=horizontalSlider_3->value(); 
  spinBox_3->setValue(mArgsInfo.radius_arg[2]);
}
