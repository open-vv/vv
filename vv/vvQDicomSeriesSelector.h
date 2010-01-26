/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
#ifndef VVDICOMSERIESSELECTOR_H
#define VVDICOMSERIESSELECTOR_H

// vv include
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
