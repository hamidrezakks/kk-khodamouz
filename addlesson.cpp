#include "addlesson.h"
#include "ui_addlesson.h"

#include "mainwindow.h"

#include <QFileDialog>
#include <QPixmap>
#include <QMouseEvent>
#include <QTimer>
#include <QSettings>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>

#include <QDebug>


addlesson::addlesson(QWidget *parent, int itemId):
    QDialog(parent),
    ui(new Ui::addlesson)
{
    ui->setupUi(this);

    cropBand = new QRubberBand(QRubberBand::Line, this);
    startCheck = false;

    currentGrade = "_5om";
    currentGradeIndex = 0;

    /*QPalette pal;
    pal.setBrush(QPalette::Highlight, QBrush(Qt::red));
    cropBand->setPalette(pal);*/
    lastWindowSize = this->rect();
    resizeTimer = new QTimer(this);
    resizeTimer->setInterval(500);
    connect(resizeTimer,SIGNAL(timeout()),this,SLOT(timerResizeEvent()));

    startPoint = endPoint = QPoint(0,0);

    imageSelected = false;

    itemID = itemId;

    if(itemId == -1)
    {
        setWindowTitle("افزودن درس نامه");
        pix = new QPixmap(5,5);
        showCategory();
    }
    else
    {
        setWindowTitle("تغییر درس نامه");
        ui->pushButtonAdd->setText("اعمال تغییرات");
        ui->checkBox->setVisible(false);
        int tempGCat;
        QString tempCat;
        QSqlQuery query("SELECT * FROM darsname WHERE state = 1 AND id = "+QString::number(itemId)+" ;");
        if(query.next())
        {
            fileName = QDir::currentPath()+"/data/darsname/"+query.value(4).toString();
            tempGCat = query.value(2).toInt();
            tempCat = query.value(3).toString();
        }

        if(!QFile::exists(fileName))
        {
            QMessageBox::warning(0, qApp->tr("خطا"),
            qApp->tr("فایل مورد نظر موجود نیست!")
                                , "تایید");
            this->close();
            return;
        }
        showCategory(tempGCat);
        QPixmap pix3;
        pix = new QPixmap(fileName);
        pix3 = pix->scaled(ui->picLabel->width(),ui->picLabel->height(),Qt::KeepAspectRatio);
        ui->picLabel->setPixmap(pix3);
        ui->picLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        ui->comboBoxGradeCat->setCurrentIndex(tempGCat);
        ui->comboBoxCat->setCurrentText(tempCat);
    }
}

addlesson::~addlesson()
{
    delete ui;
    delete pix;
    delete cropBand;
}

void addlesson::closeEvent(QCloseEvent *)
{
    delete pix;
    delete cropBand;
}

void addlesson::resizeEvent(QResizeEvent *event)
{
    int height = event->size().height(), width = event->size().width();
    QRect browseBtn, addBtn, checkbox, catCombo, gcatCombo, otherText, group1, group2, picLable;

    picLable.setRect(10,10,width - 260,height - 20);
    browseBtn.setRect(width - 240,10,230,40);
    group1.setRect(width - 240,50,230,65);
    gcatCombo.setRect(10,25,210,30);
    group2.setRect(width - 240,120,230,105);
    catCombo.setRect(10,25,210,30);
    otherText.setRect(10,65,210,30);
    checkbox.setRect(width - 240,height - 80,230,20);
    addBtn.setRect(width - 240,height - 50,230,40);

    ui->picLabel->setGeometry(picLable);
    ui->pushButtonBrowse->setGeometry(browseBtn);
    ui->groupBox_2->setGeometry(group1);
    ui->comboBoxGradeCat->setGeometry(gcatCombo);
    ui->groupBox->setGeometry(group2);
    ui->comboBoxCat->setGeometry(catCombo);
    ui->lineEditOther->setGeometry(otherText);
    ui->checkBox->setGeometry(checkbox);
    ui->pushButtonAdd->setGeometry(addBtn);

    if(!resizeTimer->isActive() && pix->width() > 10)
        resizeTimer->start();
}

void addlesson::timerResizeEvent()
{

    if(lastWindowSize == this->rect())
    {
        if(pix->width() > 10)
        {
            QPixmap pix3;
            pix3 = pix->scaled(ui->picLabel->width(),ui->picLabel->height(),Qt::KeepAspectRatio);
            ui->picLabel->setPixmap(pix3);
            ui->picLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);
            resizeTimer->stop();
        }
    }

    lastWindowSize = this->rect();
}

