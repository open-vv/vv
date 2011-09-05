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
#ifndef VVDICOMSERIESSELECTOR_CXX
#define VVDICOMSERIESSELECTOR_CXX
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <gdcmFile.h>
#if GDCM_MAJOR_VERSION == 2
#include <gdcmImageReader.h>
#include <gdcmDataSetHelper.h>
#include <gdcmStringFilter.h>
#include <gdcmImageHelper.h>
#else
#include <gdcmDocEntry.h>
#endif

#include "vvQDicomSeriesSelector.h"
//#include "vvUserConfig.h"

//====================================================================
vvDicomSeriesSelector::vvDicomSeriesSelector(QWidget* parent)
  :QDialog(parent)
{
  // Set GUI
  ui.setupUi(this);
  // Correct GUI for splitter ...
  //   QSplitter * splitter = new QSplitter(this);
  //   splitter->setOrientation(Qt::Horizontal);
  //   ui.gridLayout1->addWidget(splitter, 0, 0, 1, 1);
  //   ui.mFrameLeft->setParent(splitter);
  //   ui.mFrameRight->setParent(splitter);
  //  ui.toolBox->setCurrentIndex(0);



  ui.mButtonBox->button(QDialogButtonBox::Open)->setEnabled(false);

  connect(ui.mBrowseButton, SIGNAL(released()),
          this, SLOT(BrowseButtonRelease()));
  connect(ui.mSearchButton, SIGNAL(released()),
          this, SLOT(SearchButtonRelease()));
  connect(ui.mListWidget, SIGNAL(itemSelectionChanged()),
          this, SLOT(itemSelectionChanged()));
  connect(ui.mDicomDetailsListWidget, SIGNAL(itemSelectionChanged()),
          this, SLOT(itemDetailsSelectionChanged()));

  // Initialization
  /*	if (config::get_current_path() != QString(0))
    mFoldername = config::get_current_path();
    else*/
  mFoldername = QFileInfo("./").absolutePath();

  mPreviousPath = mFoldername;
  ui.mFolderLineEdit->setText(mFoldername);
  //  ui.mTableWidget->setRowCount(0);
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::show()
{
  QDialog::show();
  //grabKeyboard();
  // ui.mListWidget->grabKeyboard();
  //  ui.mDicomDetailsListWidget->grabKeyboard();
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::close()
{
  QDialog::close();
  // ui.mListWidget->releaseKeyboard()
  //releaseKeyboard();
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::BrowseButtonRelease()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setFilter("DICOM files (*.dcm); All files (*)");
  mFoldername = dialog.getExistingDirectory(this,
                "Select a folder to find DICOM image",
                mPreviousPath);
  ui.mFolderLineEdit->setText(mFoldername);
  mPreviousPath = QFileInfo(mFoldername).absolutePath();
  //	config::set_current_path(mPreviousPath);
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::SearchButtonRelease()
{
  typedef itk::GDCMSeriesFileNames NamesGeneratorType;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
  nameGenerator->SetUseSeriesDetails(true);
  //nameGenerator->SetDirectory(mFoldername.toStdString());
  nameGenerator->SetRecursive(ui.mIsRecursiveCheckBox->checkState() == Qt::Checked);

  //ds gérer recursive moi-meme pour progress ...
  nameGenerator->SetInputDirectory(mFoldername.toStdString());

  // insert in table
  typedef std::vector<std::string> SeriesIdContainer;
  const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

  for (unsigned int i=0; i<seriesUID.size(); i++) {
    // std::cout << seriesUID[i] << std::endl; //ds verif existe pas déja

    if (mListOfSeriesFilenames[seriesUID[i]]) {
      std::cout << seriesUID[i] << " exist" << std::endl;
    } else {
      // store filenames
      std::vector<std::string> * filenames = new std::vector<std::string>;
      const std::vector<std::string> & temp = nameGenerator->GetFileNames(seriesUID[i]);
      for (unsigned int j=0; j<temp.size(); j++) {
        filenames->push_back(temp[j]);
      }
      mListOfSeriesFilenames[seriesUID[i]] = filenames;

      // store first header
#if GDCM_MAJOR_VERSION == 2
      mDicomHeader[seriesUID[i]].SetFileName( (*filenames)[0].c_str() );
      mDicomHeader[seriesUID[i]].Read();
#else
      gdcm::File *header = new gdcm::File();
      header->SetFileName((*filenames)[0]);
      header->SetMaxSizeLoadEntry(16384);
      header->SetLoadMode(gdcm::LD_NOSHADOW);// don't load shadow tags (in order to save memory)
      header->Load();


      //
      DD(header->GetEntryValue(0x0028,0x0030).c_str());

      //DS TEST DAVID
      DD(header->GetXSize());
      DD(header->GetYSize());
      DD(header->GetZSize());
      DD(header->GetXSpacing());
      DD(header->GetYSpacing());
      DD(header->GetZSpacing());
      DD(header->GetXOrigin());
      DD(header->GetYOrigin());
      DD(header->GetZOrigin());
      DD("\n");
      /*
        QString size = QString("%1x%2x%3")
        .arg(header->GetXSize())
        .arg(header->GetYSize())
        .arg(header->GetZSize());
        QString spacing = QString("%1x%2x%3")
        .arg(header->GetXSpacing())
        .arg(header->GetYSpacing())
        .arg(header->GetZSpacing());
        QString origin = QString("%1x%2x%3")
        .arg(header->GetXOrigin())
        .arg(header->GetYOrigin())
        .arg(header->GetZOrigin());
      */


      mDicomHeader[seriesUID[i]] = header;
#endif

      // new item
      QListWidgetItem *newItem = new QListWidgetItem;
      newItem->setText(seriesUID[i].c_str());
      ui.mListWidget->insertItem(i, newItem);

      //AddSerieToTheTable(i, *filenames);
    }
  }
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::itemSelectionChanged()
{
  //  mLabelSelected.setText(
  mCurrentSerie = ui.mListWidget->selectedItems()[0]->text().toStdString();
  mFilenames = mListOfSeriesFilenames[mCurrentSerie];
  ui.mButtonBox->button(QDialogButtonBox::Open)->setEnabled(true);

  if (mDicomInfo[mCurrentSerie] == "") {
    // 	QString m;
    // 	m = QString("Patient : <font color=\"blue\">%1</font><br>").arg(mDicomHeader[s]->GetEntryValue(0x0010,0x0010).c_str()); // Patient's name

    mDicomInfo[mCurrentSerie] = MakeDicomInfo(mCurrentSerie, mDicomHeader[mCurrentSerie]);
  }
  ui.mDicomInfoPanel->setText(mDicomInfo[mCurrentSerie]);

  // Detail tab
  ui.mDicomDetailsListWidget->clear();
  for (unsigned int i=0; i<mFilenames->size(); i++) {
    QListWidgetItem * newItem = new QListWidgetItem;
    newItem->setText(QFileInfo((*mFilenames)[i].c_str()).fileName());
    ui.mDicomDetailsListWidget->insertItem(i, newItem);
  }

}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::itemDetailsSelectionChanged()
{
  unsigned int i = ui.mDicomDetailsListWidget->currentRow();
  if (i<mFilenames->size()) {
    if (mDicomDetails[(*mFilenames)[i]] == "") {
      std::ostringstream s;
      QString l;

#if GDCM_MAJOR_VERSION == 2
      mDicomHeader[mCurrentSerie].GetFile().Print(s);
      const gdcm::File& header = mDicomHeader[mCurrentSerie].GetFile();
      gdcm::StringFilter sf;
      sf.SetFile( header );
      const gdcm::DataSet &ds = header.GetDataSet();
      gdcm::DataSet::ConstIterator it = ds.Begin();
      for (; it != ds.End(); ++it )
        {
        const gdcm::DataElement & ref = *it;
        const gdcm::Tag &         tag = ref.GetTag();
        gdcm::VR vr = gdcm::DataSetHelper::ComputeVR(header, ds, tag);
        if ( vr & ( gdcm::VR::OB | gdcm::VR::OF | gdcm::VR::OW | gdcm::VR::SQ | gdcm::VR::UN ) )
          {
          // What is the behavior for binary stuff ?
          }
        else /* if ( vr & gdcm::VR::VRASCII ) */
          {
          if ( tag.IsPublic() )
            {
            std::pair<std::string, std::string> p = sf.ToStringPair(tag);
            l += QString("%1 : %2\n")
              .arg( p.first.c_str() )
              .arg( p.second.c_str() );
            }
          }
        }
#else
      mDicomHeader[mCurrentSerie]->Print(s);
      gdcm::File * header = mDicomHeader[mCurrentSerie];
      gdcm::DocEntry * e = header->GetFirstEntry();
      while (e) {
        if (e->GetName() != "gdcm::Unknown") {
          l += QString("%1 : %2\n")
               .arg(e->GetName().c_str())
               .arg((header->GetEntryValue(e->GetGroup(), e->GetElement())).c_str());
        }
        e = header->GetNextEntry();
      }
#endif

      mDicomDetails[(*mFilenames)[i]] = l.toStdString();
    }
    ui.mDicomDetailsLabel->setText(mDicomDetails[(*mFilenames)[i]].c_str());
  }
}
//====================================================================

//====================================================================
#if GDCM_MAJOR_VERSION == 2
QString vvDicomSeriesSelector::MakeDicomInfo(std::string & s, const gdcm::Reader& header)
{
  QString n = QString("%1").arg(mListOfSeriesFilenames[s]->size());
  const gdcm::File &f = header.GetFile();
  std::vector<double> thespacing = gdcm::ImageHelper::GetSpacingValue(f);
  std::vector<double> theorigin = gdcm::ImageHelper::GetOriginValue(f);

  QString size = QString("%1x%2x%3")
                 .arg(0)
                 .arg(0)
                 .arg(0);
  QString spacing = QString("%1x%2x%3")
                    .arg(thespacing[0])
                    .arg(thespacing[1])
                    .arg(thespacing[2]);
  QString origin = QString("%1x%2x%3")
                   .arg(theorigin[0])
                   .arg(theorigin[1])
                   .arg(theorigin[2]);

  QString ss =
    //AddInfo(        "Serie ID   : ", s)+
    AddInfo(&header.GetFile(), "Patient : ", 0x0010,0x0010)+
    AddInfo(        "Folder : ", QFileInfo((*mFilenames)[0].c_str()).canonicalPath().toStdString())+
    AddInfo(&header.GetFile(), "Series Description : ", 0x0008,0x103e)+
    AddInfo(&header.GetFile(), "Modality : ", 0x0008,0x0060)+
    AddInfo(&header.GetFile(), "# images : ", 0x0020,0x0013)+
    AddInfo(        "# files : ", n.toStdString())+
    AddInfo(        "Size : ", size.toStdString())+
    AddInfo(        "Spacing : ", spacing.toStdString())+
    AddInfo(        "Origin : ", origin.toStdString())+
    AddInfo(&header.GetFile(), "Pixel size : ", 0x0028,0x0100)+
    AddInfo(        "Pixel type : ", "");
  return ss;
}
#else
QString vvDicomSeriesSelector::MakeDicomInfo(std::string & s, gdcm::File *header)
{
  QString n = QString("%1").arg(mListOfSeriesFilenames[s]->size());
  QString size = QString("%1x%2x%3")
                 .arg(header->GetXSize())
                 .arg(header->GetYSize())
                 .arg(header->GetZSize());
  QString spacing = QString("%1x%2x%3")
                    .arg(header->GetXSpacing())
                    .arg(header->GetYSpacing())
                    .arg(header->GetZSpacing());
  QString origin = QString("%1x%2x%3")
                   .arg(header->GetXOrigin())
                   .arg(header->GetYOrigin())
                   .arg(header->GetZOrigin());
  QString ss =
    //AddInfo(        "Serie ID   : ", s)+
    AddInfo(header, "Patient : ", 0x0010,0x0010)+
    AddInfo(        "Folder : ", QFileInfo((*mFilenames)[0].c_str()).canonicalPath().toStdString())+
    AddInfo(header, "Series Description : ", 0x0008,0x103e)+
    AddInfo(header, "Modality : ", 0x0008,0x0060)+
    AddInfo(header, "# images : ", 0x0020,0x0013)+
    AddInfo(        "# files : ", n.toStdString())+
    AddInfo(        "Size : ", size.toStdString())+
    AddInfo(        "Spacing : ", spacing.toStdString())+
    AddInfo(        "Origin : ", origin.toStdString())+
    AddInfo(header, "Pixel size : ", 0x0028,0x0100)+
    AddInfo(        "Pixel type : ", header->GetPixelType());
  return ss;
}
#endif
//====================================================================

//====================================================================
QString vvDicomSeriesSelector::AddInfo(const gdcm::File *header, QString n, uint16_t group, uint16_t elem)
{
#if GDCM_MAJOR_VERSION == 2
  gdcm::StringFilter sf;
  sf.SetFile( *header );
  gdcm::Tag t( group, elem );
  std::string s = sf.ToString( t );
  return AddInfo(n.toStdString(), s);
#else
  return AddInfo(n.toStdString(), const_cast<gdcm::File*>(header)->GetEntryValue(group, elem));
#endif
}
//====================================================================

//====================================================================
QString vvDicomSeriesSelector::AddInfo(std::string n, std::string m)
{
  QString s = QString("%1 <font color=\"blue\">%2</font><br>").
              arg(n.c_str()).arg(m.c_str());
  return s;
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::AddSerieToTheTable(int i, std::vector<std::string> & filenames)
{
#if GDCM_MAJOR_VERSION == 2
#else
  gdcm::File *header = new gdcm::File();
  header->SetFileName(filenames[0]);
  header->SetMaxSizeLoadEntry(16384);
  header->SetLoadMode(gdcm::LD_NOSHADOW);// don't load shadow tags (in order to save memory)
  header->Load();
  //header->Print(cout);
  // ->GetValEntry
  //  mDicomHeader[] = header;

  /*

  QTableWidgetItem *newItem = new
  QTableWidgetItem(QString("# images = %1").arg(header->GetImageNumber()));
  //  newItem->setCheckState(Qt::Checked);
  //newItem->setFlags(!Qt::ItemIsEditable);
  DD(ui.mTableWidget->rowCount());
  ui.mTableWidget->setItem(i, 0, newItem);
  */
#endif
}
//====================================================================

#endif // VVDICOMSERIESSELECTOR_CXX
