/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

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
  vvMainWindowBase * mMainWindow;
  std::vector<vvSlicerManager*> mSlicerManagersCompatible;
  vvSlicerManager * mCurrentSlicerManager;
  int mCurrentCompatibleIndex;
  vvImage * mCurrentImage;
  bool mIsInitialized;

}; // end class vvToolWidgetBase
//------------------------------------------------------------------------------

#endif

