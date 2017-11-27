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
Module:    $RCSfile: vvToolMIP.h,v $
Language:  C++
Date:      $Date: 2010/10/26 12:37:59 $
Version:   $Revision: 1.1 $
Author :   Joel Schaerer (joel.schaerer@gmail.com)

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

#ifndef VVTOOLMIP_H
#define VVTOOLMIP_H

#include <vvImage.h>

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif
#include "vvToolBase.h"
#include "QWidget"
#include "vvToolWidgetBase.h"
#include "ui_vvToolMIP.h"


//------------------------------------------------------------------------------
class vvToolMIP:
  public vvToolWidgetBase,
  public vvToolBase<vvToolMIP>, 
  private Ui::vvToolMIP
{
  Q_OBJECT	
  public:
    vvToolMIP(vvMainWindowBase* parent=0, Qt::WindowFlags f=0);
    ~vvToolMIP();

    static void Initialize();
    virtual void InputIsSelected(vvSlicerManager * m);
    virtual void apply();
  protected:

    template<class PixelType,int Dim> void Update_WithDimAndPixelType(vvImage::Pointer);
    template<class PixelType> void Update_WithPixelType(vvImage::Pointer);

    Ui::vvToolMIP ui;

}; // end class vvToolMIP
//------------------------------------------------------------------------------

#endif

