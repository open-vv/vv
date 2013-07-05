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
	//ui.setChecked(true);
	//ui.on_check_ModAll_clicked(true);
	QIcon icon;
	icon.addFile("basket_download.png",QSize(),QIcon::Normal,QIcon::Off);
	ui.importButton->setIcon(icon);
	icon.addFile("file_upload.png",QSize(),QIcon::Normal,QIcon::Off);
	// exportButton->setIcon(icon);
	icon.addFile("email_forward.png",QSize(),QIcon::Normal,QIcon::Off);
	icon.addFile("edit.png",QSize(),QIcon::Normal,QIcon::Off);
	ui.optionsButton->setIcon(icon);
	icon.addFile("bullet_info.png",QSize(),QIcon::Normal,QIcon::Off);
	ui.helpButton->setIcon(icon);

	ui.networkCombo->addItems(getDicomServers());
	connect(ui.networkCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(connectServer(int)));
	connect(ui.scanButton,SIGNAL(clicked()),this,SLOT(on_scanButton_clicked()));
	update();
}

void vvQPacsConnection::refreshNetworks()
{
	ui.networkCombo->clear();
	ui.networkCombo->addItems(getDicomServers());
	//ui.networkCombo->show();
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

	manageStudiesFilter(true);
	bool didItWork = gdcm::CompositeNetworkFunctions::CEcho(m_adress.c_str(), atoi(m_port.c_str()),	"CREATIS", m_nickname.c_str() );
	std::vector< std::pair<gdcm::Tag, std::string> > keys = getKeys();

	gdcm::EQueryLevel theLevel = gdcm::eStudy;
	gdcm::ERootType theRoot  = gdcm::eStudyRootType;//ePatientRootType;
	
	std::vector<gdcm::DataSet> theDataSet;
	theLevel = gdcm::ePatient;
	theRoot  = gdcm::ePatientRootType;//ePatientRootType;
	gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,getPatientKeys(""));
	gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), theQuery, theDataSet, 	"CREATIS", m_nickname.c_str());
	std::vector<gdcm::DataSet>::iterator it_ds = theDataSet.begin();

	for(; it_ds != theDataSet.end(); it_ds++)
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
		Patientmodel->appendRow(items);
	}
}


void vvQPacsConnection::on_optionsButton_clicked()
{
	vvPacsSettingsDialog *dg  = new vvPacsSettingsDialog(this);
	dg->show();
}

void vvQPacsConnection::convertDataSet(std::vector<gdcm::DataSet> i_ds, QStandardItemModel *i_model, std::vector< std::pair<gdcm::Tag, std::string> > keys)
{
	gdcm::Tag tagdb(0x0008,0x0020);
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
				/*if((*it_key).first == tagdb)
				{
				QDate date;
				date.fromString(tr(buffer.c_str()),"yyyy'MM'd");
				item->setText(date.toString());
				}
				else
				{*/
				item->setText(tr(buffer.c_str()));
				//}
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
	Imagesmodel->setHorizontalHeaderLabels(Imageslist);
	ui.imagesTreeView->setModel(Imagesmodel);
}

void vvQPacsConnection::selectStudies(const QModelIndex &index)
{
	Studymodel->removeRows(0, Studymodel->rowCount(),QModelIndex());
	QVariant elt= Patientmodel->data(index.sibling(index.row(),1));
	m_patient=elt.toString().toStdString();
	manageSeriesFilter(true);
	gdcm::EQueryLevel theLevel = gdcm::eStudy;
	gdcm::ERootType theRoot  = gdcm::ePatientRootType;//ePatientRootType;
	std::vector<gdcm::DataSet> theDataSet;
	std::vector< std::pair<gdcm::Tag, std::string> > keys;

	// Study Description
	gdcm::Tag tagsd(0x0010,0x0020);
	keys.push_back(std::make_pair(tagsd, m_patient));
	// Study Description
	gdcm::Tag tagsdc(0x0008,0x1030);
	keys.push_back(std::make_pair(tagsdc, ""));
	// Study date
	gdcm::Tag tagdb(0x0008,0x0020);
	keys.push_back(std::make_pair(tagdb, ""));
	// Study Hour
	gdcm::Tag tagsdh(0x0008,0x0030);
	keys.push_back(std::make_pair(tagsdh, ""));
	gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);
	gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), theQuery, theDataSet, "CREATIS", m_nickname.c_str());
	convertDataSet(theDataSet, Studymodel, getStudyKeys(""));
}


void vvQPacsConnection::selectSeries(const QModelIndex &index)
{
	Seriesmodel->removeRows(0, Seriesmodel->rowCount(),QModelIndex());
	QVariant elt= Studymodel->data(index.sibling(index.row(),3));
	QVariant elt2= Patientmodel->data(index.sibling(ui.patientTreeView->selectionModel()->selectedRows().first().row(),1));

	//manageImagesFilter(true);
	gdcm::EQueryLevel theLevel = gdcm::eSeries;
	gdcm::ERootType theRoot  = gdcm::ePatientRootType;//ePatientRootType;
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

	gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);
	keys.clear();
	// Modality
	keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x0060), elt.toString().toStdString()));
	// Description
	keys.push_back(std::make_pair(gdcm::Tag(0x0008,0x0060), elt.toString().toStdString()));
	// Acceptance NUmber????
	keys.push_back(std::make_pair(gdcm::Tag(0x0020,0x000e), elt.toString().toStdString()));

	gdcm::CompositeNetworkFunctions::CFind(m_adress.c_str(), atoi(m_port.c_str()), theQuery, theDataSet, "CREATIS", m_nickname.c_str());
	convertDataSet(theDataSet, Seriesmodel, keys);
}

void vvQPacsConnection::selectImages(const QModelIndex &index)
{
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

	// Study Description
	gdcm::Tag tagsdc(0x0020,0x0013);
	keys.push_back(std::make_pair(tagsdc, ""));


	gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);
	keys.clear();
	// Study Description

	keys.push_back(std::make_pair(tagsdc, ""));

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

std::vector< std::pair<gdcm::Tag, std::string> > vvQPacsConnection::getPatientKeys(const std::string i_val)
{
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
	// Patient Name
	gdcm::Tag tag(0x0010,0x0010);
	keys.push_back(std::make_pair(tag, ""));

	//// Patient ID
	gdcm::Tag tagpid(0x0010,0x0020);
	keys.push_back(std::make_pair(tagpid, i_val));
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

void vvQPacsConnection::connectServer(int index)
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
		QModelIndex index;
		QVariant elt= Patientmodel->data(index.sibling(ui.patientTreeView->selectionModel()->selectedRows().first().row(),1));
		
		gdcm::EQueryLevel theLevel = gdcm::ePatient;
		gdcm::ERootType theRoot  = gdcm::ePatientRootType;//ePatientRootType;
		std::vector<gdcm::DataSet> theDataSet;
		std::vector< std::pair<gdcm::Tag, std::string> > keys;
		
		// Study Description
		gdcm::Tag tagsdc(0x0010,0x0020);
		keys.push_back(std::make_pair(tagsdc, elt.toString().toStdString()));
				gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);
	//	gdcm::CompositeNetworkFunctions::CFind("127.0.0.1", 5678, theQuery, theDataSet, "CREATIS", "CONQUESTSRV1");
	
		 bool didItWork =  gdcm::CompositeNetworkFunctions::CMove(m_adress.c_str(),atoi(m_port.c_str()), theQuery, 0,
       "CREATIS", m_nickname.c_str(), "d:\\temp_pacs_import\\" );
	}