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

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif
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

  void AddImage(vvImage * binaryImage, std::string name, std::string filename, double BG, bool m_modeBG=true);
  void UpdateAllContours();
  void UpdateAllROIStatus();
#if __cplusplus > 199711L
  virtual void SaveState(std::shared_ptr<QXmlStreamWriter> & m_XmlWriter);
#else
  virtual void SaveState(std::auto_ptr<QXmlStreamWriter> & m_XmlWriter);
#endif
  virtual void InitializeNewTool(bool ReadStateFlag);

  public slots:
  void AnImageIsBeingClosed(vvSlicerManager *);
  void SelectedImageHasChanged(vvSlicerManager *);
  void Open();
  void OpenBinaryImage(QStringList & filenames);
  void OpenDicomImage(std::string filaneme, double tol=0);
  void SelectedItemChangedInTree();
  void VisibleROIToggled(bool b);
  void VisibleContourROIToggled(bool b);
  void OpacityChanged(int v);
  void ChangeColor();
  void ChangeContourColor();
  void ChangeContourWidth(int n);
  void ChangeDepth(int n);
  void AllVisibleROIToggled(int b);
  void AllVisibleContourROIToggled(int b);
  void ReloadCurrentROI();  
  void close();
  void RemoveROI();
  vvSlicerManager * GetCurrentSlicerManager() { return mCurrentSlicerManager; }

protected:
  Ui::vvToolROIManager ui;
  vvSlicerManager * mCurrentSlicerManager; // need it because do not inherit from vvToolWidgetBase
  vvImage * mCurrentImage;
  vvMainWindowBase * mMainWindow;
  int mIndexFirstTab;
  bool mOpenFileBrowserFlag;
  
  int mNumberOfVisibleROI;
  int mNumberOfVisibleContourROI;
  static int nbTotalROI;

  vtkSmartPointer<vtkLookupTable> mDefaultLUTColor;

  QStringList mOpenedBinaryImageFilenames;
  std::vector<clitk::DicomRT_ROI::Pointer> mROIList;
  std::vector<QSharedPointer<vvROIActor> > mROIActorsList;

  // Data for the widget Tree
  std::vector< QSharedPointer<QTreeWidgetItem> > mTreeWidgetList;
  std::map<clitk::DicomRT_ROI::Pointer, QTreeWidgetItem *> mMapROIToTreeWidget;
  std::map<QTreeWidgetItem *, clitk::DicomRT_ROI::Pointer> mMapTreeWidgetToROI;
  
  // Data for current selected roi
  clitk::DicomRT_ROI::Pointer mCurrentROI;
  QSharedPointer<vvROIActor> mCurrentROIActor;
  
  // Data for XML Reader
  void ReadXMLInformation();
  void ReadXMLInformation_ROI();
  uint mInitialImageIndex;
  QStringList mROIFilenames;
  std::vector<QSharedPointer<vvROIActor> > mROIActorsParamList;
 
}; // end class vvToolROIManager
//------------------------------------------------------------------------------

#endif

