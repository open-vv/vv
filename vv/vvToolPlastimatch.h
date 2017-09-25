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
#ifndef VVTOOLPLASTIMATCH_H
#define VVTOOLPLASTIMATCH_H

#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvImageContour.h"
#include "ui_vvToolPlastimatch.h"

#include "clitkBinarizeImage_ggo.h"

//------------------------------------------------------------------------------
class vvToolPlastimatch:
  public vvToolWidgetBase,
  public vvToolBase<vvToolPlastimatch>, 
  private Ui::vvToolPlastimatch 
{
  Q_OBJECT
    public:
  vvToolPlastimatch(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolPlastimatch();

  //-----------------------------------------------------
  static void Initialize();
  void GetOptionsFromGUI();
  virtual void InputIsSelected(std::vector<vvSlicerManager *> & m);

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();

 protected:
  virtual void closeEvent(QCloseEvent *event);
  Ui::vvToolPlastimatch ui;
  std::vector<vvSlicerManager *> m_InputSlicerManagers;

  typedef itk::Image<float, 3>           FloatImageType;
  typedef itk::Vector<float, 3>          FloatVectorType;
  typedef itk::Image<FloatVectorType, 3> DeformationFieldType;  
  vvImage::Pointer              m_Fixed;
  vvImage::Pointer              m_Moving;
  vtkImageData *                m_FixedVTK;
  vtkImageData *                m_MovingVTK;
  FloatImageType::ConstPointer  m_FixedITK;
  FloatImageType::ConstPointer  m_MovingITK;  
  DeformationFieldType::Pointer m_DeformationField;
  FloatImageType::Pointer       m_WarpedImageITK;
  vvImage::Pointer              m_WarpedImage;

}; // end class vvToolPlastimatch
//------------------------------------------------------------------------------

#endif

