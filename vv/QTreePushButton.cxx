/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
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
