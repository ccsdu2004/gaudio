#ifndef GPLAYER_H
#define GPLAYER_H
#include <QWidget>
#include "..\..\..\include\gaudio.h"
#include "ui_gplayer.h"

class GPlayer : public QWidget
{
	Q_OBJECT
public:
	static void GAPICONV GPosition(gsource* source,int32_t time);
public:
	GPlayer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GPlayer();
private slots:
	void open(const QString& file);
	void onAbout();
    void onPause();
	void onVolume(int);
	void onPan(int);
	void onSurround(int);
	void onUseBassboost();
	void onBassboostGain(int);
	void onBassboostFreq(int);
	void onSpeedChanger(int);
private:
	void initWidget();
	void initGAudio();
	void connectSlots();
	void closeAudio();
private:
	gsource* source;
	geffect* bassboost;
	geffect* speedchanger;
	Ui::Widget ui;
};

#endif // GPLAYER_H