QString addlesson::getDirByIndex(int index)
{
    switch (index) {
    case 0:
        return "_5om";
        break;
    case 1:
        return "_6om";
        break;
    case 2:
        return "_7om";
        break;
    case 3:
        return "_8om";
        break;
    case 4:
        return "_1motevasete";
        break;
    case 5:
        return "_2motevasete";
        break;
    case 6:
        return "_amar";
        break;
    case 7:
        return "_3motevasete_riazi";
        break;
    case 8:
        return "_3motevasete_tajrobi";
        break;
    case 9:
        return "_4motevasete_riazi";
        break;
    case 10:
        return "_4motevasete_tajrobi";
        break;
    default:
        return "_5om";
        break;
    }
}

void addlesson::showCategory(int gCatIndex)
{
    QSqlQuery query("SELECT lesson_category FROM darsname WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
    QString cat = "";
    QList<QString> catList;
    while(query.next())
    {
        if(query.value(0).toString().compare(cat) != 0)
        {
            cat = query.value(0).toString();
            catList.append(cat);
        }
    }
    ui->comboBoxCat->clear();
    ui->comboBoxCat->addItem("");
    for(int i=0; i < catList.count(); i++)
    {
        ui->comboBoxCat->addItem(catList.at(i));
    }
    ui->comboBoxCat->addItem("جدید");
    ui->lineEditOther->setText("");
}

void addlesson::mousePressEvent(QMouseEvent *event)
{
    if(ui->picLabel->underMouse())
    {
        startPoint = event->pos();
        //startPoint = ui->label->mapFromParent(startPoint);
        cropBand->setGeometry(QRect(startPoint,startPoint));
        cropBand->show();
        startCheck = true;
    }
}

void addlesson::mouseMoveEvent(QMouseEvent *event)
{
    if(startCheck)
    {
        QPoint tempPoint, tempPoint2;
        tempPoint = event->pos();
        tempPoint2 = ui->picLabel->mapFromParent(tempPoint);
        if(tempPoint2.x()<0)
            tempPoint.setX(ui->picLabel->geometry().x());
        if(tempPoint2.y()<0)
            tempPoint.setY(ui->picLabel->geometry().y());
        if(tempPoint2.x() > ui->picLabel->geometry().width())
            tempPoint.setX(ui->picLabel->geometry().x()+ui->picLabel->geometry().width());
        if(tempPoint2.y() > ui->picLabel->geometry().height())
            tempPoint.setY(ui->picLabel->geometry().y()+ui->picLabel->geometry().height());

        cropBand->setGeometry(QRect(startPoint,tempPoint).normalized());
    }
}

void addlesson::mouseReleaseEvent(QMouseEvent *event)
{
    //cropBand->hide();
    if(startCheck)
    {
        QPoint tempPoint;
        tempPoint = event->pos();
        if(tempPoint.x()<ui->picLabel->geometry().x())
            tempPoint.setX(ui->picLabel->geometry().x());
        if(tempPoint.y()<ui->picLabel->geometry().y())
            tempPoint.setY(ui->picLabel->geometry().y());
        if(tempPoint.x() > ui->picLabel->geometry().x()+ui->picLabel->geometry().width())
            tempPoint.setX(ui->picLabel->geometry().x()+ui->picLabel->geometry().width());
        if(tempPoint.y() > ui->picLabel->geometry().y()+ui->picLabel->geometry().height())
            tempPoint.setY(ui->picLabel->geometry().y()+ui->picLabel->geometry().height());
        endPoint = tempPoint;
        startCheck = false;
    }
}

void addlesson::on_pushButtonBrowse_clicked()
{
    QSettings settings(settingAddress, QSettings::IniFormat);
    fileName = QFileDialog::getOpenFileName(this,
                        tr("Open Image"), settings.value(DEFAULT_DIR_KEY).toString(), tr("Image Files (*.png *.jpg *.bmp)"));
    if(!fileName.isEmpty())
    {
        QDir CurrentDir;
        settings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(fileName));

        QPixmap pix3;
        pix->load(fileName);
        pix3 = pix->scaled(ui->picLabel->width(),ui->picLabel->height(),Qt::KeepAspectRatio);
        ui->picLabel->setPixmap(pix3);
        ui->picLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        imageSelected = true;
    }
}

