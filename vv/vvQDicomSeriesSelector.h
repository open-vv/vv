#ifndef VVDICOMSERIESSELECTOR_H
#define VVDICOMSERIESSELECTOR_H
#include "ui_vvDicomSeriesSelector.h"
#include "clitkCommon.h"
#include "vvQProgressDialogITKCommand.h"

namespace gdcm {class File;}

// qt include
#include <QSplitter>
#include <QFileDialog>

class vvDicomSeriesSelector : public QDialog {
    Q_OBJECT

public:
    vvDicomSeriesSelector(QWidget * parent=0);
    virtual void show();
    virtual void close();
    std::vector<std::string> * GetFilenames() {
        return mFilenames;
    }

protected slots:
    void BrowseButtonRelease();
    void SearchButtonRelease();
    void itemSelectionChanged();
    void itemDetailsSelectionChanged();

protected:
    QString mPreviousPath;
    QString mFoldername;
    void AddSerieToTheTable(int i, std::vector<std::string> & filenames);
    QString MakeDicomInfo(std::string & s, gdcm::File *header);
    QString AddInfo(gdcm::File *header, QString n, unsigned short group, unsigned short elem);
    QString AddInfo(std::string n, std::string m);

private:
    Ui::vvDicomSeriesSelector ui;
    std::string mCurrentSerie;
    std::map<std::string, std::vector<std::string>* > mListOfSeriesFilenames;
    std::vector<std::string> * mFilenames;
    std::map<std::string, QString> mDicomInfo;
    std::map<std::string, gdcm::File*> mDicomHeader;
    std::map<std::string, std::string> mDicomDetails;
};

#endif // VVDICOMSERIESSELECTOR_H
