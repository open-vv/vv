/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolBinarize.h,v $
  Language:  C++
  Date:      $Date: 2010/03/24 10:48:18 $
  Version:   $Revision: 1.10 $
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

#ifndef VVTOOLBINARIZE_H
#define VVTOOLBINARIZE_H

#include <QtDesigner/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvImageContour.h"
#include "ui_vvToolBinarize.h"
#include "clitkBinarizeImage_ggo.h"

//------------------------------------------------------------------------------
class vvToolBinarize:
  public vvToolWidgetBase,
  public vvToolBase<vvToolBinarize>, 
  private Ui::vvToolBinarize 
{
  Q_OBJECT
    public:
  vvToolBinarize(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolBinarize();

  //-----------------------------------------------------
  static void Initialize();
  void GetArgsInfoFromGUI();
  virtual void InputIsSelected(vvSlicerManager * m);

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  void valueChangedT1(double v);
  void valueChangedT2(double v);
  void UpdateSlice(int slicer,int slices);
  void enableLowerThan(bool b);
  void useFGBGtoggled(bool);
  void InteractiveDisplayToggled(bool b);

 protected:
  void RemoveVTKObjects();
  Ui::vvToolBinarize ui;
  args_info_clitkBinarizeImage mArgsInfo;
  std::vector<vvImageContour*> mImageContour;
  std::vector<vvImageContour*> mImageContourLower;
  bool mInteractiveDisplayIsEnabled;

}; // end class vvToolBinarize
//------------------------------------------------------------------------------

#endif

