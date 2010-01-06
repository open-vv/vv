/*=========================================================================

Program:   vv
Module:    $RCSfile: vvQProgressDialogITKCommand.h,v $
Language:  C++
Date:      $Date: 2010/01/06 13:31:58 $
Version:   $Revision: 1.1 $
Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

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
#ifndef VVQPROGRESSDIALOGITKCOMMAND_H
#define VVQPROGRESSDIALOGITKCOMMAND_H


// itk include
#include "itkCommand.h"

// qt include
#include <QProgressDialog>

class vvQProgressDialogITKCommand : public itk::Command {

public:
    typedef vvQProgressDialogITKCommand    Self;
    typedef itk::Command                   Superclass;
    typedef itk::SmartPointer<Self>        Pointer;

    itkNewMacro(Self);

    void Initialize(QString title, float sec, int max);

    void Execute(itk::Object *caller, const itk::EventObject & event);
    void Execute(const itk::Object *caller, const itk::EventObject & event);

protected:
    vvQProgressDialogITKCommand();
    QProgressDialog progress;
    int i;

}; // end class vvQProgressDialogITKCommand

#endif /* end #define VVQPROGRESSDIALOGITKCOMMAND_H */

