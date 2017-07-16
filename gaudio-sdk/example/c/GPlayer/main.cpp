#include <QtGui/QApplication>
#include <QtGui/QCleanlooksStyle>
#include <QTextCodec>
#include "gplayer.h"

#define INSTALL_SYSTEM_CODEC\
    QTextCodec* codec= QTextCodec::codecForName("System");\
    QTextCodec::setCodecForLocale(codec);\
    QTextCodec::setCodecForCStrings(codec);\
    QTextCodec::setCodecForTr(codec);	

int main(int argc, char *argv[])
{   
	INSTALL_SYSTEM_CODEC
	QApplication app(argc,argv);
	QApplication::setStyle(new QCleanlooksStyle());
	GPlayer w;
	w.show();
	return app.exec();
}
