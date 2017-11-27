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
#ifndef VVTOOLSEGMENTATION_H
#define VVTOOLSEGMENTATION_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvROIActor.h"
#include "ui_vvToolSegmentation.h"

#include "vtkLookupTable.h"

//------------------------------------------------------------------------------
class vvToolSegmentation:
  public vvToolWidgetBase,
  public vvToolBase<vvToolSegmentation>, 
  private Ui::vvToolSegmentation 
{
  Q_OBJECT
    public:
  vvToolSegmentation(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolSegmentation();

  //-----------------------------------------------------
  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager * m);
  void OpenBinaryImage();
  void RegionGrowing();
  void Erode();
  void Dilate();
  void Labelize();
  void Merge();
  void RemoveLabel();
  void UpdateAndRenderNewMask();

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  void KeyPressed(std::string KeyPress);
  virtual bool close();
  virtual void MousePositionChanged(int slicer);

 protected:
  Ui::vvToolSegmentation ui;
  QSharedPointer<vvROIActor> mRefMaskActor;
  QSharedPointer<vvROIActor> mCurrentMaskActor;
  std::vector<QSharedPointer<vvROIActor> > mCurrentCCLActors;
  vvImage::Pointer mRefMaskImage;
  vvImage::Pointer mCurrentMaskImage;
  vvImage::Pointer mCurrentCCLImage;
  int mKernelValue;
  vtkSmartPointer<vtkLookupTable> mDefaultLUTColor;
  enum { State_Default, State_CCL};
  int mCurrentState;

  QSharedPointer<vvROIActor> CreateMaskActor(vvImage::Pointer image, int i, int colorID, bool BGMode=false);
  
  double mCurrentLabelUnderMousePointer;
  std::vector<double> mCurrentMousePositionInMM;
  //std::vector<double> mCurrentMousePositionInPixel;
  double GetBackgroundValue() { return 0; }
  double GetForegroundValue() { return 1; }
  long ComputeNumberOfPixels(vvImage::Pointer image, double value);

  // Compute and store sizes of mask Foreground
  void   UpdateMaskSize(vvImage::Pointer image, long & pix, double & cc);
  void   UpdateMaskSizeLabels();
  long   mRefMaskSizeInPixels;
  double mRefMaskSizeInCC;
  long   mCurrentMaskSizeInPixels;
  double mCurrentMaskSizeInCC;

}; // end class vvToolSegmentation
//------------------------------------------------------------------------------

#endif
