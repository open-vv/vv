#ifndef __vvQPacsConnection_h_INCLUDED__
#define __vvQPacsConnection_h_INCLUDED__

#include <QTGUI/QWidget>
#include "ui_vvPacsConnection.h"
#include "gdcmCompositeNetworkFunctions.h"
#include <QtGui/QStandardItemModel.h>
#include <QtGui/QStringListModel.h>
#include <QFileDialog>
#include "vvDicomServerQueryFactory.h"

  /**
   * \ingroup GUI
   */
  //=====================================================================
 //======================================================================

struct vvQuery{
	gdcm::ERootType theRoot;
	gdcm::EQueryLevel theLevel;
	std::vector< std::pair<gdcm::Tag, std::string> > keys;
};

  class vvQPacsConnection : public QDialog 
  {
	  Q_OBJECT
  public:
	  //vvQPacsConnection(){}    
	  vvQPacsConnection(QWidget *parent=0);
    
	  ~vvQPacsConnection(){}
	void refreshNetworks();
	std::vector <std::string> getFileNames();

public slots:
	void selectStudies(const QModelIndex &index);
	void selectSeries(const QModelIndex &index);
	void selectImages(const QModelIndex &index);
  private slots:
    void on_scanButton_clicked();
	void on_clearButton_clicked();
	void on_optionsButton_clicked();
	void on_check_ModAll_clicked(bool state);
	void on_importButton_clicked();
 void chooseServer(int index);
 void removeServer();
 void modifyServer();

  private :

	  	 vvQuery getQueryPatient(const std::string i_patname, const std::string i_patid);


	  Ui::vvPacsConnection ui;
	std::vector< std::pair<gdcm::Tag, std::string> > getKeys();
	std::vector< std::pair<gdcm::Tag, std::string> > getPatientKeys(const std::string , const std::string );
	std::vector< std::pair<gdcm::Tag, std::string> > getStudyKeys(const std::string);
	std::vector< std::pair<gdcm::Tag, std::string> > getSeriesKeys(const std::string patient_id, const std::string study_id, bool bdisplay);

std::vector< std::pair<gdcm::Tag, std::string> > getQueryKeysforImages(const std::string patient_id, const std::string study_id, const std::string series_id,bool bdisplay);
vvQuery getQueryforImages(const std::string patient_id, const std::string study_id, const std::string series_id,bool bdisplay);
	void manageStudiesFilter(bool i_enable);
	void createTreeView();
	void cleanTree();
	void setNewPacs();
	QStandardItemModel *Patientmodel;
	QStandardItemModel *Studymodel;
	QStandardItemModel *Seriesmodel;
	QStandardItemModel *Imagesmodel;
	void convertDataSet(std::vector<gdcm::DataSet> i_ds, QStandardItemModel *i_model, std::vector< std::pair<gdcm::Tag, std::string> > keys);
	void manageSeriesFilter(bool i_enable);
	std::vector< std::pair<gdcm::Tag, std::string> > fillMoveKeys();
	std::string m_patient;
	std::string m_study;
	std::string m_series;
	gdcm::EQueryLevel m_level;
	std::string m_port;
	std::string m_aetitle;
	std::string m_adress;
	std::string m_nickname;
	vvDicomServerQueryFactory mquery;
	vvQuery m_query;
	vvQuery f_query;
	 gdcm::Directory::FilenamesType m_files;

	 vvQuery getQueryforSeries(const std::string patient_id, const std::string study_id, bool bdisplay);

	 vvQuery getQueryforStudy(const std::string patient_id, bool bdisplay);
	 std::vector< std::pair<gdcm::Tag, std::string> > getQueryKeysforStudy(const std::string patient_id, bool bdisplay);
	
  }; // class vvQPacsConnection
  //=====================================================================

  

#endif // __vvQPacsConnection_h_INCLUDED__


