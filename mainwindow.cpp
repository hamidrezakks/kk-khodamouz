#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addlesson.h"
#include "addquestion.h"
#include "setting.h"
#include "about.h"
#include <QDebug>
#include <QResizeEvent>
#include <QDialog>
#include <QWheelEvent>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDir>
#include <QMessageBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QTime>
#include <QTimer>

#include <QTreeWidgetItem>
#include <QList>

#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QGraphicsEffect>

#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->menu->actions().at(0),SIGNAL(triggered()),this,SLOT(addNewLesson()));
    connect(ui->menu->actions().at(1),SIGNAL(triggered()),this,SLOT(addNewQuestion()));
    connect(ui->menu->actions().at(2),SIGNAL(triggered()),this,SLOT(changeLesson()));
    connect(ui->menu->actions().at(3),SIGNAL(triggered()),this,SLOT(deleteItem()));
    connect(ui->menu->actions().at(5),SIGNAL(triggered()),this,SLOT(printQusetions()));
    connect(ui->menu_2->actions().at(0),SIGNAL(triggered()),this,SLOT(showSetting()));
    connect(ui->menu_3->actions().at(0),SIGNAL(triggered()),this,SLOT(showAbout()));

    questionTimer = new QTimer();

    connect(questionTimer,SIGNAL(timeout()),this,SLOT(qTimerSlot()));
    questionTimer->setInterval(1000);

    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName(QDir::currentPath()+"/data/db.db3");
    if (!myDB.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
        qApp->tr("Unable to establish a database connection.\n"
              "This example needs SQLite support. Please read "
              "the Qt SQL driver documentation for information how "
              "to build it.\n\n"
              "Click Cancel to exit."), QMessageBox::Cancel);
    }

    if(!myDB.tables().contains("darsname"))
    {
        QSqlQuery squery;
        squery.exec("CREATE TABLE darsname (id int PRIMARY KEY, grade_category varchar(128), grade_index int, lesson_category varchar(128), pic_dir varchar(128), date varchar(32), state int )");
    }

    if(!myDB.tables().contains("qtesti"))
    {
        QSqlQuery squery;
        squery.exec("CREATE TABLE qtesti (id int PRIMARY KEY, grade_index int, lesson_category varchar(128), pic_dir varchar(128) , numb_border varchar(32) , picsol_dir varchar(128) , numbsol_border varchar(32), date varchar(32), state int )");
    }

    if(!myDB.tables().contains("qtashrihi"))
    {
        QSqlQuery squery;
        squery.exec("CREATE TABLE qtashrihi (id int PRIMARY KEY, grade_index int, lesson_category varchar(128), pic_dir varchar(128) , numb_border varchar(32) , picsol_dir varchar(128) , numbsol_border varchar(32), date varchar(32), state int )");
    }

    if(!myDB.tables().contains("qsakht"))
    {
        QSqlQuery squery;
        squery.exec("CREATE TABLE qsakht (id int PRIMARY KEY, grade_index int, lesson_category varchar(128), pic_dir varchar(128) , numb_border varchar(32) , picsol_dir varchar(128) , numbsol_border varchar(32), date varchar(32), state int )");
    }

    if(!myDB.tables().contains("qhoosh"))
    {
        QSqlQuery squery;
        squery.exec("CREATE TABLE qhoosh (id int PRIMARY KEY, lesson_category varchar(128), pic_dir varchar(128) , numb_border varchar(32) , picsol_dir varchar(128) , numbsol_border varchar(32), date varchar(32), state int )");
    }

    myApp.adminToggle = false;
    catRatio = 0.5;
    catToogle = true;
    vResizeActive = false;

    viewResizeActive = false;
    viewRatio = 0.5;


    ui->pushButtonLeftEnd->setStyleSheet("QPushButton{border-image:url(image/leftendw.png);}"
                                  "QPushButton::pressed{border-image:url(image/leftendb.png);}");
    ui->pushButtonLeft->setStyleSheet("QPushButton{border-image:url(image/leftw.png);}"
                                  "QPushButton::pressed{border-image:url(image/leftb.png);}");
    ui->pushButtonRight->setStyleSheet("QPushButton{border-image:url(image/rightw.png);}"
                                  "QPushButton::pressed{border-image:url(image/rightb.png);}");
    ui->pushButtonRightEnd->setStyleSheet("QPushButton{border-image:url(image/rightendw.png);}"
                                  "QPushButton::pressed{border-image:url(image/rightendb.png);}");

    ui->pushButtonShowSol->setStyleSheet("QPushButton{border-image:url(image/solw.png);}"
                                  "QPushButton::hover{border-image:url(image/soly.png);}");

    ui->pushButtonPrev->setStyleSheet("border-image:url(image/media-prev.png);");
    ui->pushButtonStop->setStyleSheet("border-image:url(image/media-stop.png);");
    ui->pushButtonPlay->setStyleSheet("border-image:url(image/media-play.png);");
    ui->pushButtonNext->setStyleSheet("border-image:url(image/media-next.png);");

    ui->frameMusic->setStyleSheet("background-color: #333;");

    //media player

    playList = new QMediaPlaylist();

    QStringList filters;
    filters << "*.mp3" << "*.wav" << "*.wave" << "*.mp4" << "*.m4a";

    QDir path(QDir::currentPath()+"/music");
    QStringList mediaFiles = path.entryList(filters);

    for(int i=0 ; i < mediaFiles.count(); i++)
        playList->addMedia(QUrl::fromLocalFile(QDir::currentPath()+"/music/"+mediaFiles.at(i)));

    mediaPlayer = new QMediaPlayer();
    mediaPlayer->setPlaylist(playList);

    connect(mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(changeMusicBar(qint64)));
    //playList->setCurrentIndex(1);
    mediaPlayer->setVolume(100);
    mediaPlayer->playlist()->setPlaybackMode(QMediaPlaylist::Loop);

    ui->lcdNumber->display("00:00");

    ui->lcdNumberTimer->display("00:00");
    ui->lcdNumberTimer->setVisible(false);
    ui->lcdNumberTimer->setToolTip("توقف");


    //end of media player

    myApp.cPage = 0;
    myApp.cMode = -1;
    myApp.cCat = "";
    myApp.cGCatIndex = 0;
    myApp.cQTimer = 0;
    myApp.addQTimer = 0;

    pixMap1 = new QPixmap(10,10);
    item1 = new QGraphicsPixmapItem(*pixMap1);

    pixMap2 = new QPixmap(500,200);
    item2 = new QGraphicsPixmapItem(*pixMap2);

    lastWindowSize = this->rect();
    resizeTimer = new QTimer(this);
    resizeTimer->setInterval(500);
    connect(resizeTimer,SIGNAL(timeout()),this,SLOT(timerResizeEvent()));

    blurEffect = new QGraphicsBlurEffect();
    blurEffect->setBlurRadius(25);
    ui->graphicsViewSol->setGraphicsEffect(blurEffect);

    lastRandList.clear();
    iniConfigs();

    adminSettingShow(false);
}

