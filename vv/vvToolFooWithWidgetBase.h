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
#ifndef VVTOOLFOO_H
#define VVTOOLFOO_H
#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "ui_vvToolFoo.h"

//------------------------------------------------------------------------------
class vvToolFooWithWidgetBase:
  public vvToolWidgetBase,
  public vvToolBase<vvToolFooWithWidgetBase>, 
  private Ui::vvToolFoo
{
  Q_OBJECT
    public:
  vvToolFooWithWidgetBase(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
  ~vvToolFooWithWidgetBase();

  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager *m);
  //  OR ===> virtual void InputIsSelected(std::vector<vvSlicerManager *> & m);

public slots:
  virtual void apply();

 protected:
  Ui::vvToolFoo ui;

}; // end class vvToolFooWithWidgetBase
//------------------------------------------------------------------------------

#endif

