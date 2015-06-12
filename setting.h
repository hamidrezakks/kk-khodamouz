#ifndef SETTING_H
#define SETTING_H

#include <QDialog>

#include <QSettings>

namespace Ui {
class setting;
}

class setting : public QDialog
{
    Q_OBJECT

public:
    explicit setting(QWidget *parent = 0);
    ~setting();

    int qMode;
    int questionTime;
    bool randomQuestion;
    int randomQNumb;
    bool playMusic;
    bool musicLoop;
    bool lastState;

    void setDisMode();

    QSettings *mySettingbykk;

signals:
    void exits();

private slots:
    void on_radioButtonQMode1_clicked();

    void on_radioButtonQMode2_clicked();

    void on_radioButtonQMode3_clicked();

    void on_checkBoxQRandom_clicked();

    void on_checkBoxMusic_clicked();

    void on_pushButton_clicked();

    void on_spinBoxQTime_valueChanged(int arg1);

    void on_spinBoxQRNumb_valueChanged(int arg1);

    void on_checkBoxMLoop_clicked();

    void on_checkBoxLastState_clicked();

private:
    Ui::setting *ui;
};

#endif // SETTING_H