void MainWindow::setViewFrame()
{
    int sfWidth = ui->frameSlide->width(), sfHeight = ui->frameSlide->height();
    QRect graphicViewRect, graphicViewSolRect, lineFrameRect, showSolRect, cbpsRect;
    cbpsRect.setRect(5,5,20,20);
    if(myApp.cMode == 4 || myApp.cMode == -1)
    {
        graphicViewRect.setRect(0,0,sfWidth,sfHeight);
        graphicViewSolRect.setRect(0,sfHeight*viewRatio+2,sfWidth,sfHeight*(1.0-viewRatio));
        ui->graphicsView->setGeometry(graphicViewRect);
        ui->graphicsViewSol->setGeometry(graphicViewSolRect);
        ui->graphicsViewSol->setVisible(false);
        ui->lineFrame->setVisible(false);
        ui->pushButtonShowSol->setVisible(false);
        ui->checkBoxPrintSelect->setGeometry(cbpsRect);
        ui->checkBoxPrintSelect->setVisible(false);
    }
    else
    {
        sfHeight = sfHeight - 2;
        graphicViewRect.setRect(0,0,sfWidth,sfHeight*viewRatio);
        graphicViewSolRect.setRect(0,sfHeight*viewRatio+2,sfWidth,sfHeight*(1.0-viewRatio));
        lineFrameRect.setRect(0,sfHeight*viewRatio-1,sfWidth,4);
        showSolRect.setRect(10,sfHeight*viewRatio+12,68,50);
        ui->graphicsView->setGeometry(graphicViewRect);
        ui->graphicsViewSol->setVisible(true);
        ui->graphicsViewSol->setGeometry(graphicViewSolRect);
        ui->lineFrame->setVisible(true);
        ui->lineFrame->setGeometry(lineFrameRect);
        ui->pushButtonShowSol->setGeometry(showSolRect);
        ui->checkBoxPrintSelect->setGeometry(cbpsRect);
        ui->checkBoxPrintSelect->setVisible(true);
    }
}

void MainWindow::iniConfigs(bool loadDef)
{
    QSettings settings(settingAddress, QSettings::IniFormat);
    myConfig.playMusic = settings.value(PLAY_MUSIC, "").toBool();
    myConfig.musicLoop = settings.value(MUSIC_LOOP, "").toBool();
    myConfig.qTime = settings.value(QUESTION_TIME, "").toInt();
    myConfig.qMode = settings.value(QUESTION_MODE, "").toInt();
    myConfig.randomQ = settings.value(RANDOM_QUESTION, "").toBool();
    myConfig.qNumb = settings.value(QUESTION_NUMBER, "").toInt();

    myConfig.questionList = settings.value(QUESTION_LIST, "").toString();
    myConfig.khMode = settings.value(PROGRAM_MODE, "").toInt();
    myConfig.gCat = settings.value(GRADE_CATEGORY, "").toInt();
    myConfig.lessonCat = settings.value(LESSON_CATEGORY, "").toString();
    myConfig.lastPage = settings.value(LAST_PAGE, "").toInt();

    myConfig.lastState = settings.value(LAST_STATE, "").toBool();

    if(myConfig.musicLoop)
        mediaPlayer->playlist()->setPlaybackMode(QMediaPlaylist::Loop);
    else
        mediaPlayer->playlist()->setPlaybackMode(QMediaPlaylist::Sequential);

    if(myConfig.khMode != 4 && !loadDef)
    {
        showQuestion(myConfig.khMode, myConfig.gCat, myConfig.lessonCat);
        showQuestionPage(myConfig.lastPage, true);
    }
    if(myConfig.khMode == -1 || !loadDef || !myConfig.lastState)
        return;
    if(myConfig.khMode == 4)
    {
        showDarsname(myConfig.gCat, myConfig.lessonCat, true);
        showDarsnamePage(myConfig.lastPage);
    }
    else
    {
        showQuestion(myConfig.khMode, myConfig.gCat, myConfig.lessonCat, myConfig.questionList, true);
        showQuestionPage(myConfig.lastPage, true);
    }
}

