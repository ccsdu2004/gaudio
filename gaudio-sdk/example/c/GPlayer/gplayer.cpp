#include <QByteArray>
#include <stdio.h>
#include "gplayer.h"
#include "AboutDlg.h"

static QString AUDIO_MAP_LIST[] = {"AU","AIFF","WAV","FLAC","OGG","MP3","IT","XM","MOD","S3M","MID","MIDI","FLAC","WV"}; 

void GAPICONV GPlayer::GPosition(gsource* source,int32_t time)
{
    void* user = gaudio_source_get_user(source);
	if(user)
	{
	    GPlayer* player = (GPlayer*)user;
		
		time /= 1000;
		int32_t minute = time/60;
		int32_t sec = time%60;

		QString position;
		if(minute < 10)
			position = QString().sprintf("0%d:",minute);
		else
            position = QString().sprintf("%d:",minute);
        if(sec < 10)
            position += QString().sprintf("0%d",sec);
		else
			position += QString().sprintf("%d",sec);
 
		player->ui.mPosition->setText(position);

		int32_t duration;
	    gaudio_source_get_int32(source,AUDIO_ENUM_DURATION,&duration);
		player->ui.mPositionSlider->setRange(0,duration/1000);
		player->ui.mPositionSlider->setValue(time);
	}
}
 
GPlayer::GPlayer(QWidget *parent,Qt::WFlags flags): 
QWidget(parent,flags)
{
    initWidget();
	connectSlots();
	initGAudio();
}

GPlayer::~GPlayer()
{   
	closeAudio();
	gaudio_effect_destroy(bassboost);
	gaudio_effect_destroy(speedchanger);
    gaudio_deinit();
}

void GPlayer::initWidget()
{
    ui.setupUi(this);
	setWindowTitle(tr("GPlayer 0.2.1"));
	
	ui.mBassBoostGroup->setChecked(false);
	
	ui.mVolumeLabel->setText("1.0");
	ui.mVolumeSlider->setRange(0,100);
	ui.mVolumeSlider->setValue(100);

	ui.mPanLabel->setText("0.0");
	ui.mPanSlider->setRange(-10,10);
	ui.mPanSlider->setValue(0);

    ui.mSurroundLabel->setText("0");
	ui.mSurroundSlider->setRange(0,2);
	ui.mSurroundSlider->setValue(0);

	ui.mBassBoostFreq->setRange(AUDIO_EFFECT_BASSBOOST_FREQUENCE_LOW,AUDIO_EFFECT_BASSBOOST_FREQUENCE_HIGH);
    ui.mBassboostGain->setRange(AUDIO_EFFECT_BASSBOOST_GAIN_LOW,AUDIO_EFFECT_BASSBOOST_GAIN_HIGH);
    ui.mBassBoostFreq->setValue(AUDIO_EFFECT_BASSBOOST_FREQUENCE_DEFAULT);
	ui.mBassboostGain->setValue(AUDIO_EFFECT_BASSBOOST_GAIN_DEFAULT);

	ui.mSurroundSlider->setRange(AUDIO_EFFECT_CROSSFFED_LOW,AUDIO_EFFECT_CROSSFFED_HIGH);
    ui.mSurroundSlider->setValue(AUDIO_EFFECT_CROSSFFED_DEFAULT);
	ui.mSurroundLabel->setText(QString().sprintf("%d",AUDIO_EFFECT_CROSSFFED_DEFAULT)); 

    ui.mSpeedSlider->setRange(AUDIO_EFFECT_SPEED_CHANGER_RATIO_LOW*100,AUDIO_EFFECT_SPEED_CHANGER_RATIO_HIGH*100);
	ui.mSpeedSlider->setValue(AUDIO_EFFECT_SPEED_CHANGER_RATIO_DEFAULT*100);
	ui.mSpeedLabel->setText("1.0");
}

void GPlayer::initGAudio()
{
    gaudio_init("addons");
	gaudio_set_string(AUDIO_ENUM_SOUNDFONT,"instruments\\snd.cfg");
	source = 0;
 
	bassboost = gaudio_effect_create(AUDIO_EFFECT_BASSBOOST,FALSE);
	//gaudio_effect_bind(bassboost,0);

	speedchanger = gaudio_effect_create(AUDIO_EFFECT_SPEED_CHANGER,FALSE);
    gaudio_effect_bind(speedchanger,0);
}

