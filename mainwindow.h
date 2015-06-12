#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addlesson.h"
#include "addquestion.h"
#include "setting.h"
#include "about.h"

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QTreeWidgetItem>

#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <QDir>

const QString DEFAULT_DIR_KEY("default_dir");
const QString PLAY_MUSIC("play_music");
const QString MUSIC_LOOP("music_loop");
const QString QUESTION_MODE("question_mode");
const QString QUESTION_NUMBER("question_number");
const QString QUESTION_TIME("question_time");
const QString RANDOM_QUESTION("random_question");
const QString settingAddress(QDir::currentPath()+"/data/setting.ini");

const QString QUESTION_LIST("question_list");
const QString PROGRAM_MODE("program_mode");
const QString GRADE_CATEGORY("grade_categery");
const QString LESSON_CATEGORY("lesson_category");
const QString LAST_PAGE("last_page");

const QString LAST_STATE("last_state");

struct kkConfig {
    int qMode, qNumb, qTime;
    bool playMusic, musicLoop, randomQ;
    QString questionList;
    int khMode, gCat, lastPage;
    QString lessonCat;
    bool lastState;
};

struct kkApp {
    int cQTimer, addQTimer;
    int cPage, cGCatIndex, cMode, pageCount;
    QString cCat;
    bool adminToggle;
};

struct darsname {
    int id, gCatIndex;
    QString cat, gCat, picDir;
};

struct question {
    int id, gCatIndex;
    QString cat, picDir, numbPic, solDir, numbSol;
    bool isAnswered, toPrint;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void geometryIni(int wOffset = 0);
    void setViewFrame();
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent * event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void closeEvent(QCloseEvent *);

    QRect lastWindowSize;
    QTimer *resizeTimer;

    //DB
    QSqlDatabase myDB;
    void adminSettingShow(bool show);
    //category
    double catRatio;
    bool vResizeActive;
    bool catToogle;

    //media player
    QMediaPlayer *mediaPlayer;
    QMediaPlaylist *playList;

    //darsname
    void showDarsname(int gCatIndex, QString cat = "", bool startUp = false);
    void showDarsnamePage(int page);
    QList<darsname> darsnameList;
    //question
    void showQuestion(int qCatIndex, int gCatIndex, QString cat = "", QString randList = "", bool startUp = false);
    void showQuestionPage(int page, bool dontDisturb = false);
    QList<question> questionList;

    QPixmap* pixMap1;
    QGraphicsPixmapItem* item1;
    QPixmap* pixMap2;
    QGraphicsPixmapItem* item2;
    void treeAddChild(QTreeWidget *obj, QString name);

    static int itemId;
    void drawNumb(QPixmap &pixIn, QString rect ,int numb);

    //graphicsView
    double viewRatio;
    bool viewResizeActive;

    QGraphicsBlurEffect *blurEffect;
    void showSolution(int id = -1);
    void showAllSolutions();

    void iniConfigs(bool loadDef = true);

    kkConfig myConfig;
    kkApp myApp;

    QList<int> lastRandList;

    QList<question> scrambleList(QList<question> list, int max);
    QList<question> scrambleList(QList<question> list, int max, QList<int> randomList);
    QList<question> sortByRandomOrder(QList<question> list, QList<int> randomList);

    QTimer *questionTimer;

    bool checkHaveToPrint();

    ~MainWindow();
public slots:
    void addNewLesson();
    void changeMusicBar(qint64);
    void deleteItem();
    void changeLesson();
    void timerResizeEvent();
    void addNewQuestion();
    void showSetting();
    void settingExits();
    void qTimerSlot();
    void printQusetions();
    void showAbout();
private slots:
    void on_pushButtonCatToggle_clicked();

    void on_pushButtonLeftEnd_clicked();

    void on_pushButtonLeft_clicked();

    void on_pushButtonRight_clicked();

    void on_pushButtonRightEnd_clicked();

    void on_musicBar_actionTriggered(int action);

    void on_pushButtonPrev_clicked();

    void on_pushButtonStop_clicked();

    void on_pushButtonPlay_clicked();

    void on_pushButtonNext_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidgetcat_itemClicked(QTreeWidgetItem *item, int column);

    void on_lineEditPage_returnPressed();

    void on_pushButtonShowSol_clicked();

    void on_checkBoxPrintSelect_clicked();

private:
    addlesson *addlessonDialog;
    addquestion *addquestionDialog;
    setting *settingDialog;
    about *aboutDialog;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
