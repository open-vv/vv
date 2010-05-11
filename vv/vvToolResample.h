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

#ifndef _VVTOOLRESAMPLE_H
#define _VVTOOLRESAMPLE_H

#include "ui_vvToolResample.h"
#include "clitkCommon.h"
#include "clitkImageResampleGenericFilter.h"
#include "vvImage.h"
#include "vvSlicerManager.h"
#include "vvToolWidgetBase.h"
#include "vvToolBase.h"

#include <QtDesigner/QDesignerExportWidget>
#include <QTreeWidget>

//------------------------------------------------------------------------------
class vvToolResample :  
  public vvToolWidgetBase,
  public vvToolBase<vvToolResample>, 
  private Ui::vvToolResample {

  Q_OBJECT
    
  public:
  // constructor - destructor
  vvToolResample(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);

  // Get output result
  vvImage::Pointer GetOutput() {
    return mOutput;
  }
  std::string GetOutputFileName();
  static void Initialize();
  void apply();

public slots:
  //  void SetImagesList(QTreeWidget * tree);
  void UpdateControlSizeAndSpacing();
  void ComputeNewSizeFromSpacing();
  void ComputeNewSizeFromScale();
  void ComputeNewSizeFromIso();
  void ComputeNewSpacingFromSize();
  void ComputeNewSpacingFromScale();
  void ComputeNewSpacingFromIso();
  void UpdateInterpolation();
  void UpdateGaussianFilter();
  void InputIsSelected(vvSlicerManager* m);

protected:
  Ui::vvToolResample ui;
  vvImage::Pointer mOutput;
  clitk::ImageResampleGenericFilter::Pointer mFilter;

  vvSlicerManager* mCurrentSlicerManager;
  vvImage::Pointer mCurrentImage;
  int mCurrentIndex;

  std::vector<int> mInputOrigin;
  std::vector<int> mInputSize;
  std::vector<double> mInputSpacing;
  std::vector<int> mOutputSize;
  std::vector<double> mOutputSpacing;
  int mDimension;

  QString mLastError;

  QString mInputFileName;

  QString mInputFileFormat;
  QString mPixelType;
  QString ComponentType;

  QStringList OutputListFormat;

  void UpdateInputInfo();
  void UpdateOutputInfo();
  void UpdateOutputFormat();
  void FillSizeEdit(std::vector<int> size);
  void FillSpacingEdit(std::vector<double> spacing);
  void UpdateOutputSizeAndSpacing();

  QString GetSizeInBytes(std::vector<int> & size);
  QString GetVectorDoubleAsString(std::vector<double> vectorDouble);
  QString GetVectorIntAsString(std::vector<int> vectorInt);

}; // end class vvToolResample
//------------------------------------------------------------------------------


#endif