void MainWindow::geometryIni(int wOffset)
{
    int height = ui->centralWidget->height()-30, width = ui->centralWidget->width() + wOffset;
    QRect categoryRect, catSplitLine, category2Rect, slideFrameRect;
    QRect sBtnContRect, catToggleRect;
    int heightB = height - 6;
    categoryRect.setRect(width-202,2,200,heightB*catRatio-1);
    catSplitLine.setRect(width-202,heightB*catRatio,200,3);
    category2Rect.setRect(width-202,heightB*catRatio+3,200,heightB*(1.0-catRatio)+2);
    slideFrameRect.setRect(2,2,width-211,height-80);
    sBtnContRect.setRect(2,height-78,width-211,76);
    catToggleRect.setRect(width-210,heightB*catRatio+2,10,29);
    ui->treeWidget->setGeometry(categoryRect);
    ui->line->setGeometry(catSplitLine);
    ui->treeWidgetcat->setGeometry(category2Rect);
    ui->frameSlide->setGeometry(slideFrameRect);
    ui->frame->setGeometry(sBtnContRect);
    ui->pushButtonCatToggle->setGeometry(catToggleRect);

    setViewFrame();

    int frameHeight = ui->frame->height(), frameWidth = ui->frame->width();
    QRect rRect, rEndRect, lRect, lEndRect, pageRect, ofPageRect, timerRect;
    int sfw = (frameWidth-290)/2, sfh = (frameHeight-40)/2;
    lEndRect.setRect(sfw,sfh,40,40);
    lRect.setRect(sfw+50,sfh,40,40);
    pageRect.setRect(sfw+100,sfh,40,40);
    ofPageRect.setRect(sfw+141,sfh,50,40);
    rRect.setRect(sfw+200,sfh,40,40);
    rEndRect.setRect(sfw+250,sfh,40,40);
    timerRect.setRect(40,sfh,60,40);

    ui->pushButtonLeftEnd->setGeometry(lEndRect);
    ui->pushButtonLeft->setGeometry(lRect);
    ui->lineEditPage->setGeometry(pageRect);
    ui->labelOfPage->setGeometry(ofPageRect);
    ui->pushButtonRight->setGeometry(rRect);
    ui->pushButtonRightEnd->setGeometry(rEndRect);
    ui->lcdNumberTimer->setGeometry(timerRect);

    QRect playerFrameRect;
    playerFrameRect.setRect(0,height-2,width,32);
    ui->frameMusic->setGeometry(playerFrameRect);

    int mHeight = ui->frameMusic->height(), mWidth = ui->frameMusic->width();
    QRect prevRect, stopRect, playRect, nextRect, musicBarRect, lcdRect;
    int mfh = (mHeight-26)/2;
    prevRect.setRect(20,mfh,26,26);
    stopRect.setRect(56,mfh,26,26);
    playRect.setRect(92,mfh,26,26);
    nextRect.setRect(128,mfh,26,26);
    musicBarRect.setRect(174,mfh+5,mWidth-244,16);
    lcdRect.setRect(mWidth-60,mfh,50,26);

    ui->pushButtonPrev->setGeometry(prevRect);
    ui->pushButtonStop->setGeometry(stopRect);
    ui->pushButtonPlay->setGeometry(playRect);
    ui->pushButtonNext->setGeometry(nextRect);
    ui->musicBar->setGeometry(musicBarRect);
    ui->lcdNumber->setGeometry(lcdRect);

    if(!resizeTimer->isActive())
        resizeTimer->start();
}

void MainWindow::timerResizeEvent()
{

    if(lastWindowSize == this->rect())
    {
        //mode0 = darsname
        if(myApp.pageCount > 0)
        {
            if(myApp.cMode == 4)
                showDarsnamePage(myApp.cPage);
            else
                showQuestionPage(myApp.cPage);
        }
        resizeTimer->stop();
    }

    lastWindowSize = this->rect();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    QSettings settings(settingAddress, QSettings::IniFormat);

    QString str = "";
    if(lastRandList.count() > 0)
    {
        for(int i = 0; i < lastRandList.count(); i++)
        {
            if(i == lastRandList.count()-1)
                str += QString::number(lastRandList.at(i));
            else
                str += QString::number(lastRandList.at(i))+"|";
        }
    }
    settings.setValue(QUESTION_LIST, str);
    settings.setValue(PROGRAM_MODE, myApp.cMode);
    settings.setValue(GRADE_CATEGORY, myApp.cGCatIndex);
    settings.setValue(LESSON_CATEGORY, myApp.cCat);
    settings.setValue(LAST_PAGE, myApp.cPage);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        widget->close();
       }
}

void MainWindow::treeAddChild(QTreeWidget *obj, QString name)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(obj);
    treeItem->setText(0, name);
}

void MainWindow::showDarsname(int gCatIndex, QString cat, bool startUp)
{
    if(mediaPlayer->state() == QMediaPlayer::PlayingState)
        on_pushButtonPlay_clicked();
    myApp.cQTimer = 0;
    ui->lcdNumberTimer->display("00:00");
    questionTimer->stop();
    ui->lcdNumberTimer->setVisible(false);

    darsnameList.clear();
    QSqlQuery query;

    if(cat == "" || startUp)
    {
        query.exec("SELECT lesson_category FROM darsname WHERE grade_index = "+QString::number(gCatIndex)+" AND state = 1 ORDER BY lesson_category ASC");
        QString category = "";
        QList<QString> catList;
        while(query.next())
        {
            if(query.value(0).toString().compare(category) != 0)
            {
                category = query.value(0).toString();
                catList.append(category);
            }
        }
        ui->treeWidgetcat->clear();
        treeAddChild(ui->treeWidgetcat,"همه");
        for(int i=0; i < catList.count(); i++)
        {
            treeAddChild(ui->treeWidgetcat,catList.at(i));
        }
    }

    if(cat == "" || cat == "همه")
        query.exec("SELECT id, grade_category, grade_index, lesson_category, pic_dir  FROM darsname WHERE grade_index = "+QString::number(gCatIndex)+" AND state = 1 ORDER BY id ASC");
    else
        query.exec("SELECT id, grade_category, grade_index, lesson_category, pic_dir  FROM darsname WHERE grade_index = "+QString::number(gCatIndex)+" AND lesson_category = '"+cat+"' AND state = 1 ORDER BY id ASC");
    while(query.next())
    {
        darsname list;
        list.id = query.value(0).toInt();
        list.gCat = query.value(1).toString();
        list.gCatIndex = query.value(2).toInt();
        list.cat = query.value(3).toString();
        list.picDir = query.value(4).toString();

        darsnameList.append(list);
    }

    myApp.pageCount = darsnameList.count();
    ui->labelOfPage->setText("/"+QString::number(myApp.pageCount));
    myApp.cGCatIndex = gCatIndex;
    myApp.cMode = 4;
    myApp.cCat = cat;

    if(myApp.pageCount > 0)
    {
        ui->frameSlide->show();
        showDarsnamePage(1);
    }
    else
    {
        ui->frameSlide->hide();
        ui->lineEditPage->setText("0");
    }
    setViewFrame();
}

void MainWindow::showDarsnamePage(int page)
{
    page = page - 1;
    if( page < 0 ) page = 0;
    if( page > myApp.pageCount - 1) page = myApp.pageCount - 1;

    if(!QFile::exists(QDir::currentPath()+"/data/darsname/"+darsnameList.at(page).picDir))
    {
        QMessageBox::warning(0, qApp->tr("خطا"),
        qApp->tr("فایل مورد نظر موجود نیست!")
                            , "تایید");
        return;
    }
    pixMap1->load(QDir::currentPath()+"/data/darsname/"+darsnameList.at(page).picDir);
    //scene1 = new QGraphicsScene();
    item1->setPixmap(*pixMap1);
    QGraphicsScene* sceneTemp1 = new QGraphicsScene();
    sceneTemp1->addItem(item1);
    ui->graphicsView->setScene(sceneTemp1);
    ui->graphicsView->fitInView(item1,Qt::KeepAspectRatio);
    ui->graphicsView->show();

    myApp.cPage = page + 1;
    ui->lineEditPage->setText(QString::number(myApp.cPage));
    //delete sceneTemp1;
}

