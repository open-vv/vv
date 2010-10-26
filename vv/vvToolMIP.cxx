/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolMIP.cxx,v $
  Language:  C++
  Date:      $Date: 2010/10/26 12:37:58 $
  Version:   $Revision: 1.1 $
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

#include <QMessageBox>
#include <QSpinBox>
#include <itkImage.h>
#include <itkMaximumProjectionImageFilter.h>

#include "vvToolMIP.h"
#include "vvSlicerManager.h"
#include "vvSlicer.h"
#include "vvToolInputSelectorWidget.h"
#include "clitkMIPGenericFilter.h"

//------------------------------------------------------------------------------
// Create the tool and automagically
ADD_TOOL(vvToolMIP);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvToolMIP::vvToolMIP(vvMainWindowBase * parent, Qt::WindowFlags f)
  :vvToolWidgetBase(parent,f),
   vvToolBase<vvToolMIP>(parent),
   Ui::vvToolMIP()
{
  // Setup the UI
  Ui_vvToolMIP::setupUi(mToolWidget);
  //mFilter=clitk::MIPGenericFilter::New(); //Causes a segfault. Why? (joel 26/10/2010)
  mFilter=new clitk::MIPGenericFilter;

  // Main filter
  // Set how many inputs are needed for this tool
  //AddInputSelector("Select one image", mFilter);
  AddInputSelector("Select one image");
}

//------------------------------------------------------------------------------
vvToolMIP::~vvToolMIP()
{
  delete mFilter;
}
//------------------------------------------------------------------------------
void vvToolMIP::Initialize()
{
  SetToolName("MIP");
  SetToolMenuName("Maximum Intensity Projection");
  SetToolIconFilename(":common/icons/ducky.png");
  SetToolTip("Compute the maximum intensity projection of an image.");
  SetToolExperimental(true);
}
//------------------------------------------------------------------------------

void vvToolMIP::apply()
{
  if (!mCurrentSlicerManager) close();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  // Main filter
  clitk::MIPGenericFilter::args_info_type args_info;
  cmdline_parser_clitkMIP_init(&args_info);
  args_info.dimension_arg=this->dimensionSpinBox->value();
  args_info.dimension_given=true;
  clitk::MIPGenericFilter* filter= dynamic_cast<clitk::MIPGenericFilter*>(mFilter);
  filter->SetArgsInfo(args_info);
  filter->SetInputVVImage(mCurrentImage);
  filter->Update();
  vvImage::Pointer output = filter->GetOutputVVImage();
  std::ostringstream osstream;
  osstream << "MIPed_" << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(output,osstream.str());
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------
void vvToolMIP::InputIsSelected(vvSlicerManager *m)
{
  mCurrentSlicerManager =m;
  this->dimensionSpinBox->setMaximum(m->GetDimension()-1);
}

//-----------------------------------------------------------------------------
