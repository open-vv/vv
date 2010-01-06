#ifndef _vvLinkPanel_H
#define _vvLinkPanel_H

/*=========================================================================

 Program:   vv
 Module:    $RCSfile: vvLinkPanel.h,v $
 Language:  C++
 Date:      $Date: 2010/01/06 13:31:57 $
 Version:   $Revision: 1.1 $
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 200COLUMN_IMAGE_NAME
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

#include <iostream>
#include <vector>

#include "ui_vvLinkPanel.h"

//====================================================================
class vvLinkPanel : public QWidget, private Ui::vvLinkPanel
{

    Q_OBJECT

public:
    // constructor - destructor
    vvLinkPanel(QWidget * parent=0);
    ~vvLinkPanel() {}

    void addImage(std::string name, std::string id);
    void removeImage(int i);

public slots :
    void UpdateComboBox2(int i);
    void removeLink(int row, int column);
    void addLink();
    void linkAll();

signals:
    void addLink(QString image1,QString image2);
    void removeLink(QString image1,QString image2);

private:
    void UpdateComboBox1();

    std::vector<std::string> imageNames;
    std::vector<std::string> image1Ids;
    std::vector<std::string> image2Ids;

}; // end class vvLinkPanel
//====================================================================

#endif /* end #define _vvLinkPanel_H */

