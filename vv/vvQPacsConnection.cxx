#include "vvQPacsConnection.h"
#include "gdcmCompositeNetworkFunctions.h"
#include <QtGui/qlistview.h>
#include <qfile.h>
#include <QDate>
#include <QIcon>
#include <QDateTime>
#include "vvPacsSettingsDialog.h"
#include "vvUtils.h"



vvQPacsConnection::vvQPacsConnection(QWidget *i_parent)
	:QDialog(i_parent)
{
	ui.setupUi(this);
	setWindowTitle(QString::fromUtf8("PACS CONNECTIONHHHH"));
	createTreeView();
	ui.tabFilter->setTabText(0,QString(tr("Modality")));
	ui.tabFilter->setTabText(1,QString(tr("Date")));

	ui. tabNetwork->setTabText(0,QString(tr("Network")));
	ui. tabNetwork->setTabText(1,QString(tr("Configuration")));
	ui.check_ModAll->setEnabled(true);
	ui.networkCombo->addItem("");
	ui.networkCombo->addItems(getDicomServers());
	
	// Connection	
	connect(ui.networkCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(chooseServer(int)));
	connect(ui.removeNetworkButton,SIGNAL(clicked()),this,SLOT(removeServer()));
	connect(ui.NetworkButton,SIGNAL(clicked()),this,SLOT(modifyServer()));
	
	update();
}

// remote a Dicom Server in VV settings
void vvQPacsConnection::removeServer()
{
	removeDicomServer(m_nickname);
	ui.networkCombo->removeItem(ui.networkCombo->findText(QString(m_nickname.c_str())));
	m_nickname="";
	refreshNetworks();
}

// modify a Dicom Server in VV settings
void vvQPacsConnection::modifyServer()
{
	AddDicomServer(ui.NameEdit->text().toStdString(),ui.AETitleEdit->text().toStdString(),ui.AdressEdit->text().toStdString(),ui.PortEdit->text().toStdString());
	removeServer();
}

// refresh the list of Dicom Servers available from VV settings
void vvQPacsConnection::refreshNetworks()
{
	ui.networkCombo->clear();
	ui.networkCombo->addItem(QString());
	ui.networkCombo->addItems(getDicomServers());
	ui.NameEdit->setText(QString());
	ui.AETitleEdit->setText(QString());
	ui.AdressEdit->setText(QString());
	ui.PortEdit->setText(QString());
	ui.tabNetwork->setCurrentIndex(0);
}

void vvQPacsConnection::on_clearButton_clicked()
{
	Patientmodel->removeRows(0, Patientmodel->rowCount(),QModelIndex());
	Studymodel->removeRows(0, Studymodel->rowCount(),QModelIndex());
	Seriesmodel->removeRows(0, Seriesmodel->rowCount(),QModelIndex());
	Imagesmodel->removeRows(0, Imagesmodel->rowCount(),QModelIndex());
}

void vvQPacsConnection::on_scanButton_clicked()
{
	cleanTree();
	manageStudiesFilter(true);

	// test first if echo works
	bool didItWork = gdcm::CompositeNetworkFunctions::CEcho(m_adress.c_str(), atoi(m_port.c_str()),	getDicomClientAETitle().c_str(), m_nickname.c_str() );
	if (didItWork)
	{
		m_level =gdcm::ePatient;
		std::vector<gdcm::DataSet> theDataSet;
		f_query = mQFactory.getQueryPatient(ui.patientName->toPlainText().toStdString(), 	ui.patientID->toPlainText().toStdString());

		bool cfindWork = gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), 
			gdcm::CompositeNetworkFunctions::ConstructQuery(f_query.theRoot, f_query.theLevel ,f_query.keys),
			theDataSet, getDicomClientAETitle().c_str()	, m_nickname.c_str());
		if( cfindWork)
		{
			convertDataSet(theDataSet,Patientmodel,mQFactory.getPatientKeys("",""));
		} // end cfindwork
	} // end didItwork
}


/// show Options DialogBox to set a new Dicom Server
void vvQPacsConnection::on_optionsButton_clicked()
{
	vvPacsSettingsDialog *dg  = new vvPacsSettingsDialog(this);
	dg->show();
}

