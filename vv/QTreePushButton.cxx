#include "QTreePushButton.h"

QTreePushButton::QTreePushButton():QPushButton()
{
    m_item = NULL;
    m_index = 0;
    m_column = 0;
    connect(this,SIGNAL(clicked()),this, SLOT(clickedIntoATree()));
}

void QTreePushButton::clickedIntoATree()
{
    emit clickedInto(m_item,m_column);
    emit clickedInto(m_index,m_column);
}