void addlesson::on_pushButtonAdd_clicked()
{
    if(pix->width() > 10
       && ui->comboBoxCat->currentText() != ""
       && ((ui->comboBoxCat->currentText() == "جدید" && ui->lineEditOther->text().length() > 2) || ui->comboBoxCat->currentText() != "جدید"))
    {
        bool checkRubber = false;
        if(startPoint == QPoint(0,0) && endPoint == QPoint(0,0))
        {
            startPoint = ui->picLabel->geometry().topLeft();
            endPoint = ui->picLabel->geometry().bottomRight();
            checkRubber = true;
        }
        QRect cropRect;
        QRect imageRect;
        QRect lableRect;

        int picId = 10000;

        if(itemID == -1)
        {
            QSqlQuery squery("SELECT id FROM darsname WHERE state = 1 OR state = 0 ORDER BY id DESC");
            if(squery.next())
            {
                picId = squery.value(0).toInt();
                picId++;
            }
        }
        else
            picId = itemID;

        imageRect = pix->rect();
        lableRect = ui->picLabel->rect();
        double ratio;
        if(double(imageRect.width())/imageRect.height() > double(lableRect.height())/lableRect.height())
            ratio = double(imageRect.width())/lableRect.width();
        else
            ratio = double(imageRect.height())/lableRect.height();

        cropRect.setTopLeft((ui->picLabel->mapFromParent(startPoint))*ratio);
        cropRect.setBottomRight((ui->picLabel->mapFromParent(endPoint))*ratio);
        cropRect = cropRect.normalized();
        QPixmap pix2(endPoint.x()-startPoint.x(),endPoint.y()-endPoint.y());
        if(!imageSelected && checkRubber)
            pix2 = *pix;
        else
            pix2 = pix->copy(cropRect);

        QString filePath = currentGrade+"/p"+QString::number(picId)+".png";
        pix2.save(QDir::currentPath()+"/data/darsname/"+filePath);

        //MainWindow::myDB
        QString cdate = QDateTime::currentDateTime().toString("dd.MM.yyyy h:m:s ap");

        QString lessonCat = "";
        if(ui->comboBoxCat->currentText() == "جدید")
            lessonCat = ui->lineEditOther->text();
        else
            lessonCat = ui->comboBoxCat->currentText();

        QString str;

        if(itemID == -1)
            str = "INSERT INTO `darsname`(`id`,`grade_category`,`grade_index`,`lesson_category`,`pic_dir`,`date`, `state`) "
                " VALUES ("+QString::number(picId)+",'"+currentGrade+"',"+QString::number(currentGradeIndex)+","
                "'"+lessonCat+"','"+filePath+"','"+cdate+"', 1)";
        else
            str = "UPDATE `darsname` SET `grade_category` = '"+currentGrade+"' , `grade_index` = "+QString::number(currentGradeIndex)+", "
                  " `lesson_category` = '"+lessonCat+"' ,`pic_dir` = '"+filePath+"' "
                  "WHERE id = "+QString::number(itemID)+" ;";
        QSqlQuery query;
        bool state = query.exec(str);
        //qDebug()<<query
        if(ui->checkBox->checkState() == Qt::Checked)
        {
            if(state)
                this->close();
            else
                QMessageBox::critical(0, qApp->tr("خطا"),
                qApp->tr("خطا در افزودن اسلاید")
                                    , "تایید");
        }
        else
        {
            if(state)
                QMessageBox::information(0, qApp->tr(""),
                qApp->tr("اسلاید با موفقیت افزوده شد.")
                                    , "تایید");
            else
                QMessageBox::critical(0, qApp->tr("خطا"),
                qApp->tr("خطا در افزودن اسلاید")
                                    , "تایید");
            showCategory();
        }
    }
    else
    {
        QMessageBox::warning(0, qApp->tr("خطا"),
        qApp->tr("لطفا فرم را کامل پر کنید")
                            , "تایید");
    }
}

void addlesson::on_comboBoxGradeCat_currentIndexChanged(int index)
{
    currentGradeIndex = index;
    currentGrade = getDirByIndex(index);
    showCategory(index);
}
