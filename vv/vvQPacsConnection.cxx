#include "vvQPacsConnection.h"
#include "gdcmCompositeNetworkFunctions.h"
#include <QtGui/qlistview.h>
#include <qfile.h>
#include <QDate>
#include <QDateTime>


	vvQPacsConnection::vvQPacsConnection(QWidget *i_parent)
		:QWidget(i_parent)
	{
		 setupUi(this);
		 setWindowTitle(QString::fromUtf8("DCM API PACS"));
		 createTreeView();
		 tabFilter->setTabText(0,QString(tr("Modality")));
		 tabFilter->setTabText(1,QString(tr("Date")));

		 tabNetwork->setTabText(0,QString(tr("Network")));
		 tabNetwork->setTabText(1,QString(tr("Configuration")));
		 check_ModAll->setChecked(true);
		 on_check_ModAll_clicked(true);
		 QIcon icon;
		 icon.addFile("basket_download.png",QSize(),QIcon::Normal,QIcon::Off);
		 importButton->setIcon(icon);
		 icon.addFile("file_upload.png",QSize(),QIcon::Normal,QIcon::Off);
		// exportButton->setIcon(icon);
		 icon.addFile("email_forward.png",QSize(),QIcon::Normal,QIcon::Off);
		 icon.addFile("edit.png",QSize(),QIcon::Normal,QIcon::Off);
		 optionsButton->setIcon(icon);
		 icon.addFile("bullet_info.png",QSize(),QIcon::Normal,QIcon::Off);
		 helpButton->setIcon(icon);
		 update();
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
		//on_clearButton_clicked();
		manageStudiesFilter(true);
		#if defined (USE_GDCM2)
	    bool didItWork = gdcm::CompositeNetworkFunctions::CEcho( "127.0.0.1", 5678,
			"CREATIS", "CONQUESTSRV1" );
				/*  tag.SetElement(8);
		  tag.SetGroup(8);*/
		std::vector< std::pair<gdcm::Tag, std::string> > keys = getKeys();

		//std::pair<gdcm::Tag, std::string> pa;
		//pa.first = tag;
		//pa.second = "";
		////keys.push_back(std::make_pair(tag, ""));
		gdcm::EQueryLevel theLevel = gdcm::eStudy;
		gdcm::ERootType theRoot  = gdcm::eStudyRootType;//ePatientRootType;
		//gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);

	
		    std::vector<gdcm::DataSet> theDataSet;

			  theLevel = gdcm::ePatient;
  theRoot  = gdcm::ePatientRootType;//ePatientRootType;
  gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,getPatientKeys(""));

//	
//theDataSet.clear();
//  gdcm::CompositeNetworkFunctions::CFind("127.0.0.1", 5678, theQuery, theDataSet,
//       "CREATIS", "CONQUESTSRV1");
//   keys.clear();
//   keys.push_back(std::make_pair(gdcm::Tag(0x0010,0x0010), ""));			
//			
			gdcm::CompositeNetworkFunctions::CFind("127.0.0.1", 5678, theQuery, theDataSet,
       "CREATIS", "CONQUESTSRV1");
  std::vector<gdcm::DataSet>::iterator it_ds = theDataSet.begin();
