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
#ifndef VVQPROGRESSDIALOGITKCOMMAND_H
#define VVQPROGRESSDIALOGITKCOMMAND_H
#include "itkCommand.h"

// qt include
#include <QProgressDialog>

#include "clitkCommon.h"

class vvQProgressDialogITKCommand : public itk::Command {

public:
    typedef vvQProgressDialogITKCommand    Self;
    typedef itk::Command                   Superclass;
    typedef itk::SmartPointer<Self>        Pointer;

    itkNewMacro(Self);

    void Initialize(QString title, float sec, int max);

    void Execute(itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE;
    void Execute(const itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE;

protected:
    vvQProgressDialogITKCommand();
    QProgressDialog progress;
    int i;

}; // end class vvQProgressDialogITKCommand

#endif /* end #define VVQPROGRESSDIALOGITKCOMMAND_H */

