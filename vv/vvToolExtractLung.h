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

#ifndef VVTOOLEXTRACTLUNG_H
#define VVTOOLEXTRACTLUNG_H

// clitk
#include "clitkExtractLungGenericFilter.h"
#include "../segmentation/clitkExtractLung_ggo.h"

// vv
#include "ui_vvToolExtractLung.h"
#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvROIActor.h"

// qt
#include <QtDesigner/QDesignerExportWidget>

//------------------------------------------------------------------------------
class vvToolExtractLung:
  public vvToolWidgetBase,
  public vvToolBase<vvToolExtractLung>, 
  private Ui::vvToolExtractLung 
{
  Q_OBJECT
    public:
  vvToolExtractLung(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
  ~vvToolExtractLung();

  //-----------------------------------------------------
  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager *m);
  void GetArgsInfoFromGUI();

  //-----------------------------------------------------
public slots:
  virtual void apply();
  virtual bool close();
  void PatientMaskInputIsSelected();

  //-----------------------------------------------------
protected:
  typedef args_info_clitkExtractLung ArgsInfoType;
  ArgsInfoType mArgsInfo;
  typedef clitk::ExtractLungGenericFilter<ArgsInfoType> FilterType;
  FilterType::Pointer mFilter;
  vvImage::Pointer mPatient;
  double mPatientBackgroundValue;

}; // end class vvToolExtractLung
//------------------------------------------------------------------------------

#endif

