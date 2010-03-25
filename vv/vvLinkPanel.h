#ifndef _vvLinkPanel_H
#define _vvLinkPanel_H
#include <iostream>
#include <vector>

#include "ui_vvLinkPanel.h"

//====================================================================
class vvLinkPanel : public QWidget, private Ui::vvLinkPanel
{

    Q_OBJECT

public:
    // constructor - destructor
    vvLinkPanel(QWidget * parent=0);
    ~vvLinkPanel() {}

    void addImage(std::string name, std::string id);
    void removeImage(int i);

public slots :
    void UpdateComboBox2(int i);
    void removeLink(int row, int column);
    void addLink();
    void linkAll();

signals:
    void addLink(QString image1,QString image2);
    void removeLink(QString image1,QString image2);

private:
    void UpdateComboBox1();

    std::vector<std::string> imageNames;
    std::vector<std::string> image1Ids;
    std::vector<std::string> image2Ids;

}; // end class vvLinkPanel
//====================================================================

#endif /* end #define _vvLinkPanel_H */