void MainWindow::showQuestion(int qCatIndex, int gCatIndex, QString cat, QString randList, bool startUp)
{
    myApp.cQTimer = 0;
    myApp.addQTimer = 0;
    ui->lcdNumberTimer->setVisible(true);
    ui->lcdNumberTimer->display("00:00");
    if(myConfig.playMusic)
        if(mediaPlayer->state() != QMediaPlayer::PlayingState)
            on_pushButtonPlay_clicked();
    questionList.clear();
    QSqlQuery query;

    if((cat == "" || startUp )&& randList == "")
    {
        if(qCatIndex == 0)
            query.exec("SELECT lesson_category FROM qtesti WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
        else if(qCatIndex == 1)
            query.exec("SELECT lesson_category FROM qtashrihi WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
        else if(qCatIndex == 2)
            query.exec("SELECT lesson_category FROM qsakht WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
        else if(qCatIndex == 3)
            query.exec("SELECT lesson_category FROM qhoosh WHERE state = 1 ORDER BY lesson_category ASC");
        QString category = "";
        QList<QString> catList;
        while(query.next())
        {
            if(query.value(0).toString().compare(category) != 0)
            {
                category = query.value(0).toString();
                catList.append(category);
            }
        }
        ui->treeWidgetcat->clear();
        treeAddChild(ui->treeWidgetcat,"همه");
        for(int i=0; i < catList.count(); i++)
        {
            treeAddChild(ui->treeWidgetcat,catList.at(i));
        }
    }

    if(randList != "")
    {
        if(qCatIndex == 0)
            query.exec("SELECT lesson_category FROM qtesti WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
        else if(qCatIndex == 1)
            query.exec("SELECT lesson_category FROM qtashrihi WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
        else if(qCatIndex == 2)
            query.exec("SELECT lesson_category FROM qsakht WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
        else if(qCatIndex == 3)
            query.exec("SELECT lesson_category FROM qhoosh WHERE state = 1 ORDER BY lesson_category ASC");
        QString category = "";
        QList<QString> catList;
        while(query.next())
        {
            if(query.value(0).toString().compare(category) != 0)
            {
                category = query.value(0).toString();
                catList.append(category);
            }
        }
        ui->treeWidgetcat->clear();
        treeAddChild(ui->treeWidgetcat,"همه");
        for(int i=0; i < catList.count(); i++)
        {
            treeAddChild(ui->treeWidgetcat,catList.at(i));
        }

        QList<int> tempList;
        QStringList tempIDs = randList.split("|");
        for(int i = 0; i < tempIDs.count(); i++)
            tempList.append(tempIDs.at(i).toInt());
        QString str3 = "";
        for(int i =  0; i < tempIDs.count(); i++ )
        {
            if(i == tempIDs.count() - 1)
                str3 += " id = "+QString::number(tempList.at(i))+" ";
            else
                str3 += " id = "+QString::number(tempList.at(i))+" OR ";
            lastRandList.append(tempIDs.at(i).toInt());
        }

        if(qCatIndex == 0)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qtesti WHERE "+str3+" AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 1)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qtashrihi WHERE "+str3+" AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 2)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qsakht WHERE "+str3+" AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 3)
            query.exec("SELECT id, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qhoosh WHERE "+str3+" AND state = 1 ORDER BY id ASC");
    }
    else if(cat == "" || cat == "همه")
    {
        if(qCatIndex == 0)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qtesti WHERE grade_index = "+QString::number(gCatIndex)+" AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 1)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qtashrihi WHERE grade_index = "+QString::number(gCatIndex)+" AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 2)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qsakht WHERE grade_index = "+QString::number(gCatIndex)+" AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 3)
            query.exec("SELECT id, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qhoosh WHERE state = 1 ORDER BY id ASC");
    }
    else
    {
        if(qCatIndex == 0)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qtesti WHERE grade_index = "+QString::number(gCatIndex)+" AND lesson_category = '"+cat+"' AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 1)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qtashrihi WHERE grade_index = "+QString::number(gCatIndex)+" AND lesson_category = '"+cat+"' AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 2)
            query.exec("SELECT id, grade_index, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qsakht WHERE grade_index = "+QString::number(gCatIndex)+" AND lesson_category = '"+cat+"' AND state = 1 ORDER BY id ASC");
        else if(qCatIndex == 3)
            query.exec("SELECT id, lesson_category, pic_dir, numb_border, picsol_dir, numbsol_border  FROM qhoosh WHERE lesson_category = '"+cat+"' AND state = 1 ORDER BY id ASC");
    }
    while(query.next())
    {
        question list;
        if(qCatIndex != 3)
        {
            list.id = query.value(0).toInt();
            list.gCatIndex = query.value(1).toInt();
            list.cat = query.value(2).toString();
            list.picDir = query.value(3).toString();
            list.numbPic = query.value(4).toString();
            list.solDir = query.value(5).toString();
            list.numbSol = query.value(6).toString();
            list.isAnswered = false;
            list.toPrint = false;
        }
        else
        {
            list.id = query.value(0).toInt();
            //list.gCatIndex = query.value(1).toInt();
            list.cat = query.value(1).toString();
            list.picDir = query.value(2).toString();
            list.numbPic = query.value(3).toString();
            list.solDir = query.value(4).toString();
            list.numbSol = query.value(5).toString();
            list.isAnswered = false;
            list.toPrint = false;
        }
        questionList.append(list);
    }

    if(myConfig.randomQ && randList == "")
        questionList = scrambleList(questionList,myConfig.qNumb);

    if(randList != "")
        questionList = sortByRandomOrder(questionList, lastRandList);

    myApp.pageCount = questionList.count();
    ui->labelOfPage->setText("/"+QString::number(myApp.pageCount));
    myApp.cGCatIndex = gCatIndex;
    myApp.cMode = qCatIndex;
    myApp.cCat = cat;

    if(myApp.pageCount > 0)
    {
        ui->frameSlide->show();
        showQuestionPage(1);
    }
    else
    {
        ui->frameSlide->hide();
        ui->lineEditPage->setText("0");
    }
    setViewFrame();
}