//
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
//   gdcm::Tag tagtest(0x0020,0x1208);
//   keys.clear();
//   keys.push_back(std::make_pair(gdcm::Tag(0x0010,0x0010), ""));
////  keys.push_back(std::make_pair(tagtest, ""));
//  theLevel = gdcm::ePatient;
//  theRoot  = gdcm::ePatientRootType;//ePatientRootType;
//  theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);
//
//	
//theDataSet.clear();
//  gdcm::CompositeNetworkFunctions::CFind("127.0.0.1", 5678, theQuery, theDataSet,
//       "CREATIS", "CONQUESTSRV1");
#endif
	}


	void vvQPacsConnection::on_importButton_clicked()
	{
		QModelIndex index;
		QVariant elt= Patientmodel->data(index.sibling(patientTreeView->selectionModel()->selectedRows().first().row(),1));
		
		gdcm::EQueryLevel theLevel = gdcm::ePatient;
		gdcm::ERootType theRoot  = gdcm::ePatientRootType;//ePatientRootType;
		std::vector<gdcm::DataSet> theDataSet;
		std::vector< std::pair<gdcm::Tag, std::string> > keys;
		
		// Study Description
		gdcm::Tag tagsdc(0x0010,0x0020);
		keys.push_back(std::make_pair(tagsdc, elt.toString().toStdString()));
				gdcm::SmartPointer<gdcm::BaseRootQuery> theQuery =  gdcm::CompositeNetworkFunctions::ConstructQuery(theRoot, theLevel ,keys);
		gdcm::CompositeNetworkFunctions::CFind("127.0.0.1", 5678, theQuery, theDataSet, "CREATIS", "CONQUESTSRV1");
	
		 bool didItWork =  gdcm::CompositeNetworkFunctions::CMove( "127.0.0.1", 5678, theQuery, 0,
       "CREATIS", "CONQUESTSRV1", "d:\\temp_pacs_import\\" );
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
		patientTreeView->setModel(Patientmodel);
		patientTreeView->setEnabled(true);
		connect(patientTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectStudies(QModelIndex)));
		
		// Study Tree View
		Studymodel = new QStandardItemModel(0,3,this); 
		QStringList Studylist;
		Studylist.push_back(tr("DESCRIPTION"));
		Studylist.push_back(tr("DATE"));
		Studylist.push_back(tr("HOUR"));
		Studymodel->setHorizontalHeaderLabels(Studylist);
		studyTreeView->setModel(Studymodel);
		connect(studyTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectSeries(QModelIndex)));


		// Series Tree View
		Seriesmodel = new QStandardItemModel(0,2,this); 
		QStringList Serieslist;
		Serieslist.push_back(tr("MODALITY"));
		Serieslist.push_back(tr("DESCRIPTION"));
		Serieslist.push_back(tr("no. accept."));
		Seriesmodel->setHorizontalHeaderLabels(Serieslist);
		seriesTreeView->setModel(Seriesmodel);
		connect(seriesTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectImages(QModelIndex)));

		// Images Tree View
		Imagesmodel = new QStandardItemModel(0,1,this); 
		QStringList Imageslist;
		Imageslist.push_back(tr("instance number"));
		Imagesmodel->setHorizontalHeaderLabels(Imageslist);
		imagesTreeView->setModel(Imagesmodel);

		//model->setHeaderData(0, Qt::Horizontal,tr("test"),Qt::DisplayRole);
		/*QFileSystemModel *filemodel = new   QFileSystemModel;
		filemodel->insertColumn(1);
		filemodel->setHeaderData(1, Qt::Horizontal,"test",0);*/

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
		gdcm::CompositeNetworkFunctions::CFind("127.0.0.1", 5678, theQuery, theDataSet, "CREATIS", "CONQUESTSRV1");
		convertDataSet(theDataSet, Studymodel, getStudyKeys(""));
	}


	void vvQPacsConnection::selectSeries(const QModelIndex &index)
	{
		Seriesmodel->removeRows(0, Seriesmodel->rowCount(),QModelIndex());
		QVariant elt= Studymodel->data(index.sibling(index.row(),3));
		QVariant elt2= Patientmodel->data(index.sibling(patientTreeView->selectionModel()->selectedRows().first().row(),1));

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



		gdcm::CompositeNetworkFunctions::CFind("127.0.0.1", 5678, theQuery, theDataSet, "CREATIS", "CONQUESTSRV1");
		convertDataSet(theDataSet, Seriesmodel, keys);
	}

	void vvQPacsConnection::selectImages(const QModelIndex &index)
	{
		Imagesmodel->removeRows(0, Imagesmodel->rowCount(),QModelIndex());
		QVariant elt= Seriesmodel->data(index.sibling(index.row(),2));
		QVariant elt2= Patientmodel->data(index.sibling(patientTreeView->selectionModel()->selectedRows().first().row(),1));

		//manageImagesFilter(true);
		gdcm::EQueryLevel theLevel = gdcm::eImageOrFrame;
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
		

		gdcm::CompositeNetworkFunctions::CFind("127.0.0.1", 5678, theQuery, theDataSet, "CREATIS", "CONQUESTSRV1");
		convertDataSet(theDataSet, Seriesmodel, keys);
	}


	void vvQPacsConnection::manageStudiesFilter(bool i_enable)
	{
		text_PHYS->setEnabled(i_enable);
		text_SDESC->setEnabled(i_enable);
		dateTab->setEnabled(i_enable);

	}

	void vvQPacsConnection::manageSeriesFilter(bool i_enable)
	{
		modalityTab->setEnabled(i_enable);
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
		check_MR->setEnabled(!state);
		check_CR->setEnabled(!state);
		check_OT->setEnabled(!state);
		check_RF->setEnabled(!state);
		check_SC->setEnabled(!state);
		check_CT->setEnabled(!state);
		check_US->setEnabled(!state);
		check_NM->setEnabled(!state);
		check_DR->setEnabled(!state);
		check_US->setEnabled(!state);
		check_NM->setEnabled(!state);
		check_DR->setEnabled(!state);
		check_SR->setEnabled(!state);
		check_XA->setEnabled(!state);
		check_MG->setEnabled(!state);
		if(state)
		{
			check_MR->setChecked(state);
			check_CR->setChecked(state);
			check_OT->setChecked(state);
			check_RF->setChecked(state);
			check_SC->setChecked(state);
			check_CT->setChecked(state);
			check_US->setChecked(state);
			check_NM->setChecked(state);
			check_DR->setChecked(state);
			check_US->setChecked(state);
			check_NM->setChecked(state);
			check_DR->setChecked(state);
			check_SR->setChecked(state);
			check_XA->setChecked(state);
			check_MG->setChecked(state);
		}

	}