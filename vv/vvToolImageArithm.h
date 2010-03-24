/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolImageArithm.h,v $
  Language:  C++
  Date:      $Date: 2010/03/24 13:02:59 $
  Version:   $Revision: 1.1 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

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

#ifndef VVTOOLImageArithm_H
#define VVTOOLImageArithm_H

#include <QtDesigner/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "ui_vvToolImageArithm.h"
#include "clitkImageArithm_ggo.h"

//------------------------------------------------------------------------------
class vvToolImageArithm:
  public vvToolWidgetBase,
  public vvToolBase<vvToolImageArithm>, 
  private Ui::vvToolImageArithm
{
  Q_OBJECT
    public:
  vvToolImageArithm(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
  ~vvToolImageArithm();

  static void Initialize();
  void GetArgsInfoFromGUI();
  virtual void InputIsSelected(std::vector<vvSlicerManager *> & m);
  virtual void InputIsSelected(vvSlicerManager * m);

public slots:
  virtual void apply();

 protected:
  Ui::vvToolImageArithm ui;
  vvSlicerManager * mInput1;
  vvSlicerManager * mInput2;
  args_info_clitkImageArithm mArgsInfo;
  bool mTwoInputs;

}; // end class vvToolImageArithm
//------------------------------------------------------------------------------

#endif

