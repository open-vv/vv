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
#ifndef _vvLandmarksPanel_CXX
#define _vvLandmarksPanel_CXX
#include "vvLandmarksPanel.h"

#include <QtGui>
#include <Qt>
#include <QFileDialog>
#include "QTreePushButton.h"
#include "vvLandmarks.h"

#include <vtksys/SystemTools.hxx>
#include <clitkDD.h>

//====================================================================
vvLandmarksPanel::vvLandmarksPanel(QWidget * parent):QWidget(parent)
{ //out << __func__ << endl;
  setupUi(this);

  tableWidget->verticalHeader()->hide();
  tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  loadButton->setEnabled(0);
  saveButton->setEnabled(0);
  removeButton->setEnabled(0);
  connect(loadButton, SIGNAL(clicked()),this,SLOT(Load()));
  connect(saveButton, SIGNAL(clicked()),this,SLOT(Save()));
  connect(removeButton, SIGNAL(clicked()),this,SLOT(RemoveSelectedPoints()));
  connect(removeAllButton, SIGNAL(clicked()),this,SLOT(RemoveAllPoints()));
  connect(tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(CommentsChanged(int,int)));
  connect(tableWidget,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(SelectPoint()));
}

void vvLandmarksPanel::Load()
{ //out << __func__ << endl;
  QString file = QFileDialog::getOpenFileName(this,tr("Load Landmarks"),
                 mCurrentPath.c_str(),tr("Landmarks ( *.txt *.pts)"));
  if (!file.isEmpty()) {
    std::vector<std::string> files(1, file.toStdString());
    LoadFromFile(files);
  }
}

bool vvLandmarksPanel::LoadFromFile(std::vector<std::string> files)
{ //out << __func__ << endl;
  if (!mCurrentLandmarks->LoadFile(files))
    return false;
  
  SetCurrentLandmarks(mCurrentLandmarks,0);
  emit UpdateRenderWindows();
  return true;
}

void vvLandmarksPanel::Save()
{ //out << __func__ << endl;
  QString file = QFileDialog::getSaveFileName(this,
                 tr("Save Landmarks"),
                 mCurrentPath.c_str(),tr("Landmarks ( *.txt)"));
  if (!file.isEmpty()) {
    std::string filename = vtksys::SystemTools::GetFilenamePath(file.toStdString());
    filename += "/" + vtksys::SystemTools::GetFilenameWithoutLastExtension(file.toStdString());
    filename += ".txt";
    mCurrentLandmarks->SaveFile(filename.c_str());
  }
}

void vvLandmarksPanel::SelectPoint()
{ //out << __func__ << endl;
  if (tableWidget->rowCount() > 0) {
    QList<QTableWidgetItem *> items = tableWidget->selectedItems();
    if (!items.empty()) {
      // we're using single-selection mode
      int row = items[0]->row();
      mSelectedPoint[0] = mCurrentLandmarks->GetCoordinates(row)[0];
      mSelectedPoint[1] = mCurrentLandmarks->GetCoordinates(row)[1];
      mSelectedPoint[2] = mCurrentLandmarks->GetCoordinates(row)[2];
      mSelectedPoint[3] = mCurrentLandmarks->GetCoordinates(row)[3];
        
      emit SelectedPointChanged();
    }
  }

}


void vvLandmarksPanel::RemoveSelectedPoints()
{ //out << __func__ << endl;
  if (tableWidget->rowCount() > 0) {
    QList<QTableWidgetItem *> items = tableWidget->selectedItems();
    if (items.empty()) {
      tableWidget->removeRow(tableWidget->rowCount()-1);
      mCurrentLandmarks->RemoveLastLandmark();
    }
    else {
      // we're using single-selection mode
      int row = items[0]->row();
      mCurrentLandmarks->RemoveLandmark(row);
      tableWidget->removeRow(row);
      
      for (int i = row; i < tableWidget->rowCount(); i++) {
        QTableWidgetItem* iItem = tableWidget->item(i, 0);
        iItem->setText(QString::number(i));
      }
        
    }
    emit UpdateRenderWindows();
  }
}