void vvQPacsConnection::convertDataSet(std::vector<gdcm::DataSet> i_ds, QStandardItemModel *i_model, std::vector< std::pair<gdcm::Tag, std::string> > keys)
{

	std::vector<gdcm::DataSet>::iterator it_ds = i_ds.begin();
	for(; it_ds != i_ds.end(); it_ds++)
	{
		QList<QStandardItem *> items;
		const gdcm::DataSet ds = (*it_ds);
		std::vector< std::pair<gdcm::Tag, std::string> >::iterator it_key = keys.begin();
		int ind = 0;
		for(; it_key != keys.end(); it_key++, ind++)
		{
			gdcm::DataElement de = ds.GetDataElement((*it_key).first);
			QStandardItem *item = new QStandardItem;
			const gdcm::ByteValue *bv = (de).GetByteValue();
			if( !de.IsEmpty() )
			{
				std::string buffer = std::string( bv->GetPointer(), bv->GetLength() );
				item->setText(tr(buffer.c_str()));
			}
			else
			{
				item->setText(tr(""));
			}
			if(ind ==0)
			{
				item->setCheckable(true);
			}
			items.push_back(item);
		}
		i_model->appendRow(items);
	}
}

// TreeViews creation
void vvQPacsConnection::createTreeView()
{
	// Patient Tree View
	Patientmodel = new QStandardItemModel(0,2,this); 
	QStringList Patientlist;
	Patientlist.push_back(tr("PATIENT NAME"));
	Patientlist.push_back(tr("PATIENT ID"));
	Patientmodel->setHorizontalHeaderLabels(Patientlist);
	ui.patientTreeView->setModel(Patientmodel);
	ui.patientTreeView->setEnabled(true);
	connect(ui.patientTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectStudies(QModelIndex)));

	// Study Tree View
	Studymodel = new QStandardItemModel(0,3,this); 
	QStringList Studylist;
	Studylist.push_back(tr("DESCRIPTION"));
	Studylist.push_back(tr("DATE"));
	Studylist.push_back(tr("HOUR"));
	Studylist.push_back(tr("STUDY ID"));
	Studymodel->setHorizontalHeaderLabels(Studylist);
	ui.studyTreeView->setModel(Studymodel);
	connect(ui.studyTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectSeries(QModelIndex)));


	// Series Tree View
	Seriesmodel = new QStandardItemModel(0,2,this); 
	QStringList Serieslist;
	Serieslist.push_back(tr("MODALITY"));
	Serieslist.push_back(tr("DESCRIPTION"));
	Serieslist.push_back(tr("no. accept."));
	Seriesmodel->setHorizontalHeaderLabels(Serieslist);
	ui.seriesTreeView->setModel(Seriesmodel);
	connect(ui.seriesTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectImages(QModelIndex)));
		connect(ui.imagesTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectImage(QModelIndex)));
	// Images Tree View
	Imagesmodel = new QStandardItemModel(0,1,this); 
	QStringList Imageslist;
	Imageslist.push_back(tr("instance number"));
	Imageslist.push_back(tr("sopuid"));
	Imagesmodel->setHorizontalHeaderLabels(Imageslist);
	ui.imagesTreeView->setModel(Imagesmodel);
}

// clean the different model Trees
void vvQPacsConnection::cleanTree()
{
	Patientmodel->removeRows(0,Patientmodel->rowCount());
	Studymodel->removeRows(0,Patientmodel->rowCount());
	Seriesmodel->removeRows(0,Patientmodel->rowCount());
	Imagesmodel->removeRows(0,Patientmodel->rowCount());

}

void vvQPacsConnection::selectStudies(const QModelIndex &index)
{

		m_patient= Patientmodel->data(index.sibling(index.row(),1)).toString().toStdString();		
	Studymodel->removeRows(0, Studymodel->rowCount(),QModelIndex());
	Seriesmodel->removeRows(0, Seriesmodel->rowCount(),QModelIndex());
	Imagesmodel->removeRows(0, Imagesmodel->rowCount(),QModelIndex());
	manageSeriesFilter(true);
	convertDataSet( findQuery( mQFactory.getQueryforStudy(m_patient, false)) , Studymodel, mQFactory.getQueryKeysforStudy("",true));
}


