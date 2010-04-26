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

#ifndef VVTOOLSTRUCTURESETMANAGER_H
#define VVTOOLSTRUCTURESETMANAGER_H

#include <QtDesigner/QDesignerExportWidget>
#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "clitkDicomRT_StructureSet.h"
#include "ui_vvToolStructureSetManager.h"

class vvStructureSetActor;

//------------------------------------------------------------------------------
class vvToolStructureSetManager:
  public vvToolWidgetBase,
  public vvToolBase<vvToolStructureSetManager>, 
  private Ui::vvToolStructureSetManager
{
  Q_OBJECT
    public:
  vvToolStructureSetManager(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
  ~vvToolStructureSetManager();

  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager *m);

  int addStructureSet(clitk::DicomRT_StructureSet * mStructureSet);
  void updateStructureSetInTreeWidget(int index, clitk::DicomRT_StructureSet * s);
  void addRoiInTreeWidget(clitk::DicomRT_ROI * roi, QTreeWidgetItem * w);

public slots:
  virtual void apply();
  void open(int type);
  void LeftButtonReleaseEvent(int slicer);
  void openBinaryImage();
  void selectedItemChangedInTree();

 protected:
  Ui::vvToolStructureSetManager ui;
  clitk::DicomRT_StructureSet * mCurrentStructureSet;
  vvStructureSetActor * mCurrentStructureSetActor;
  int mCurrentStructureSetIndex;
  vtkLookupTable * mDefaultLUTColor;
 
  void setCurrentSelectedROI(clitk::DicomRT_ROI * roi);
 
  std::vector<clitk::DicomRT_StructureSet*> mStructureSetsList;
  std::vector<vvStructureSetActor *> mStructureSetActorsList;
  std::map<int, QTreeWidgetItem *> mMapStructureSetIndexToTreeWidget;
  std::map<clitk::DicomRT_ROI*, QTreeWidgetItem *> mMapROIToTreeWidget;
  std::map<QTreeWidgetItem *, clitk::DicomRT_ROI*> mMapTreeWidgetToROI;

}; // end class vvToolStructureSetManager
//------------------------------------------------------------------------------

#endif

