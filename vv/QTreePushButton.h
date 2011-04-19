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
#ifndef QTreePushButton_h
#define QTreePushButton_h
#include <QObject>
#include <QPushButton>
#include <QTreeWidgetItem>

class QTreePushButton : public QPushButton
{
    Q_OBJECT

public:
    QTreePushButton();

    void setIndex(int index) {
        m_index = index;
    }
    void setItem(QTreeWidgetItem* item) {
        m_item = item;
    }
    void setColumn(int column) {
        m_column = column;
    }

public slots:
    void clickedIntoATree();

signals:
    void clickedInto(QTreeWidgetItem* item,int column);
    void clickedInto(int index,int column);

private:
    QTreeWidgetItem* m_item;
    int m_index;
    int m_column;
};
#endif