void vvQPacsConnection::clearMove()
{

}

void vvQPacsConnection::selectSeries(const QModelIndex &index)
{
	m_study= Studymodel->data(index.sibling(index.row(),3)).toString().toStdString();
	Seriesmodel->removeRows(0, Seriesmodel->rowCount());
	Imagesmodel->removeRows(0, Imagesmodel->rowCount());
    convertDataSet( findQuery( mQFactory.getQueryforSeries(m_patient,m_study, false)), Seriesmodel, mQFactory.getSeriesKeys("","",true));
	
}

void vvQPacsConnection::selectImages(const QModelIndex &index)
{
	m_series = Seriesmodel->data(index.sibling(index.row(),2)).toString().toStdString();
	Imagesmodel->removeRows(0, Imagesmodel->rowCount(),QModelIndex());
	convertDataSet( findQuery( mQFactory.getQueryforImages(m_patient,m_study, m_series, false) ),  Imagesmodel, mQFactory.getQueryKeysforImages("","","",true));
 
}

void vvQPacsConnection::selectImage(const QModelIndex &index)
{
	std::string _image = Imagesmodel->data(index.sibling(index.row(),1)).toString().toStdString();
	mQFactory.setQueryforImage(m_patient,m_study, m_series, _image);
	
}


std::vector<gdcm::DataSet> vvQPacsConnection::findQuery(vvQuery i_query)
{
	   std::vector<gdcm::DataSet> theDataSet;
	   gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), 
		gdcm::CompositeNetworkFunctions::ConstructQuery(i_query.theRoot, i_query.theLevel,i_query.keys), theDataSet,  
		getDicomClientAETitle().c_str(), m_nickname.c_str());
	return theDataSet;
}

void vvQPacsConnection::manageStudiesFilter(bool i_enable)
{
	ui.text_PHYS->setEnabled(i_enable);
	ui.text_SDESC->setEnabled(i_enable);
	ui.dateTab->setEnabled(i_enable);

}

void vvQPacsConnection::manageSeriesFilter(bool i_enable)
{
	ui.modalityTab->setEnabled(i_enable);
}


std::vector< std::pair<gdcm::Tag, std::string> > vvQPacsConnection::getStudyKeys(const std::string i_val)
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
	// Study Description
	gdcm::Tag tagsdc(0x0008,0x1030);
	keys.push_back(std::make_pair(tagsdc, ""));
	// Study date
	gdcm::Tag tagdb(0x0008,0x0020);
	keys.push_back(std::make_pair(tagdb, ""));
	// Study Hour
	gdcm::Tag tagsdh(0x0008,0x0030);
	keys.push_back(std::make_pair(tagsdh, ""));
	// Study Instance UID
	gdcm::Tag tagsid(0x0020,0x000d);
	keys.push_back(std::make_pair(tagsid, i_val));

	return keys;
}

std::vector< std::pair<gdcm::Tag, std::string> > vvQPacsConnection::getKeys()
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
	// Patient Name
	gdcm::Tag tag(0x0010,0x0010);
	keys.push_back(std::make_pair(tag, ""));

	//// Patient ID
	gdcm::Tag tagpid(0x0010,0x0020);
	keys.push_back(std::make_pair(tagpid, ""));

	// Modality
	gdcm::Tag tagmod(0x0008,0x0061);
	keys.push_back(std::make_pair(tagmod, ""));

	// date of birth
	gdcm::Tag tagdb(0x0010,0x0030);
	keys.push_back(std::make_pair(tagdb, ""));

	// Study Date
	gdcm::Tag tagsd(0x0020,0x000D);
	keys.push_back(std::make_pair(tagsd, ""));

	//// Study Time
	//gdcm::Tag tagst(8,30);
	//keys.push_back(std::make_pair(tagst, ""));

	//// Study Description
	//gdcm::Tag tagsdc(8,1030);
	//keys.push_back(std::make_pair(tagsdc, ""));

	//// Accession n°
	//gdcm::Tag tagacc(8,50);
	//keys.push_back(std::make_pair(tagacc, ""));

	return keys;
}

