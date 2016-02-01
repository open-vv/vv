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

#ifndef VVLABELIMAGELOADERWIDGET_H
#define VVLABELIMAGELOADERWIDGET_H

// vv
#include "vvImage.h"

// qt
#if QT_VERSION < 0x050000
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif
#include <QDialog>
#include "ui_vvLabelImageLoaderWidget.h"

class vvSlicerManager;

//------------------------------------------------------------------------------
class vvLabelImageLoaderWidget: public QWidget, private Ui::vvLabelImageLoaderWidget 
{
  Q_OBJECT
    public:
  vvLabelImageLoaderWidget(QWidget * parent=0, Qt::WindowFlags f=0);
  ~vvLabelImageLoaderWidget() {}

  vvImage::Pointer GetImage();
  double GetBackgroundValue();
  void SetText(QString t);
  
signals:
  void accepted();
  
protected:
  Ui::vvLabelImageLoaderWidget ui;
  vvImage::Pointer m_Output;

 protected slots:  
  void OpenImage();

}; // end class vvLabelImageLoaderWidget
//------------------------------------------------------------------------------

#endif

