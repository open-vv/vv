#include "vvPacsSettingsDialog.h"	
#include "vvQPacsConnection.h"
#include "vvUtils.h"

vvPacsSettingsDialog::vvPacsSettingsDialog(QWidget *i_parent)
		: QDialog(i_parent)
	{
		mparent = i_parent;
		ui.setupUi(this);
		update();
	}

void vvPacsSettingsDialog::accept()
{

	AddDicomServer(ui.NameEdit->text().toStdString(),ui.AETitleEdit->text().toStdString(),ui.AdressEdit->text().toStdString(), ui.PortEdit->text().toStdString());
((vvQPacsConnection*)this->parent())->refreshNetworks();
	close();
}