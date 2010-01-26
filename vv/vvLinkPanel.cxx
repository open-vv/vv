#ifndef _vvLinkPanel_CXX
#define _vvLinkPanel_CXX

/*=========================================================================

 Program:   vv
 Language:  C++
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

#include "vvLinkPanel.h"
#include "clitkCommon.h"

#include <QtGui>
#include <Qt>
#include "QTreePushButton.h"

//====================================================================
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

void vvLinkPanel::addImage(std::string name, std::string id)
{
    imageNames.push_back(name);
    image1Ids.push_back(id);
    UpdateComboBox1();
}

void vvLinkPanel::removeImage(int index)
{
    std::string idRemoved = image1Ids[index];
    std::vector<std::string>::iterator Nameiter = imageNames.begin();
    std::vector<std::string>::iterator Iditer = image1Ids.begin();
    for (int i = 0; i < index; i++)
    {
        Nameiter++;
        Iditer++;
    }
    imageNames.erase(Nameiter);
    image1Ids.erase(Iditer);
    UpdateComboBox1();
    for (int i = linkTableWidget->rowCount() - 1; i >= 0 ;i--)
    {
        if (linkTableWidget->item(i,4)->text().toStdString() == idRemoved ||
                linkTableWidget->item(i,5)->text().toStdString() == idRemoved)
        {
            emit removeLink(linkTableWidget->item(i,4)->text(),linkTableWidget->item(i,5)->text());
            linkTableWidget->removeRow(i);
            UpdateComboBox2(image1ComboBox->currentIndex());
        }
    }
}

void vvLinkPanel::UpdateComboBox1()
{
    image1ComboBox->clear();
    for (unsigned int i = 0; i < imageNames.size();i++)
    {
        image1ComboBox->addItem(imageNames[i].c_str());
    }
}

void vvLinkPanel::UpdateComboBox2(int index)
{
    image2ComboBox->clear();
    image2Ids.resize(0);
    if (imageNames.size() > 1 && index >= 0)
    {
        for (unsigned int i = 0; i < imageNames.size();i++)
        {
            if ((int)i != index)
            {
                bool AlreadyLinked = false;
                for (int row = 0; row < linkTableWidget->rowCount();row++)
                {
                    if ((linkTableWidget->item(row,1)->text().toStdString() == imageNames[index] &&
                            linkTableWidget->item(row,3)->text().toStdString() == imageNames[i]) ||
                            (linkTableWidget->item(row,3)->text().toStdString() == imageNames[index] &&
                             linkTableWidget->item(row,1)->text().toStdString() == imageNames[i]))
                    {
                        AlreadyLinked = true;
                        break;
                    }
                }
                if (!AlreadyLinked)
                {
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

void vvLinkPanel::linkAll()
{
    //First remove all links
    while (linkTableWidget->rowCount())
        removeLink(1,1);
    //Now create all possible links
    int count=image2ComboBox->count();
    for (int j=0;j<count;j++)
    {
        image1ComboBox->setCurrentIndex(j);
        image2ComboBox->setCurrentIndex(0);
        for (int i=0;i< count-j;i++)
            addLink();
    }
}

void vvLinkPanel::addLink()
{
    if (!image1ComboBox->currentText().isEmpty()
            && !image2ComboBox->currentText().isEmpty())
    {
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
        cButton->setIcon(QIcon(QString::fromUtf8(":/new/prefix1/icons/exit.png")));
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

void vvLinkPanel::removeLink(int row, int column)
{
    emit removeLink(linkTableWidget->item(row-1,4)->text(),linkTableWidget->item(row-1,5)->text());
    linkTableWidget->removeRow(row-1);
    UpdateComboBox2(image1ComboBox->currentIndex());
}

#endif /* end #define _vvLinkPanel_CXX */

