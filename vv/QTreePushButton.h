#ifndef QTreePushButton_h
#define QTreePushButton_h
#include <QObject>
#include <QPushButton>
#include <QTreeWidgetItem>

class QTreePushButton : public QPushButton
{
    Q_OBJECT

public:
    QTreePushButton();

    void setIndex(int index) {
        m_index = index;
    }
    void setItem(QTreeWidgetItem* item) {
        m_item = item;
    }
    void setColumn(int column) {
        m_column = column;
    }

public slots:
    void clickedIntoATree();

signals:
    void clickedInto(QTreeWidgetItem* item,int column);
    void clickedInto(int index,int column);

private:
    QTreeWidgetItem* m_item;
    int m_index;
    int m_column;
};
#endif
