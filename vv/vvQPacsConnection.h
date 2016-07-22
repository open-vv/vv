#ifndef __vvQPacsConnection_h_INCLUDED__
#define __vvQPacsConnection_h_INCLUDED__

#include <QWidget>
#include "ui_vvPacsConnection.h"
#include "gdcmCompositeNetworkFunctions.h"
#include <QStandardItemModel>
#include <QStringListModel>
#include <QFileDialog>
#include "vvDicomServerQueryFactory.h"

  /**
   * \ingroup GUI
   */
  //=====================================================================
 //======================================================================



class vvQPacsConnection : public QDialog
{
	  Q_OBJECT
public:
	  //vvQPacsConnection(){}    
	  vvQPacsConnection(QWidget *parent=0);
    
	  ~vvQPacsConnection(){}
	void refreshNetworks();
	std::vector <std::string> getFileNames(int i_series);
	int getSeriesCount() { return m_fileseries.size();}
	void clearMove();

public slots:
	void selectStudies(const QModelIndex &index);
	void selectSeries(const QModelIndex &index);
	void selectImages(const QModelIndex &index);
	void selectImage(const QModelIndex &index);
  private slots:
    void on_scanButton_clicked();
	void on_clearButton_clicked();
	void on_optionsButton_clicked();
	void on_check_ModAll_clicked(bool state);
	void on_importButton_clicked();
 void chooseServer(int index);
 void removeServer();
 bool close();
 void modifyServer();

  private :

	  	
	  Ui::vvPacsConnection ui;
	std::vector< std::pair<gdcm::Tag, std::string> > getKeys();
	std::vector< std::pair<gdcm::Tag, std::string> > getStudyKeys(const std::string);
	std::vector<gdcm::DataSet> findQuery(vvQuery i_query);

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
	vvDicomServerQueryFactory mQFactory;
	vvQuery m_query;
	vvQuery f_query;
	 gdcm::Directory::FilenamesType m_files;
	 std::vector < gdcm::Directory::FilenamesType> m_fileseries;


	
  }; // class vvQPacsConnection
  //=====================================================================

  

#endif // __vvQPacsConnection_h_INCLUDED__


