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
#ifndef _vvLinkPanel_H
#define _vvLinkPanel_H
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
    bool isLinkAll();

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
    
    bool mLinkAll;

}; // end class vvLinkPanel
//====================================================================

#endif /* end #define _vvLinkPanel_H */

