#ifndef VVTOOLFOO_H
#define VVTOOLFOO_H
#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>

#include "vvToolBase.h"
#include "ui_vvToolFoo.h"

//------------------------------------------------------------------------------
class vvToolFoo:
  public QDialog,
  public vvToolBase<vvToolFoo>, 
  private Ui::vvToolFoo
{
  Q_OBJECT
    public:
  vvToolFoo(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
  ~vvToolFoo();

  static void Initialize();

 protected:
  Ui::vvToolFoo ui;

}; // end class vvToolFoo
//------------------------------------------------------------------------------

#endif

