#ifndef __vvPacsSettingsDialog_H
#define __vvPacsSettingsDialog_H
#include "ui_vvPacsSettingsDialog.h"
#include <QFileDialog>
#include <QWidget>

 class vvPacsSettingsDialog : public QDialog 
  {
	  Q_OBJECT
  public:
	  
	  vvPacsSettingsDialog(QWidget *parent=0);
    
	  ~vvPacsSettingsDialog(){}
  private:
	  QWidget *mparent;
	  Ui::vvPacsSettingsDialog ui;
private slots:
	void accept();

 };
#endif //__vvPacsSettingsDialog_H
