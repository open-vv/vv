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

  void AddImage(vvImage * binaryImage, std::string filename, double BG, bool m_modeBG=true);
  void UpdateAllContours();
  void UpdateAllROIStatus();

  public slots:
  void AnImageIsBeingClosed(vvSlicerManager *);
  void SelectedImageHasChanged(vvSlicerManager *);
  void OpenBinaryImage();
  void OpenDicomImage();
  void SelectedItemChangedInTree();
  void VisibleROIToggled(bool b);
  void VisibleContourROIToggled(bool b);
  void OpacityChanged(int v);
  void ChangeColor();
  void ChangeContourColor();
  void ChangeContourWidth(int n);
  void ChangeDepth(int n);
  void AllVisibleROIToggled(int b);
  void AllVisibleContourROIToggled(bool b);
  void ReloadCurrentROI();  
  void close();

protected:
  Ui::vvToolROIManager ui;
  vvSlicerManager * mSlicerManager;
  vvImage * mCurrentImage;
  vvMainWindowBase * mMainWindow;
  int mIndexFirstTab;
  
  int mNumberOfVisibleROI;
  int mNumberOfVisibleContourROI;

  vtkSmartPointer<vtkLookupTable> mDefaultLUTColor;

  std::vector<vvImage::Pointer> mOpenedBinaryImage;
  std::vector<clitk::DicomRT_ROI::Pointer> mROIList;
  std::vector<QSharedPointer<vvROIActor> > mROIActorsList;

  // Data for the widget Tree
  std::vector< QSharedPointer<QTreeWidgetItem> > mTreeWidgetList;
  std::map<clitk::DicomRT_ROI::Pointer, QTreeWidgetItem *> mMapROIToTreeWidget;
  std::map<QTreeWidgetItem *, clitk::DicomRT_ROI::Pointer> mMapTreeWidgetToROI;
  
  // Data for current selected roi
  clitk::DicomRT_ROI::Pointer mCurrentROI;
  QSharedPointer<vvROIActor> mCurrentROIActor;
 
}; // end class vvToolROIManager
//------------------------------------------------------------------------------

#endif