void vvLandmarksPanel::RemoveAllPoints()
{ //out << __func__ << endl;
  mCurrentLandmarks->RemoveAll();
  tableWidget->clearContents();
  tableWidget->setRowCount(0);
  emit UpdateRenderWindows();
}

void vvLandmarksPanel::AddPoint()
{ //out << __func__ << endl;
  AddPoint(mCurrentLandmarks->GetNumberOfPoints()-1);
}

void vvLandmarksPanel::AddPoint(int landmarksIndex)
{ //out << __func__ << endl;
  int rowIndex = tableWidget->rowCount();
//   DD(rowIndex);
  tableWidget->setRowCount(rowIndex+1);
  tableWidget->setRowHeight(rowIndex,20);
  QTableWidgetItem* iItem = new QTableWidgetItem(QString::number(rowIndex));
  iItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  tableWidget->setItem(rowIndex,0,iItem);

  QTableWidgetItem* xItem = new QTableWidgetItem(
    QString::number(mCurrentLandmarks->GetCoordinates(landmarksIndex)[0],'f',1));
  xItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  tableWidget->setItem(rowIndex,1,xItem);

  QTableWidgetItem* yItem = new QTableWidgetItem(
    QString::number(mCurrentLandmarks->GetCoordinates(landmarksIndex)[1],'f',1));
  yItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  tableWidget->setItem(rowIndex,2,yItem);

  QTableWidgetItem* zItem = new QTableWidgetItem(
    QString::number(mCurrentLandmarks->GetCoordinates(landmarksIndex)[2],'f',1));
  zItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  tableWidget->setItem(rowIndex,3,zItem);

  QTableWidgetItem* tItem = new QTableWidgetItem(
    QString::number(mCurrentLandmarks->GetCoordinates(landmarksIndex)[3],'f',1));
  tItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  tableWidget->setItem(rowIndex,4,tItem);


  QTableWidgetItem* vItem = new QTableWidgetItem(
    QString::number(mCurrentLandmarks->GetPixelValue(landmarksIndex),'f',1));
  vItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  tableWidget->setItem(rowIndex,5,vItem);

  tableWidget->setItem(rowIndex,6, new QTableWidgetItem(mCurrentLandmarks->GetComments(landmarksIndex).c_str()));
}

void vvLandmarksPanel::SetCurrentLandmarks(vvLandmarks* lm,int time)
{ //out << __func__ << endl;
  if (time != lm->GetTime())
    return;
  loadButton->setEnabled(1);
  saveButton->setEnabled(1);
  removeButton->setEnabled(1);
  mCurrentLandmarks = lm;
  tableWidget->clearContents();
  tableWidget->setRowCount(0);
  for (unsigned int i = 0; i < mCurrentLandmarks->GetNumberOfPoints(); i++) {
      AddPoint(i);
  }
  //if (time > 1)
  //tableWidget->setColumnHidden(4,1);
  //else
  //tableWidget->setColumnHidden(4,0);
  tableWidget->resizeColumnsToContents();
}

void vvLandmarksPanel::SetCurrentImage(std::string filename)
{ //out << __func__ << endl;
  QString image = "<b>CurrentImage : </b>";
  image += vtksys::SystemTools::GetFilenameWithoutLastExtension(filename).c_str();
  nameLabel->setText(image);
}

void vvLandmarksPanel::CommentsChanged(int row, int column)
{ //out << __func__ << endl;
  if (column == 6) {
    mCurrentLandmarks->ChangeComments(row,std::string(tableWidget->item(row,column)->text().toStdString()));
    tableWidget->resizeColumnsToContents();
  }
}

#endif /* end #define _vvLandmarksPanel_CXX */

