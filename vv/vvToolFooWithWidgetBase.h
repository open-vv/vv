#ifndef VVTOOLFOO_H
#define VVTOOLFOO_H
#include <QtDesigner/QDesignerExportWidget>

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

