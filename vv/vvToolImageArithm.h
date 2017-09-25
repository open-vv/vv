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
#ifndef VVTOOLImageArithm_H
#define VVTOOLImageArithm_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif

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