void GPlayer::connectSlots()
{   
	connect(ui.mTitle,SIGNAL(open(const QString&)),this,SLOT(open(const QString&)));
	connect(ui.mAbout,SIGNAL(clicked()),this,SLOT(onAbout()));
	connect(ui.mPlay,SIGNAL(clicked()),this,SLOT(onPause()));
	connect(ui.mVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(onVolume(int)));
	connect(ui.mPanSlider,SIGNAL(valueChanged(int)),this,SLOT(onPan(int)));
	connect(ui.mSurroundSlider,SIGNAL(valueChanged(int)),this,SLOT(onSurround(int)));
    connect(ui.mBassBoostGroup,SIGNAL(clicked()),this,SLOT(onUseBassboost()));
	connect(ui.mSurroundSlider,SIGNAL(valueChanged(int)),this,SLOT(onSurround(int)));
    connect(ui.mSpeedSlider,SIGNAL(valueChanged(int)),this,SLOT(onSpeedChanger(int)));
}

void GPlayer::closeAudio()
{   
	if(source)
	{   
		gaudio_source_stop(source);
        gaudio_source_destroy(source);
	    ui.mPositionSlider->setRange(0,100);
        ui.mPositionSlider->setValue(0);
		source = 0;
	}

    ui.mPositionSlider->setRange(0,100);
	ui.mPositionSlider->setValue(0);
	ui.mPosition->setText("00:00");
}

void GPlayer::open(const QString& file)
{   
	closeAudio();
	const char* audio = (const char*)file.toLocal8Bit();
	source = gaudio_source_create_from_file(audio,false);
	if(source)
	{
	    gaudio_source_set_user(source,this);
		gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,TRUE);
	    gaudio_source_set_position_callback(source,GPosition);
		QString label(file);
        label = label.mid(label.lastIndexOf("/")+1);
		
		int32_t samplerate = 44100;
		gaudio_source_get_int32(source,AUDIO_ENUM_SAMPLERATE,&samplerate);
		
		int32_t channel = 2;
		gaudio_source_get_int32(source,AUDIO_ENUM_CHANNEL,&channel);
        
		if(channel == 2)
		    label += QString().sprintf(" [%dkHz stereo 16bit]",samplerate/1000);
		else
		    label += QString().sprintf(" [%dkHz mono 16bit]",samplerate/1000);
        
		label = " " + label;
		ui.mTitle->setText(label);

		ui.mPlay->setText("Play");
		gaudio_source_pause(source);
	}
	else
		ui.mTitle->setText(" .Please drag audio file here!");
}

void GPlayer::onAbout()
{
    AboutDlg dlg(this);
	dlg.exec();
}

void GPlayer::onPause()
{   
	if(!source)
		return;
	int32_t state;
	gaudio_source_get_int32(source,AUDIO_ENUM_PLAYING,&state);
	if(!state)
	{
		ui.mPlay->setText("Pause");
		gaudio_source_play(source,true);
	}
	else 
	{
	    ui.mPlay->setText("Play");
		gaudio_source_pause(source);
	}
}

void GPlayer::onVolume(int v)
{
    float gain = v/100.0f;
	gaudio_set_float(AUDIO_ENUM_GAIN,gain);
	QString label = QString().sprintf("%f",gain);
	label = label.mid(0,4);
	ui.mVolumeLabel->setText(label);
}

void GPlayer::onPan(int p)
{
	if(!source)
		return;
    float pan = (float)p/10.0f;
    gaudio_set_float(/*source,*/AUDIO_ENUM_PAN,pan);
	//gaudio_source_set_float(source,AUDIO_ENUM_PAN,pan);
	QString label = QString().sprintf("%f",pan);
	label = label.mid(0,4);
	ui.mPanLabel->setText(label);
}

void GPlayer::onUseBassboost()
{
	if(ui.mBassBoostGroup->isChecked())
	{
		gaudio_effect_set_int32(bassboost,AUDIO_EFFECT_ENABLE,TRUE);
	}
	else
        gaudio_effect_set_int32(bassboost,AUDIO_EFFECT_ENABLE,FALSE);
}

void GPlayer::onBassboostGain(int gain)
{
    gaudio_effect_set_float(bassboost,AUDIO_EFFECT_BASSBOOST_GAIN,gain);
}

void GPlayer::onBassboostFreq(int freq)
{  
	gaudio_effect_set_float(bassboost,AUDIO_EFFECT_BASSBOOST_FREQUENCE,freq);
}

void GPlayer::onSurround(int value)
{
	//gaudio_effect_set_int32(crossfeed,AUDIO_EFFECT_CROSSFFED_DEEP,value);
	//ui.mSurroundLabel->setText(QString().sprintf("%d",value));
}

void GPlayer::onSpeedChanger(int speed)
{
	float s = speed/100.0f;
	gaudio_effect_set_float(speedchanger,AUDIO_EFFECT_SPEED_CHANGER_RATIO,s);
	QString label = QString().sprintf("%f",s);
	label = label.mid(0,4);
    ui.mSpeedLabel->setText(label);    
}
