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
	bool didItWork = gdcm::CompositeNetworkFunctions::CEcho(m_adress.c_str(), atoi(m_port.c_str()),	"CREATIS", m_nickname.c_str() );
	if (didItWork)
	{
		m_level =gdcm::ePatient;
		std::vector<gdcm::DataSet> theDataSet;
		gdcm::EQueryLevel theLevel = gdcm::ePatient;
		gdcm::ERootType theRoot  = gdcm::ePatientRootType;//ePatientRootType;
		m_query =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,
							getPatientKeys(ui.patientName->toPlainText().toStdString(), 	ui.patientID->toPlainText().toStdString()));
		bool cfindWork = gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), m_query, theDataSet, 	"CREATIS", m_nickname.c_str());
		if( cfindWork)
		{
			convertDataSet(theDataSet,Patientmodel,getPatientKeys("",""));
		} // end cfindwork
	} // end didItwork
}



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
	m_level =gdcm::eStudy;
	Studymodel->removeRows(0, Studymodel->rowCount(),QModelIndex());
	QVariant elt= Patientmodel->data(index.sibling(index.row(),1));
	m_patient=elt.toString().toStdString();
	manageSeriesFilter(true);
	
	std::vector<gdcm::DataSet> theDataSet;
	 m_query = mquery.composeQueryStudy(m_patient);
	if (  gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), 
		m_query, theDataSet, "CREATIS", m_nickname.c_str()) )
	{
		convertDataSet(theDataSet, Studymodel, getStudyKeys(""));
	}
}


void vvQPacsConnection::selectSeries(const QModelIndex &index)
{
	m_level =gdcm::eSeries;
	Seriesmodel->removeRows(0, Seriesmodel->rowCount(),QModelIndex());
	QVariant elt= Studymodel->data(index.sibling(index.row(),3));
	QVariant elt2= Patientmodel->data(index.sibling(ui.patientTreeView->selectionModel()->selectedRows().first().row(),1));


	std::vector<gdcm::DataSet> theDataSet;
	std::vector< std::pair<gdcm::Tag, std::string> > keys;

	//Patient Unique key
	keys.push_back(std::make_pair(gdcm::Tag(0x0010,0x0020), m_patient));

	//Study Unique Key
	keys.push_back(std::make_pair(gdcm::Tag(0x0020,0x000d), elt.toString().toStdString()));


	// Modality
	keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x0060), elt.toString().toStdString()));
	// Description
	keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x0060), elt.toString().toStdString()));
	// Acceptance NUmber????
	keys.push_back(std::make_pair(gdcm::Tag(0x0020,0x000e), elt.toString().toStdString()));
	m_query = mquery.composeQuerySeries(keys);
	if ( gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), m_query, theDataSet, "CREATIS", m_nickname.c_str()), "C:\\Boost")
	{
		keys.clear();
	// Modality
	keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x0060), elt.toString().toStdString()));
	// Description
	keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x0060), elt.toString().toStdString()));
	// Acceptance NUmber????
	keys.push_back(std::make_pair(gdcm::Tag(0x0020,0x000e), elt.toString().toStdString()));
		convertDataSet(theDataSet, Seriesmodel, getSeriesKeys(""));
	}
}

void vvQPacsConnection::selectImages(const QModelIndex &index)
{
	m_level = gdcm::eImage;
	Imagesmodel->removeRows(0, Imagesmodel->rowCount(),QModelIndex());
	QVariant elt= Seriesmodel->data(index.sibling(index.row(),2));
	QVariant elt2= Patientmodel->data(index.sibling(ui.patientTreeView->selectionModel()->selectedRows().first().row(),1));

	//manageImagesFilter(true);
	gdcm::EQueryLevel theLevel = gdcm::eImage;
	gdcm::ERootType theRoot  = gdcm::ePatientRootType;//ePatientRootType;
	std::vector<gdcm::DataSet> theDataSet;
	std::vector< std::pair<gdcm::Tag, std::string> > keys;

	gdcm::Tag tagsd(0x0010,0x0020);
	keys.push_back(std::make_pair(tagsd, m_patient));

	gdcm::Tag tagss(0x0020,0x000e);
	keys.push_back(std::make_pair(tagss, elt.toString().toStdString()));
	//= getStudyKeys(elt.toString().toStdString());

	// Image Description
	gdcm::Tag tagsdc(0x0020,0x0013);
	keys.push_back(std::make_pair(tagsdc, ""));
		gdcm::Tag tagsic(0x0008,0x0018);


	keys.push_back(std::make_pair(tagsic, ""));

	gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);
	keys.clear();
	// SOP Instance UID 
	


	keys.push_back(std::make_pair(tagsdc, ""));
keys.push_back(std::make_pair(tagsic, ""));

	gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), theQuery, theDataSet, "CREATIS", m_nickname.c_str());
	convertDataSet(theDataSet, Imagesmodel, keys);
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

std::vector< std::pair<gdcm::Tag, std::string> > vvQPacsConnection::getPatientKeys(const std::string i_patname, const std::string i_patid)
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
	// Patient Name
	gdcm::Tag tag(0x0010,0x0010);
	keys.push_back(std::make_pair(tag, i_patname));

	//// Patient ID
	gdcm::Tag tagpid(0x0010,0x0020);
	keys.push_back(std::make_pair(tagpid, i_patid));
	return keys;
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


std::vector< std::pair<gdcm::Tag, std::string> > vvQPacsConnection::getSeriesKeys(const std::string i_val)
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
	// Modality
	gdcm::Tag tagsm(0x0008,0x0060);
	keys.push_back(std::make_pair(tagsm, ""));
	// Study date
	gdcm::Tag tagdb(0x0008,0x103e);
	keys.push_back(std::make_pair(tagdb, ""));
	// Study Hour
	gdcm::Tag tagsdh(0x0020,0x000e);
	keys.push_back(std::make_pair(tagsdh, ""));
	// Study Instance UID
	gdcm::Tag tagsid(0x0020,0x1209);
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
		int sel = 0;
		//QModelIndexList list = ui.imagesTreeView-selectedIndexes();
		QModelIndexList indices =  ui.imagesTreeView->selectionModel()->selectedRows(); 
		QModelIndexList::iterator it = indices.begin();
		for(; it != indices.end(); it++)
			sel = it->row();
		QModelIndex index;
		QVariant elt= Patientmodel->data(index.sibling(ui.patientTreeView->selectionModel()->selectedRows().first().row(),1));
		

		gdcm::ERootType theRoot  = gdcm::ePatientRootType;//ePatientRootType;
		std::vector<gdcm::DataSet> theDataSet;
		std::vector< std::pair<gdcm::Tag, std::string> > keys;


		// Study Description
	//	gdcm::Tag tagsdc(0x0010,0x0020);
		//keys.push_back(std::make_pair(tagsdc, elt.toString().toStdString()));

	
	// Study Description


		//gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, m_level ,fillMoveKeys(), true);

		bool didItWork =  gdcm::CompositeNetworkFunctions::CMove(m_adress.c_str(),atoi(m_port.c_str()), m_query, getDicomClientPort(),
      getDicomClientAETitle().c_str(), m_aetitle.c_str(),"D:\\move" );
		 gdcm::Directory theDir;
		theDir.Load("D:\\move");
	   m_files =	theDir.GetFilenames();
	   accept();
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