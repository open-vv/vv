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

#ifndef VVTOOLEXTRACTPATIENT_H
#define VVTOOLEXTRACTPATIENT_H

// clitk
#include "clitkExtractPatientGenericFilter.h"
#include "../segmentation/clitkExtractPatient_ggo.h"

// vv
#include "ui_vvToolExtractPatient.h"
#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvROIActor.h"

// qt
#include <QtDesigner/QDesignerExportWidget>

//------------------------------------------------------------------------------
class vvToolExtractPatient:
  public vvToolWidgetBase,
  public vvToolBase<vvToolExtractPatient>, 
  private Ui::vvToolExtractPatient 
{
  Q_OBJECT
    public:
  vvToolExtractPatient(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
  ~vvToolExtractPatient();

  //-----------------------------------------------------
  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager *m);
  void GetArgsInfoFromGUI();
  void SetGUIFromArgsInfo();

  //-----------------------------------------------------
public slots:
  virtual void apply();
  virtual bool close();
  // void PatientMaskInputIsSelected();
  void ThreadInterrupted();

  //-----------------------------------------------------
protected:
  typedef args_info_clitkExtractPatient ArgsInfoType;
  ArgsInfoType * mArgsInfo;
  typedef clitk::ExtractPatientGenericFilter<ArgsInfoType> FilterType;
  FilterType * mFilter;
  vvImage::Pointer mPatient;
  double mPatientBackgroundValue;
  bool m_IsThreadInterrupted;

}; // end class vvToolExtractPatient
//------------------------------------------------------------------------------

#endif

