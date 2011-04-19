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

#ifndef _vvLinkPanel_CXX
#define _vvLinkPanel_CXX

#include "vvLinkPanel.h"
#include "clitkCommon.h"

#include <QtGui>
#include <Qt>
#include "QTreePushButton.h"

//------------------------------------------------------------------------------
vvLinkPanel::vvLinkPanel(QWidget * parent):QWidget(parent)
{
  setupUi(this);
  imageNames.resize(0);
  image1Ids.resize(0);
  image2Ids.resize(0);

  linkTableWidget->resizeColumnsToContents();
  linkTableWidget->verticalHeader()->hide();
  linkTableWidget->horizontalHeader()->hide();
  linkTableWidget->hideColumn(4);
  linkTableWidget->hideColumn(5);

  connect(image1ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateComboBox2(int)));
  connect(linkButton,SIGNAL(clicked()),this,SLOT(addLink()));
  connect(linkAllButton,SIGNAL(clicked()),this,SLOT(linkAll()));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvLinkPanel::addImage(std::string name, std::string id)
{
  imageNames.push_back(name);
  image1Ids.push_back(id);
  UpdateComboBox1();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvLinkPanel::isLinkAll()
{
  return mLinkAll;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvLinkPanel::removeImage(int index)
{
  std::string idRemoved = image1Ids[index];
  std::vector<std::string>::iterator Nameiter = imageNames.begin();
  std::vector<std::string>::iterator Iditer = image1Ids.begin();
  for (int i = 0; i < index; i++) {
    Nameiter++;
    Iditer++;
  }
  imageNames.erase(Nameiter);
  image1Ids.erase(Iditer);
  UpdateComboBox1();
  for (int i = linkTableWidget->rowCount() - 1; i >= 0 ; i--) {
    if (linkTableWidget->item(i,4)->text().toStdString() == idRemoved ||
        linkTableWidget->item(i,5)->text().toStdString() == idRemoved) {
      emit removeLink(linkTableWidget->item(i,4)->text(),linkTableWidget->item(i,5)->text());
      linkTableWidget->removeRow(i);
      UpdateComboBox2(image1ComboBox->currentIndex());
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvLinkPanel::UpdateComboBox1()
{
  image1ComboBox->clear();
  for (unsigned int i = 0; i < imageNames.size(); i++) {
    image1ComboBox->addItem(imageNames[i].c_str());
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvLinkPanel::UpdateComboBox2(int index)
{
  image2ComboBox->clear();
  image2Ids.resize(0);
  if (imageNames.size() > 1 && index >= 0) {
    for (unsigned int i = 0; i < imageNames.size(); i++) {
      if ((int)i != index) {
        bool AlreadyLinked = false;
        for (int row = 0; row < linkTableWidget->rowCount(); row++) {
          if ((linkTableWidget->item(row,1)->text().toStdString() == imageNames[index] &&
               linkTableWidget->item(row,3)->text().toStdString() == imageNames[i]) ||
              (linkTableWidget->item(row,3)->text().toStdString() == imageNames[index] &&
               linkTableWidget->item(row,1)->text().toStdString() == imageNames[i])) {
            AlreadyLinked = true;
            break;
          }
        }
        if (!AlreadyLinked) {
          image2ComboBox->addItem(imageNames[i].c_str());
          image2Ids.push_back(image1Ids[i]);
        }
      }
    }
  }
  if (image2ComboBox->count() == 0)
    linkButton->setEnabled(0);
  else
    linkButton->setEnabled(1);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvLinkPanel::linkAll()
{
  mLinkAll = true;
  //First remove all links
  while (linkTableWidget->rowCount())
    removeLink(1,1);
  //Now create all possible links
  int count=image2ComboBox->count();
  for (int j=0; j<count; j++) {
    image1ComboBox->setCurrentIndex(j);
    image2ComboBox->setCurrentIndex(0);
    for (int i=0; i< count-j; i++)
      addLink();
  }
  mLinkAll = false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvLinkPanel::addLink()
{
  if (!image1ComboBox->currentText().isEmpty()
      && !image2ComboBox->currentText().isEmpty()) {
    int row = linkTableWidget->rowCount();
    linkTableWidget->insertRow(row);

    linkTableWidget->setItem(row,1,new QTableWidgetItem(image1ComboBox->currentText()));
    linkTableWidget->setItem(row,2,new QTableWidgetItem("&"));
    linkTableWidget->setItem(row,3,new QTableWidgetItem(image2ComboBox->currentText()));
    linkTableWidget->setItem(row,4,new QTableWidgetItem(image1Ids[image1ComboBox->currentIndex()].c_str()));
    linkTableWidget->setItem(row,5,new QTableWidgetItem(image2Ids[image2ComboBox->currentIndex()].c_str()));
    QTreePushButton* cButton = new QTreePushButton;
    cButton->setIndex(linkTableWidget->rowCount());
    cButton->setColumn(0);
    cButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/exit.png")));
    connect(cButton,SIGNAL(clickedInto(int, int)),
            this,SLOT(removeLink(int, int)));
    cButton->setToolTip(tr("remove link"));
    linkTableWidget->setCellWidget(row,0,cButton);

    linkTableWidget->resizeColumnToContents(0);
    linkTableWidget->resizeColumnToContents(1);
    linkTableWidget->resizeColumnToContents(2);
    linkTableWidget->resizeColumnToContents(3);
    linkTableWidget->setRowHeight(row,17);

    emit addLink(image1Ids[image1ComboBox->currentIndex()].c_str(),
                 image2Ids[image2ComboBox->currentIndex()].c_str());
    UpdateComboBox2(image1ComboBox->currentIndex());
  }

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvLinkPanel::removeLink(int row, int column)
{
//  DD(row);
//   DD(column);
  while (linkTableWidget->item(row-1,4) == NULL) {
    --row;
    //  DD(linkTableWidget->rowCount());
//     DD(row);
    // return;
  }
  if (linkTableWidget->item(row-1,5) == NULL) {
    return; // should not happend ...
  }

  emit removeLink(linkTableWidget->item(row-1,4)->text(),linkTableWidget->item(row-1,5)->text());
  // DD("after emit");
  linkTableWidget->removeRow(row-1);
  // DD("after removeRow");
  UpdateComboBox2(image1ComboBox->currentIndex());
  // DD("end");
}
//------------------------------------------------------------------------------

#endif /* end #define _vvLinkPanel_CXX */

