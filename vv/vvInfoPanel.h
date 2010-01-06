/*=========================================================================

 Program:   vv
 Module:    $RCSfile: vvInfoPanel.h,v $
 Language:  C++
 Date:      $Date: 2010/01/06 13:31:57 $
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

    void setFileName(QString text);
    void setSizeMM(QString text);
    void setOrigin(QString text);
    void setSpacing(QString text);
    void setNPixel(QString text);
    void setDimension(QString text);
    void setSizePixel(QString text);
    void setCurrentInfo(int visibility, double x, double y, double z, double X, double Y, double Z, double value);
    void setViews(int window, int view, int slice);

public slots:

private:

};

#endif
