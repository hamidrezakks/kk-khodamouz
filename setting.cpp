#include "setting.h"
#include "ui_setting.h"

#include "mainwindow.h"

#include <QSettings>


setting::setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setting)
{
    ui->setupUi(this);

    ui->spinBoxQRNumb->setDisabled(true);
    ui->label_2->setDisabled(true);
    ui->spinBoxQTime->setDisabled(true);
    ui->label_3->setDisabled(true);

    qMode = 1;
    questionTime = 60;
    randomQNumb = 10;
    randomQuestion = false;

    playMusic = true;
    musicLoop = true;

    QSettings settings(settingAddress, QSettings::IniFormat);
    musicLoop = settings.value(MUSIC_LOOP, "").toBool();
    playMusic = settings.value(PLAY_MUSIC, "").toBool();
    questionTime = settings.value(QUESTION_TIME, "").toInt();
    qMode = settings.value(QUESTION_MODE, "").toInt();
    randomQuestion = settings.value(RANDOM_QUESTION, "").toBool();
    randomQNumb = settings.value(QUESTION_NUMBER, "").toInt();
    lastState = settings.value(LAST_STATE, "").toBool();

    setDisMode();

}

setting::~setting()
{
    delete ui;
}

void setting::setDisMode()
{
    ui->checkBoxMLoop->setChecked(musicLoop);
    ui->checkBoxMusic->setChecked(playMusic);
    ui->spinBoxQTime->setValue(questionTime);
    if(qMode == 1)
        ui->radioButtonQMode1->setChecked(true);
    else if(qMode == 2)
        ui->radioButtonQMode2->setChecked(true);
    else if(qMode == 3)
        ui->radioButtonQMode3->setChecked(true);
    ui->checkBoxQRandom->setChecked(randomQuestion);
    ui->spinBoxQRNumb->setValue(randomQNumb);

    if(qMode == 1)
    {
        ui->spinBoxQTime->setDisabled(true);
        ui->label_2->setDisabled(true);
    }
    else
    {
        ui->spinBoxQTime->setDisabled(false);
        ui->label_2->setDisabled(false);
    }

    if(randomQuestion)
    {
        ui->spinBoxQRNumb->setDisabled(false);
        ui->label_3->setDisabled(false);
    }
    else
    {
        ui->spinBoxQRNumb->setDisabled(true);
        ui->label_3->setDisabled(true);
    }

    ui->checkBoxLastState->setChecked(lastState);
}

void setting::on_radioButtonQMode1_clicked()
{
    if(ui->radioButtonQMode1->isChecked())
        qMode = 1;
    setDisMode();
}

void setting::on_radioButtonQMode2_clicked()
{
    if(ui->radioButtonQMode2->isChecked())
        qMode = 2;
    setDisMode();
}

void setting::on_radioButtonQMode3_clicked()
{
    if(ui->radioButtonQMode3->isChecked())
        qMode = 3;
    setDisMode();
}

void setting::on_checkBoxQRandom_clicked()
{
    randomQuestion = ui->checkBoxQRandom->isChecked();
    setDisMode();
}

void setting::on_checkBoxMusic_clicked()
{
    playMusic = ui->checkBoxMusic->isChecked();
}

void setting::on_checkBoxMLoop_clicked()
{
    musicLoop = ui->checkBoxMLoop->isChecked();
}

void setting::on_spinBoxQTime_valueChanged(int arg1)
{
    questionTime = arg1;
}

void setting::on_spinBoxQRNumb_valueChanged(int arg1)
{
    randomQNumb = arg1;
}

void setting::on_pushButton_clicked()
{
    /*QSettings settings(QDir::currentPath()+"/seting.ini", QSettings::NativeFormat);
    settings.setValue(PLAY_MUSIC,playMusic);*/
    QSettings settings(settingAddress, QSettings::IniFormat);
    settings.setValue(MUSIC_LOOP, musicLoop);
    settings.setValue(PLAY_MUSIC, playMusic);
    settings.setValue(QUESTION_TIME, questionTime);
    settings.setValue(QUESTION_MODE, qMode);
    settings.setValue(RANDOM_QUESTION, randomQuestion);
    settings.setValue(QUESTION_NUMBER, randomQNumb);
    settings.setValue(LAST_STATE, lastState);

    emit exits();
    this->close();
}


void setting::on_checkBoxLastState_clicked()
{
    lastState = ui->checkBoxLastState->isChecked();
}
