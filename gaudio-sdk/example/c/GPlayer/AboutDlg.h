#pragma once
#include <QtGui/QDialog>
#include "ui_about.h"

class AboutDlg : public QDialog
{
public:
	AboutDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~AboutDlg(void);
private:
	Ui::Dialog dlg;
};
