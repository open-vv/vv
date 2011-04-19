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
#ifndef vvInfoPanel_h
#define vvInfoPanel_h
#include <iostream>
#include <vector>

#include "ui_vvInfoPanel.h"

class vvInfoPanel : public QWidget, private Ui::vvInfoPanel
{
  Q_OBJECT

public:
    vvInfoPanel(QWidget * parent=0):QWidget(parent) {
        setupUi(this);
    }
    ~vvInfoPanel() {}

    void setTransformation(QString text);
    void setFileName(QString text);
    void setSizeMM(QString text);
    void setOrigin(QString text);
    void setSpacing(QString text);
    void setNPixel(QString text);
    void setDimension(QString text);
    void setSizePixel(QString text);
    void setCurrentInfo(int visibility, double x, double y, double z, double X, double Y, double Z, double value);
    void setViews(int window, int view, int slice);
    void setMemoryInMb(QString text);

public slots:

private:

};

#endif