void vvQPacsConnection::on_check_ModAll_clicked(bool state)
{
	ui.check_MR->setEnabled(!state);
	ui.check_CR->setEnabled(!state);
	ui.check_OT->setEnabled(!state);
	ui.check_RF->setEnabled(!state);
	ui.check_SC->setEnabled(!state);
	ui.check_CT->setEnabled(!state);
	ui.check_US->setEnabled(!state);
	ui.check_NM->setEnabled(!state);
	ui.check_DR->setEnabled(!state);
	ui.check_US->setEnabled(!state);
	ui.check_NM->setEnabled(!state);
	ui.check_DR->setEnabled(!state);
	ui.check_SR->setEnabled(!state);
	ui.check_XA->setEnabled(!state);
	ui.check_MG->setEnabled(!state);
	if(state)
	{
		ui.check_MR->setChecked(state);
		ui.check_CR->setChecked(state);
		ui.check_OT->setChecked(state);
		ui.check_RF->setChecked(state);
		ui.check_SC->setChecked(state);
		ui.check_CT->setChecked(state);
		ui.check_US->setChecked(state);
		ui.check_NM->setChecked(state);
		ui.check_DR->setChecked(state);
		ui.check_US->setChecked(state);
		ui.check_NM->setChecked(state);
		ui.check_DR->setChecked(state);
		ui.check_SR->setChecked(state);
		ui.check_XA->setChecked(state);
		ui.check_MG->setChecked(state);
	}

}

void vvQPacsConnection::chooseServer(int index)
{
	std::map < std::string, std:: string> values = getDicomServer(ui.networkCombo->currentText());
	m_port = values["PORT"];
	m_aetitle = values["AETITLE"];
	m_adress= values["ADRESS"];
	m_nickname = values["nickname"];
	ui.AdressEdit->setText(QString(m_adress.c_str()));
	ui.AETitleEdit->setText(QString(m_aetitle.c_str()));
	ui.NameEdit->setText(QString(m_nickname.c_str()));
	ui.PortEdit->setText(QString(m_port.c_str()));
}

void vvQPacsConnection::on_importButton_clicked()
	{
		setCursor(QCursor(Qt::WaitCursor));
		QString path = QDir::homePath() +QString::fromStdString("/.move");
		QDir dirpath (path);
		if (dirpath.exists())
		{
			QFileInfoList list = dirpath.entryInfoList( QDir::Files);
			QFileInfoList::iterator it_file = dirpath.entryInfoList( QDir::Files).begin();
			for(int i = 0; i < list.length() ; i++)
			{
					QFile::remove(list.at(i).filePath());
			}
		}
		else
		{
			dirpath.mkdir(path);
		}
		bool didItWork =  gdcm::CompositeNetworkFunctions::CMove(m_adress.c_str(),atoi(m_port.c_str()),
			gdcm::CompositeNetworkFunctions::ConstructQuery(mQFactory.getMoveQuery().theRoot, mQFactory.getMoveQuery().theLevel ,mQFactory.getMoveQuery().keys,true),
			getDicomClientPort(),  getDicomClientAETitle().c_str(), m_aetitle.c_str(), path.toStdString().c_str() );
		gdcm::Directory theDir;
		theDir.Load(path.toStdString().c_str());
	   m_files =	theDir.GetFilenames();
	   	std::sort (m_files.begin(), m_files.end()); // make sure names are in lexicographical order
	   
	   accept();
	  setCursor(QCursor(Qt::ArrowCursor));
	}



std::vector <std::string> vvQPacsConnection::getFileNames()
{
	std::vector <std::string> filenames;
	gdcm::Directory::FilenamesType::iterator it = m_files.begin();
	for (;it != m_files.end(); it++)
		filenames.push_back(it->c_str());
	return filenames;
}
std::vector< std::pair<gdcm::Tag, std::string> > vvQPacsConnection::fillMoveKeys()
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
	switch(m_level)
	{
	case gdcm::ePatient:
			//keys.push_back(getPatientKeys("",""));
			break;
	}

	return keys;
}
