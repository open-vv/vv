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
#ifndef vvRegisterForm_h
#define vvRegisterForm_h

#include "ui_vvRegisterForm.h"
#include <QUrl>
#include <QSettings>
#include <QNetworkAccessManager>
class vvRegisterForm : public QDialog, private Ui::vvRegisterForm
{
    Q_OBJECT

public:
    vvRegisterForm(QUrl url);
    void sendData();
    ~vvRegisterForm() {}
    virtual bool canPush();
    virtual void acquitPushed();
public slots:
  virtual void accept();
  virtual void show();
protected:
  QUrl url;
  QSettings settings;
  QNetworkAccessManager* manager;
};

#endif
