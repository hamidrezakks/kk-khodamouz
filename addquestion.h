#ifndef ADDQUESTION_H
#define ADDQUESTION_H

#include <QDialog>
#include <QPixmap>
#include <QRubberBand>
#include <QTimer>
#include <QSettings>

namespace Ui {
class addquestion;
}

class addquestion : public QDialog
{
    Q_OBJECT

public:
    explicit addquestion(QWidget *parent = 0, int itemId = -1, int qCat = 0);
    ~addquestion();
    void resizeEvent(QResizeEvent * event );

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void closeEvent(QCloseEvent *);

    bool startCheckPic, startCheckNumb, startCheckPicSol, startCheckNumbSol;
    QPoint startPointPic, startPointNumb, startPointPicSol, startPointNumbSol;
    QPoint endPointPic, endPointNumb, endPointPicSol, endPointNumbSol;
    QRubberBand *cropBandPic, *cropBandNumb, *cropBandPicSol, *cropBandNumbSol;

    QString fileName, fileNameSol;
    QPixmap *pix, *pixSol;

    QString currentGrade;
    int currentGradeIndex, currentQIndex;

    bool isQ, isPic;

    QString getDirByIndex(int index);

    QRect lastWindowSize;
    QTimer *resizeTimer;

    void showCategory(int qCat=0, int gCatIndex=0);

    bool imageSelected;
    int itemID;

    QString lastPicNumb, lastSolNumb;

    QString getNumbRectStr(QRect image, QRect layout, QPoint start, QPoint end, QPoint topLeft, QPoint bottomRight, QPoint offset);

    QPixmap cropImage(QPixmap *imagePix, QRect image, QRect layout, QPoint start, QPoint end, QPoint offset);

    int lastQCat;
    bool imageSelectedSol;

private slots:
    void on_pushButtonBrowse_clicked();

    void on_pushButtonAdd_clicked();

    void on_comboBoxQCat_currentIndexChanged(int index);

    void on_comboBoxGradeCat_currentIndexChanged(int index);

    void on_tabWidget_currentChanged(int index);

    void on_radioButtonQ_toggled(bool checked);

    void timerResizeEvent();

private:
    Ui::addquestion *ui;
};

#endif // ADDQUESTION_H
