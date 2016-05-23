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
===========================================================================*/
      /*=========================================================================

    Program:   vv
    Module:    $RCSfile: vvToolMedianFilter.h,v $
    Language:  C++
    Date:      $Date: 2010/04/26 18:21:55 $
    Version:   $Revision: 1.2 $
    Author :   Bharath Navalpakkam (Bharath.Navalpakkam@creatis.insa-lyon.fr)

    Copyright (C) 2010
    Léon Bérard cancer center http://www.centreleonberard.fr
    CREATIS                   http://www.creatis.insa-lyon.fr

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    =========================================================================*/

  #ifndef VVTOOLMedianFilter_H
  #define VVTOOLMedianFilter_H

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
  #include <QtDesigner/QDesignerExportWidget>
#else
  #include <QtUiPlugin/QDesignerExportWidget>
#endif
  #include "vvToolBase.h"
  #include "QWidget"
  #include "vvToolWidgetBase.h"
  #include "ui_vvToolMedianFilter.h"
  #include "clitkMedianImageFilter_ggo.h"
   #include <clitkMedianImageGenericFilter.h>


  //------------------------------------------------------------------------------
  class vvToolMedianFilter:
    public vvToolWidgetBase,
    public vvToolBase<vvToolMedianFilter>, 
    private Ui::vvToolMedianFilter
  {
      Q_OBJECT	
  public:
      vvToolMedianFilter(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
      ~vvToolMedianFilter();

      static void Initialize();
      virtual void GetArgsInfoFromGUI();
      virtual void InputIsSelected(vvSlicerManager * m);
      virtual void apply();
 

    public slots:    
    
     void UpdateH1slider();   
     void UpdateH2slider();   
     void UpdateH3slider();
  protected:

      Ui::vvToolMedianFilter ui;
     args_info_clitkMedianImageFilter mArgsInfo;


  }; // end class vvToolMedianFilter
  //------------------------------------------------------------------------------

  #endif

