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