void MainWindow::showQuestionPage(int page, bool dontDisturb)
{
    if(questionList.count()<=0)
        return;
    if(myConfig.qMode == 2)
    {
        if(!dontDisturb)
        {
            bool checkB = false;
            int pageB = page - 1, currentPageB = myApp.cPage;
            if( pageB < 0 ) pageB = 0;
            if( pageB > myApp.pageCount - 1) pageB = myApp.pageCount - 1;
            if( myApp.cPage > myApp.pageCount) currentPageB = myApp.pageCount;
            if(myApp.cPage != page && myApp.cPage != 0  && !questionList.at(currentPageB-1).isAnswered)
            {
                questionTimer->stop();
                checkB = QMessageBox::warning(0, qApp->tr("ادامه؟"),
                qApp->tr("سوال را رد می کنید؟")
                                    ,"خیر"
                                    ,"بله");
                questionTimer->start();
                if(!checkB)
                    return;
                else
                {
                    myApp.addQTimer = 0;
                    myApp.cQTimer = 0;
                    ui->lcdNumberTimer->display("00:00");
                }
            }
            else
            {
                myApp.addQTimer = 0;
                myApp.cQTimer = 0;
                ui->lcdNumberTimer->display("00:00");
                questionTimer->start();
            }
        }
        ui->lcdNumberTimer->setVisible(true);
    }
    else if(myConfig.qMode == 3)
    {
        questionTimer->start();
        ui->lcdNumberTimer->setVisible(true);
    }
    else
        ui->lcdNumberTimer->setVisible(false);

    page = page - 1;
    if( page < 0 ) page = 0;
    if( page > myApp.pageCount - 1) page = myApp.pageCount - 1;

    if(myConfig.playMusic && !questionList.at(page).isAnswered)
        if(mediaPlayer->state() == QMediaPlayer::PausedState)
            on_pushButtonPlay_clicked();


    if(!QFile::exists(QDir::currentPath()+"/data/"+questionList.at(page).picDir))
    {
        QMessageBox::warning(0, qApp->tr("خطا"),
        qApp->tr("فایل مورد نظر موجود نیست!")
                            , "تایید");
        return;
    }
    pixMap1->load(QDir::currentPath()+"/data/"+questionList.at(page).picDir);
    //scene1 = new QGraphicsScene();
    drawNumb(*pixMap1,questionList.at(page).numbPic,page+1);
    item1->setPixmap(*pixMap1);
    QGraphicsScene* sceneTemp1 = new QGraphicsScene();
    sceneTemp1->addItem(item1);
    /*QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
    effect->setBlurRadius(25);
    ui->graphicsView->setGraphicsEffect(effect);*/
    ui->graphicsView->setScene(sceneTemp1);
    ui->graphicsView->fitInView(item1,Qt::KeepAspectRatio);
    ui->graphicsView->show();

    pixMap2->load(QDir::currentPath()+"/data/"+questionList.at(page).solDir);
    drawNumb(*pixMap2,questionList.at(page).numbSol,page+1);
    item2->setPixmap(*pixMap2);
    QGraphicsScene* sceneTemp2 = new QGraphicsScene();
    sceneTemp2->addItem(item2);
    blurEffect->setBlurRadius(ui->frameSlide->width()/32);
    if(questionList.at(page).isAnswered)
    {
        blurEffect->setEnabled(false);
        ui->pushButtonShowSol->setVisible(false);
    }
    else
    {
        blurEffect->setEnabled(true);
        ui->pushButtonShowSol->setVisible(true);
    }
    ui->graphicsViewSol->setScene(sceneTemp2);
    ui->graphicsViewSol->show();
    ui->graphicsViewSol->fitInView(item2,Qt::KeepAspectRatio);
    ui->graphicsViewSol->show();

    ui->checkBoxPrintSelect->setChecked(questionList.at(page).toPrint);

    myApp.cPage = page + 1;
    ui->lineEditPage->setText(QString::number(myApp.cPage));
    //delete sceneTemp1;
}

void MainWindow::drawNumb(QPixmap &pixIn, QString rect, int numb)
{
    QRect tRect;
    QStringList tempList = rect.split("|");
    tRect.setX(tempList.at(0).toInt());
    tRect.setY(tempList.at(1).toInt());
    tRect.setWidth(tempList.at(2).toInt());
    tRect.setHeight(tempList.at(3).toInt());

    QPainter tempPainter(&pixIn);
    tempPainter.setBrush(QBrush(Qt::white));
    tempPainter.setPen(QColor(Qt::white));
    tempPainter.drawRect(tRect);
    tempPainter.setPen(QColor(Qt::black));
    QTextOption qto;
    qto.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    tempPainter.setFont( QFont("B Titr",-1,QFont::Black));
    QFont f = tempPainter.font();
    f.setPointSizeF(pixIn.width()*0.011);
    tempPainter.setFont(f);
    /*float factor = tRect.height() / tempPainter.fontMetrics().height();
    if ((factor < 1) || (factor > 1.25))
    {
        QFont f = tempPainter.font();
        f.setPointSizeF(f.pointSizeF()*factor);
        tempPainter.setFont(f);
        qDebug()<<f.pointSizeF()<<pixIn.width();
    }*/
    tempPainter.drawText(tRect,"-"+QString::number(numb),qto);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    if(catToogle)
        geometryIni();
    else
        geometryIni(202);
}

void QGraphicsView::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y() > 0)
        this->scale(2,2);
    else
        this->scale(0.5,0.5);
}


