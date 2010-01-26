/*=========================================================================

 Program:   vv
 Language:  C++
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
