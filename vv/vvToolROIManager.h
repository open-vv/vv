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

#ifndef VVTOOLROIMANAGER_H
#define VVTOOLROIMANAGER_H

#include <QtDesigner/QDesignerExportWidget>
#include <QSharedPointer>

#include "vvToolBase.h"
#include "vvROIActor.h"
#include "clitkDicomRT_ROI.h"
#include "ui_vvToolROIManager.h"

class vvROIActor;

//------------------------------------------------------------------------------
class vvToolROIManager:
  public QWidget, 
  public vvToolBase<vvToolROIManager>, 
  public Ui::vvToolROIManager
{
  Q_OBJECT
  public:
  vvToolROIManager(vvMainWindowBase* parent=0, Qt::WindowFlags f=Qt::Dialog);
  virtual ~vvToolROIManager();

  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager *m);

  public slots:
  void AnImageIsBeingClosed(vvSlicerManager *);
  void SelectedImageHasChanged(vvSlicerManager *);

protected:
  Ui::vvToolROIManager ui;
  vvSlicerManager * mSlicerManager;
 
}; // end class vvToolROIManager
//------------------------------------------------------------------------------

#endif

