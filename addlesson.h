#ifndef ADDLESSON_H
#define ADDLESSON_H

#include <QDialog>
#include <QPixmap>
#include <QRubberBand>
#include <QTimer>
#include <QSettings>

namespace Ui {
class addlesson;
}

class addlesson : public QDialog
{
    Q_OBJECT

public:
    explicit addlesson(QWidget *parent = 0, int itemId = -1);
    ~addlesson();
    void resizeEvent(QResizeEvent * event );

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void closeEvent(QCloseEvent *);

    bool startCheck;
    QPoint startPoint;
    QPoint endPoint;
    QRubberBand *cropBand;

    QString fileName;
    QPixmap *pix;

    QString currentGrade;
    int currentGradeIndex;

    QString getDirByIndex(int index);

    QRect lastWindowSize;
    QTimer *resizeTimer;

    void showCategory(int gCatIndex = 0);

    bool imageSelected;
    int itemID;

private slots:
    void on_pushButtonBrowse_clicked();

    void on_pushButtonAdd_clicked();

    void on_comboBoxGradeCat_currentIndexChanged(int index);

    void timerResizeEvent();

private:
    Ui::addlesson *ui;
};

#endif // ADDLESSON_H
