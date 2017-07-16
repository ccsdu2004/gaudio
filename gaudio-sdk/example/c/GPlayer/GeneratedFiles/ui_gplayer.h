/********************************************************************************
** Form generated from reading UI file 'gplayer.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GPLAYER_H
#define UI_GPLAYER_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "qlabelwithdrag.h"

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout;
    QLabelWithDrag *mTitle;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QLabel *mPosition;
    QSlider *mPositionSlider;
    QHBoxLayout *horizontalLayout_2;
    QGridLayout *gridLayout_3;
    QPushButton *mPlay;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QSlider *mSpeedSlider;
    QLabel *mSpeedLabel;
    QLabel *label_4;
    QSlider *mSurroundSlider;
    QLabel *mSurroundLabel;
    QLabel *label_5;
    QSlider *mVolumeSlider;
    QLabel *mVolumeLabel;
    QLabel *label_6;
    QSlider *mPanSlider;
    QLabel *mPanLabel;
    QGroupBox *mBassBoostGroup;
    QGridLayout *gridLayout_2;
    QSlider *mBassboostGain;
    QLabel *label;
    QLabel *label_2;
    QSlider *mBassBoostFreq;
    QFrame *line_2;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *mAbout;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(445, 280);
        Widget->setMaximumSize(QSize(16777215, 280));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        Widget->setFont(font);
        Widget->setStyleSheet(QString::fromUtf8(""));
        Widget->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(Widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mTitle = new QLabelWithDrag(Widget);
        mTitle->setObjectName(QString::fromUtf8("mTitle"));
        mTitle->setEnabled(true);
        mTitle->setMaximumSize(QSize(16777215, 27));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier New"));
        font1.setPointSize(14);
        mTitle->setFont(font1);
        mTitle->setStyleSheet(QString::fromUtf8(""));

        verticalLayout->addWidget(mTitle);

        line = new QFrame(Widget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        mPosition = new QLabel(Widget);
        mPosition->setObjectName(QString::fromUtf8("mPosition"));
        QFont font2;
        font2.setPointSize(10);
        mPosition->setFont(font2);

        horizontalLayout->addWidget(mPosition);

        mPositionSlider = new QSlider(Widget);
        mPositionSlider->setObjectName(QString::fromUtf8("mPositionSlider"));
        mPositionSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(mPositionSlider);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        mPlay = new QPushButton(Widget);
        mPlay->setObjectName(QString::fromUtf8("mPlay"));
        mPlay->setEnabled(true);
        mPlay->setMaximumSize(QSize(16777215, 16777215));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Courier New"));
        font3.setPointSize(11);
        mPlay->setFont(font3);

        gridLayout_3->addWidget(mPlay, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(248, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 0, 1, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_3 = new QLabel(Widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Courier New"));
        font4.setPointSize(10);
        label_3->setFont(font4);

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        mSpeedSlider = new QSlider(Widget);
        mSpeedSlider->setObjectName(QString::fromUtf8("mSpeedSlider"));
        mSpeedSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(mSpeedSlider, 0, 1, 1, 1);

        mSpeedLabel = new QLabel(Widget);
        mSpeedLabel->setObjectName(QString::fromUtf8("mSpeedLabel"));

        gridLayout->addWidget(mSpeedLabel, 0, 2, 1, 1);

        label_4 = new QLabel(Widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font4);

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        mSurroundSlider = new QSlider(Widget);
        mSurroundSlider->setObjectName(QString::fromUtf8("mSurroundSlider"));
        mSurroundSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(mSurroundSlider, 1, 1, 1, 1);

        mSurroundLabel = new QLabel(Widget);
        mSurroundLabel->setObjectName(QString::fromUtf8("mSurroundLabel"));

        gridLayout->addWidget(mSurroundLabel, 1, 2, 1, 1);

        label_5 = new QLabel(Widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font4);

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        mVolumeSlider = new QSlider(Widget);
        mVolumeSlider->setObjectName(QString::fromUtf8("mVolumeSlider"));
        mVolumeSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(mVolumeSlider, 2, 1, 1, 1);

        mVolumeLabel = new QLabel(Widget);
        mVolumeLabel->setObjectName(QString::fromUtf8("mVolumeLabel"));

        gridLayout->addWidget(mVolumeLabel, 2, 2, 1, 1);

        label_6 = new QLabel(Widget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font4);

        gridLayout->addWidget(label_6, 3, 0, 1, 1);

        mPanSlider = new QSlider(Widget);
        mPanSlider->setObjectName(QString::fromUtf8("mPanSlider"));
        mPanSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(mPanSlider, 3, 1, 1, 1);

        mPanLabel = new QLabel(Widget);
        mPanLabel->setObjectName(QString::fromUtf8("mPanLabel"));

        gridLayout->addWidget(mPanLabel, 3, 2, 1, 1);


        gridLayout_3->addLayout(gridLayout, 1, 0, 1, 2);


        horizontalLayout_2->addLayout(gridLayout_3);

        mBassBoostGroup = new QGroupBox(Widget);
        mBassBoostGroup->setObjectName(QString::fromUtf8("mBassBoostGroup"));
        mBassBoostGroup->setCheckable(true);
        gridLayout_2 = new QGridLayout(mBassBoostGroup);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        mBassboostGain = new QSlider(mBassBoostGroup);
        mBassboostGain->setObjectName(QString::fromUtf8("mBassboostGain"));
        mBassboostGain->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(mBassboostGain, 0, 0, 1, 1);

        label = new QLabel(mBassBoostGroup);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        label_2 = new QLabel(mBassBoostGroup);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 1, 1, 1, 1);

        mBassBoostFreq = new QSlider(mBassBoostGroup);
        mBassBoostFreq->setObjectName(QString::fromUtf8("mBassBoostFreq"));
        mBassBoostFreq->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(mBassBoostFreq, 0, 1, 1, 1);


        horizontalLayout_2->addWidget(mBassBoostGroup);


        verticalLayout->addLayout(horizontalLayout_2);

        line_2 = new QFrame(Widget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        mAbout = new QPushButton(Widget);
        mAbout->setObjectName(QString::fromUtf8("mAbout"));
        mAbout->setFont(font4);

        horizontalLayout_4->addWidget(mAbout);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", 0, QApplication::UnicodeUTF8));
        mTitle->setProperty("text", QVariant(QApplication::translate("Widget", " Please drag audio file here!", 0, QApplication::UnicodeUTF8)));
        mPosition->setText(QApplication::translate("Widget", "00:00", 0, QApplication::UnicodeUTF8));
        mPlay->setText(QApplication::translate("Widget", "Play", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Widget", "playback speed", 0, QApplication::UnicodeUTF8));
        mSpeedLabel->setText(QApplication::translate("Widget", "1.00", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Widget", "surround", 0, QApplication::UnicodeUTF8));
        mSurroundLabel->setText(QApplication::translate("Widget", "1.00", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Widget", "volume", 0, QApplication::UnicodeUTF8));
        mVolumeLabel->setText(QApplication::translate("Widget", "1.00", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Widget", "pan", 0, QApplication::UnicodeUTF8));
        mPanLabel->setText(QApplication::translate("Widget", "0.00", 0, QApplication::UnicodeUTF8));
        mBassBoostGroup->setTitle(QApplication::translate("Widget", "BassBoost", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Widget", "Gain", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Widget", "Freq", 0, QApplication::UnicodeUTF8));
        mAbout->setText(QApplication::translate("Widget", "?", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GPLAYER_H
