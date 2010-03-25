#ifndef VVTOOLWIDGETBASE_H
#define VVTOOLWIDGETBASE_H
#include <QtDesigner/QDesignerExportWidget>
#include "ui_vvToolWidgetBase.h"
#include "clitkImageToImageGenericFilter.h"
class vvMainWindowBase;

//------------------------------------------------------------------------------
class vvToolWidgetBase:
  public QDialog, 
  public Ui::vvToolWidgetBase 
{
  Q_OBJECT
  public:

  vvToolWidgetBase(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolWidgetBase();

  virtual void InputIsSelected(vvSlicerManager *m);
  virtual void InputIsSelected(std::vector<vvSlicerManager*> & l);
  void AddInputSelector(QString s, clitk::ImageToImageGenericFilterBase * f, bool allowSkip=false);
  void AddInputSelector(QString s, bool allowSkip=false);

public slots:
  virtual void apply() = 0;
  virtual bool close();
  void InputIsSelected();
  void AnImageIsBeingClosed(vvSlicerManager * m);
  void show();

protected:
  void InitializeInputs();
  Ui::vvToolWidgetBase ui;
  clitk::ImageToImageGenericFilterBase * mFilter;
  vvMainWindowBase * mMainWindowBase;
  std::vector<vvSlicerManager*> mSlicerManagersCompatible;
  vvSlicerManager * mCurrentSlicerManager;
  int mCurrentCompatibleIndex;
  vvImage * mCurrentImage;
  bool mIsInitialized;

}; // end class vvToolWidgetBase
//------------------------------------------------------------------------------

#endif

