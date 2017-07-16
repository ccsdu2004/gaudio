#include <QDialog>
#include "AboutDlg.h"

AboutDlg::AboutDlg(QWidget *parent,Qt::WFlags flags):
QDialog(parent,flags)
{
	dlg.setupUi(this);
	setWindowTitle("About GPlayer 0.2.1");
	connect(dlg.mClose,SIGNAL(clicked()),this,SLOT(close()));
}

AboutDlg::~AboutDlg(void)
{
}