void MainWindow::mousePressEvent(QMouseEvent *)
{
    if(ui->line->underMouse())
    {
        vResizeActive = true;
    }
    if(ui->lineFrame->underMouse() && (myApp.cMode != -1 && myApp.cMode != 4))
    {
        viewResizeActive = true;
    }
    if(ui->lcdNumberTimer->underMouse() && myApp.cMode != 4)
    {
        if(myConfig.qMode != 1)
        {
            if(questionTimer->isActive())
            {
                questionTimer->stop();
                ui->lcdNumberTimer->setToolTip("شروع");
            }
            else
            {
                questionTimer->start();
                ui->lcdNumberTimer->setToolTip("توقف");
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(vResizeActive)
    {
        QRect lineRect;
        QRect cat1Rect, cat2Rect;
        QPoint tempPoint = ui->centralWidget->mapFromParent(event->pos());
        int height = ui->centralWidget->height()-30;

        if(tempPoint.y() < 30)
            tempPoint.setY(30);
        if(height - tempPoint.y() < 30)
            tempPoint.setY(height - 30);
        lineRect = ui->line->geometry();
        lineRect.setY(tempPoint.y());
        lineRect.setHeight(3);
        cat1Rect = ui->treeWidget->geometry();
        cat1Rect.setBottom(tempPoint.y());
        cat2Rect = ui->treeWidgetcat->geometry();
        cat2Rect.setY(tempPoint.y()+3);
        cat2Rect.setBottom(height-3);
        ui->line->setGeometry(lineRect);
        ui->treeWidget->setGeometry(cat1Rect);
        ui->treeWidgetcat->setGeometry(cat2Rect);
        catRatio = (double)tempPoint.y()/(height-6);

        QRect catToggleRect = ui->pushButtonCatToggle->geometry();
        catToggleRect.setY((height-6)*catRatio+2);
        catToggleRect.setHeight(29);
        ui->pushButtonCatToggle->setGeometry(catToggleRect);
    }
    if(viewResizeActive)
    {
        QPoint tempPoint = ui->centralWidget->mapFromParent(event->pos());
        int height = ui->frameSlide->height();
        if(tempPoint.y() < height*0.2)
            tempPoint.setY(height*0.2);
        if(height - tempPoint.y() < height*0.2)
            tempPoint.setY(height*0.8);
        viewRatio = (double)tempPoint.y()/(height-2);
        setViewFrame();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    vResizeActive = false;
    viewResizeActive = false;
}

void MainWindow::adminSettingShow(bool show)
{
    ui->menu->actions().at(0)->setVisible(show);
    ui->menu->actions().at(1)->setVisible(show);
    ui->menu->actions().at(2)->setVisible(show);
    ui->menu->actions().at(3)->setVisible(show);
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    bool isSHIFT = keyMod.testFlag(Qt::ShiftModifier);
    bool isCTRL = keyMod.testFlag(Qt::ControlModifier);

    if (isSHIFT && isCTRL)
    {
        if(event->key() == Qt::Key_F9)
        {
            myApp.adminToggle = myApp.adminToggle? false : true;
            adminSettingShow(myApp.adminToggle);
        }
    }
}

void MainWindow::addNewLesson()
{
    addlessonDialog = new addlesson();
    addlessonDialog->setWindowFlags( Qt::CustomizeWindowHint
                                     | Qt::WindowCloseButtonHint
                                     | Qt::WindowMaximizeButtonHint );
    addlessonDialog->show();
}

void MainWindow::addNewQuestion()
{
    addquestionDialog = new addquestion();
    addquestionDialog->setWindowFlags(Qt::CustomizeWindowHint
                                      | Qt::WindowCloseButtonHint
                                      | Qt::WindowMaximizeButtonHint );
    addquestionDialog->show();
}

void MainWindow::deleteItem()
{
    if(myApp.pageCount > 0)
    {
        bool check = QMessageBox::question(0, qApp->tr("حذف آیتم"),
                                       qApp->tr("مطمئن هستید؟")
                                                ,"خیر","بله");
        if(check)
        {
            bool check2;
            QSqlQuery query;
            if(myApp.cMode == 4)
                check2 = query.exec("UPDATE `darsname` SET `state`=0 WHERE id="+QString::number(darsnameList.at(myApp.cPage-1).id)+";");
            else if(myApp.cMode == 0)
                check2 = query.exec("UPDATE `qtesti` SET `state`=0 WHERE id="+QString::number(questionList.at(myApp.cPage-1).id)+";");
            else if(myApp.cMode == 1)
                check2 = query.exec("UPDATE `qtashrihi` SET `state`=0 WHERE id="+QString::number(questionList.at(myApp.cPage-1).id)+";");
            else if(myApp.cMode == 2)
                check2 = query.exec("UPDATE `qsakht` SET `state`=0 WHERE id="+QString::number(questionList.at(myApp.cPage-1).id)+";");
            else if(myApp.cMode == 3)
                check2 = query.exec("UPDATE `qhoosh` SET `state`=0 WHERE id="+QString::number(questionList.at(myApp.cPage-1).id)+";");

            if(check2)
            {
                QMessageBox::information(0, qApp->tr(""),
                                           qApp->tr("آیتم با موفقیت حذف شد.")
                                                    ,"تایید");
                showDarsname(myApp.cGCatIndex, myApp.cCat);
            }
            else
                QMessageBox::critical(0, qApp->tr("خطا"),
                                           qApp->tr("خطا در حذف آیتم.")
                                                    ,"تایید");
        }
    }
}

void MainWindow::changeLesson()
{
    if(myApp.pageCount > 0)
    {
        if(myApp.cMode == 4)
        {
            addlessonDialog = new addlesson(0, darsnameList.at(myApp.cPage-1).id);
            addlessonDialog->setWindowFlags( Qt::CustomizeWindowHint
                                             | Qt::WindowCloseButtonHint
                                             | Qt::WindowMaximizeButtonHint );
            addlessonDialog->show();
        }
        else
        {
            addquestionDialog = new addquestion(0, questionList.at(myApp.cPage-1).id, myApp.cMode);
            addquestionDialog->setWindowFlags(Qt::CustomizeWindowHint
                                              | Qt::WindowCloseButtonHint
                                              | Qt::WindowMaximizeButtonHint );
            addquestionDialog->show();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonCatToggle_clicked()
{
    if(catToogle)
    {
        geometryIni(202);
        catToogle = false;
        ui->pushButtonCatToggle->setText("<");
    }
    else
    {
        geometryIni(0);
        catToogle = true;
        ui->pushButtonCatToggle->setText(">");
    }
    //mode0 = darsname
    if(myApp.pageCount > 0)
    {
        if(myApp.cMode == 4)
            showDarsnamePage(myApp.cPage);
        else
            showQuestionPage(myApp.cPage);
    }
}

void MainWindow::on_pushButtonLeftEnd_clicked()
{
    if(myApp.cMode == 4)
        showDarsnamePage(0);
    else
        showQuestionPage(0);
}

void MainWindow::on_pushButtonLeft_clicked()
{
    if(myApp.cMode == 4)
        showDarsnamePage(myApp.cPage - 1);
    else
        showQuestionPage(myApp.cPage - 1);
}

void MainWindow::on_pushButtonRight_clicked()
{
    if(myApp.cMode == 4)
        showDarsnamePage(myApp.cPage + 1);
    else
        showQuestionPage(myApp.cPage + 1);
}

void MainWindow::on_pushButtonRightEnd_clicked()
{
    if(myApp.cMode == 4)
        showDarsnamePage(myApp.pageCount);
    else
        showQuestionPage(myApp.pageCount);
}

void MainWindow::changeMusicBar(qint64 pos)
{
    ui->musicBar->setMaximum(mediaPlayer->duration());
    ui->musicBar->setValue(pos);
    QTime displayTime(0, (pos / 60000) % 60, (pos / 1000) % 60);
    ui->lcdNumber->display(displayTime.toString("mm:ss"));
        //timeLcd->display(displayTime.toString("mm:ss"));
}

void MainWindow::on_musicBar_actionTriggered(int action)
{
    mediaPlayer->setPosition(ui->musicBar->value());
}

void MainWindow::on_pushButtonPrev_clicked()
{
    mediaPlayer->playlist()->previous();
}

void MainWindow::on_pushButtonStop_clicked()
{
    mediaPlayer->stop();
    ui->pushButtonPlay->setStyleSheet("border-image:url(image/media-play.png);");
}

void MainWindow::on_pushButtonPlay_clicked()
{
    if(mediaPlayer->state() == QMediaPlayer::PlayingState)
    {
        mediaPlayer->pause();
        ui->pushButtonPlay->setStyleSheet("border-image:url(image/media-play.png);");
    }
    else
    {
        mediaPlayer->play();
        ui->pushButtonPlay->setStyleSheet("border-image:url(image/media-pause.png);");
    }
}

void MainWindow::on_pushButtonNext_clicked()
{
    mediaPlayer->playlist()->next();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if(ui->treeWidget->currentIndex().parent().row() != -1)
    {
        int parent = ui->treeWidget->currentIndex().parent().row();
        int child = ui->treeWidget->currentIndex().row();
        if(parent == 0)
        {
            showDarsname(child);
        }
        else if(parent == 1)
        {
            showQuestion(0,child);
        }
        else if(parent == 2)
        {
            showQuestion(1,child);
        }
        else if(parent == 3)
        {
            showQuestion(2,child);
        }
    }
    else if(ui->treeWidget->currentIndex().row() == 4)
    {
        showQuestion(3,0);
    }
}

void MainWindow::on_treeWidgetcat_itemClicked(QTreeWidgetItem *item, int column)
{
    QString cat = ui->treeWidgetcat->currentItem()->text(0);
    if(myApp.cMode == 4)
        showDarsname(myApp.cGCatIndex, cat);
    else
        showQuestion(myApp.cMode, myApp.cGCatIndex, cat);
}

void MainWindow::on_lineEditPage_returnPressed()
{
    int page = ui->lineEditPage->text().toInt();
    if(myApp.cMode == 4)
        showDarsnamePage(page);
    else
        showQuestionPage(page);
}

void MainWindow::showSolution(int id)
{
    if(questionTimer->isActive())
        questionTimer->stop();
    if(id <= 0)
    {
    item2->setPixmap(*pixMap2);
    QGraphicsScene* sceneTemp2 = new QGraphicsScene();
    sceneTemp2->addItem(item2);
    blurEffect->setEnabled(false);
    ui->graphicsViewSol->setScene(sceneTemp2);
    ui->graphicsViewSol->fitInView(item2,Qt::KeepAspectRatio);
    ui->graphicsViewSol->show();
    ui->pushButtonShowSol->setVisible(false);
    }
    if(id == -1) id = myApp.cPage - 1;
    question tempList;
    tempList = questionList.at(id);
    tempList.isAnswered = true;
    questionList.replace(id,tempList);
}

void MainWindow::showAllSolutions()
{
    for(int i=0; i < questionList.count(); i++)
        showSolution(i);
}

void MainWindow::on_pushButtonShowSol_clicked()
{
    showSolution();
}

void MainWindow::showSetting()
{
    settingDialog = new setting();
    settingDialog->setWindowFlags(Qt::CustomizeWindowHint
                            | Qt::WindowCloseButtonHint);
    connect(settingDialog,SIGNAL(exits()),this,SLOT(settingExits()));
    settingDialog->show();
}

void MainWindow::settingExits()
{
    iniConfigs(false);
}

QList<question> MainWindow::scrambleList(QList<question> list, int max)
{
    QList<int> randList;
    int num = list.count();
    int randNumb;
    for(int i = 0; i < max && i < num; i++)
    {
        randNumb = rand()%num;
        if(randList.contains(randNumb))
        {
            i--;
            continue;
        }
        else
            randList.append(randNumb);
    }

    lastRandList.clear();
    for(int i = 0; i < randList.count(); i++)
        lastRandList.append(list.at(randList.at(i)).id);

    return scrambleList(list, max, randList);
}

QList<question> MainWindow::scrambleList(QList<question> list, int max, QList<int> randomList)
{
    QList<question> tempList;
    for(int i = 0; i < randomList.count(); i++)
        tempList.append(list.at(randomList.at(i)));

    return tempList;
}

QList<question> MainWindow::sortByRandomOrder(QList<question> list, QList<int> randomList)
{
    QList<question> tempList;
    for(int i = 0; i < randomList.count(); i++)
    {
        for(int j = 0; j < list.count(); j++)
        {
            if(randomList.at(i) == list.at(j).id)
            {
                tempList.append(list.at(j));
                break;
            }
        }
    }
    return tempList;
}

void MainWindow::qTimerSlot()
{
    if(myConfig.qMode == 1)
        return;
    myApp.cQTimer++;
    if(myConfig.qMode == 2)
    {
        if(myApp.cQTimer >= myConfig.qTime + myApp.addQTimer)
        {
            questionTimer->stop();
            if(mediaPlayer->state() == QMediaPlayer::PlayingState)
                on_pushButtonPlay_clicked();
            bool check = QMessageBox::warning(this,"زمان پاسخگوی"
                                      ,"زمان پاسخگویی تمام شد"
                                        ,"ادامه"
                                        ,"نمایش پاسخ");
            if(check)
            {
                if(mediaPlayer->state() == QMediaPlayer::PlayingState)
                    on_pushButtonPlay_clicked();
                showSolution();
                myApp.addQTimer = 0;
            }
            else
            {
                if(mediaPlayer->state() == QMediaPlayer::PausedState && myConfig.playMusic)
                    on_pushButtonPlay_clicked();
                myApp.addQTimer += myConfig.qTime;
                questionTimer->start();
            }
        }
    }
    else if(myConfig.qMode == 3)
    {
        if(myApp.cQTimer >= myConfig.qTime*questionList.count() + myApp.addQTimer)
        {
            questionTimer->stop();
            if(mediaPlayer->state() == QMediaPlayer::PlayingState)
                on_pushButtonPlay_clicked();
            bool check = QMessageBox::warning(this,"زمان پاسخگوی"
                                      ,"زمان پاسخگویی تمام شد"
                                        ,"ادامه"
                                        ,"نمایش پاسخ ها");
            if(check)
            {
                if(mediaPlayer->state() == QMediaPlayer::PlayingState)
                    on_pushButtonPlay_clicked();
                showAllSolutions();
                myApp.addQTimer = 0;
            }
            else
            {
                if(mediaPlayer->state() == QMediaPlayer::PausedState && myConfig.playMusic)
                    on_pushButtonPlay_clicked();
                myApp.addQTimer += myConfig.qTime;
                questionTimer->start();
            }
        }
    }
    QTime displayTime(0, (myApp.cQTimer / 60) % 60, myApp.cQTimer % 60);
    ui->lcdNumberTimer->display(displayTime.toString("mm:ss"));
}

void MainWindow::printQusetions()
{
    if(questionList.count()<= 0)
        return;
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    QPixmap *printPix = new QPixmap(10,10);
    QPixmap *questionTemp = new QPixmap(QDir::currentPath()+"/image/questionsb.png");
    QPixmap *solutionTemp = new QPixmap(QDir::currentPath()+"/image/solutionsb.png");
    QPrintDialog *printerDialog = new QPrintDialog(&printer,0);
    bool selectedToPrint = checkHaveToPrint();
    if(printerDialog->exec() == QDialog::Accepted)
    {
        QPainter painter(&printer);
        double paperW = painter.window().width(),
               paperH = painter.window().height();
        long nextTop = 0.028506 * 2 * paperH + 5;
        int marginW = 0.040322 * paperW + 5;
        int marginH = 0.028506 * paperH + 5;
        painter.drawPixmap(QRect(0,0,paperW,paperH),*questionTemp);
        int tempW, tempH;
        bool check;
        int questionNumb = 0;
        for(int i = 0; i < questionList.count(); i++)
        {
            if((selectedToPrint && questionList.at(i).toPrint) || !selectedToPrint)
            {
                questionNumb++;
                printPix->load(QDir::currentPath()+"/data/"+questionList.at(i).picDir);
                tempW = printPix->width();
                tempH = printPix->height();
                check = false;
                drawNumb(*printPix,questionList.at(i).numbPic,questionNumb);
                if(nextTop + ((paperW-marginW*2)/tempW)*tempH > paperH - marginH)
                {
                    nextTop = 0.028506 * 2 * paperH + 10;
                    check = true;
                    printer.newPage();
                    painter.drawPixmap(QRect(0,0,paperW,paperH),*questionTemp);
                }
                painter.drawPixmap(QRect(marginW,nextTop,paperW - marginW * 2,((paperW-marginW*2)/tempW)*tempH),*printPix);
                if(!check)
                    nextTop += ((paperW-marginW*2)/tempW)*tempH+5;
            }
        }
        printer.newPage();
        painter.drawPixmap(QRect(0,0,paperW,paperH),*solutionTemp);
        nextTop = 0.028506 * 2 * paperH + 10;
        questionNumb = 0;
        for(int i = 0; i < questionList.count(); i++)
        {
            if((selectedToPrint && questionList.at(i).toPrint) || !selectedToPrint)
            {
                questionNumb++;
                printPix->load(QDir::currentPath()+"/data/"+questionList.at(i).solDir);
                tempW = printPix->width();
                tempH = printPix->height();
                check = false;
                drawNumb(*printPix,questionList.at(i).numbSol,questionNumb);
                if(nextTop + ((paperW-marginW*2)/tempW)*tempH > paperH - marginH)
                {
                    nextTop = 0.028506 * 2 * paperH + 10;
                    check = true;
                    printer.newPage();
                    painter.drawPixmap(QRect(0,0,paperW,paperH),*questionTemp);
                }
                painter.drawPixmap(QRect(marginW,nextTop,paperW - marginW * 2,((paperW-marginW*2)/tempW)*tempH),*printPix);
                if(!check)
                    nextTop += ((paperW-marginW*2)/tempW)*tempH+5;
            }
        }
        painter.end();
    }
    delete printerDialog;
}

bool MainWindow::checkHaveToPrint()
{
    for(int i = 0; i < questionList.count(); i++)
    {
        if(questionList.at(i).toPrint)
            return true;
    }
    return false;
}

void MainWindow::on_checkBoxPrintSelect_clicked()
{
    if(!questionList.at(myApp.cPage-1).toPrint)
    {
        ui->checkBoxPrintSelect->setChecked(true);
        question tempList;
        tempList = questionList.at(myApp.cPage-1);
        tempList.toPrint = true;
        questionList.replace(myApp.cPage-1,tempList);
    }
    else
    {
        ui->checkBoxPrintSelect->setChecked(false);
        question tempList;
        tempList = questionList.at(myApp.cPage-1);
        tempList.toPrint = false;
        questionList.replace(myApp.cPage-1,tempList);
    }
}

void MainWindow::showAbout()
{
    aboutDialog = new about();
    aboutDialog->setWindowFlags(Qt::CustomizeWindowHint
                         | Qt::WindowCloseButtonHint);
    aboutDialog->show();

}
