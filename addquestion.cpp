#include "addquestion.h"
#include "ui_addquestion.h"

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
#include <QtSql/QSqlError>

#include <QDebug>



addquestion::addquestion(QWidget *parent, int itemId, int qCat) :
    QDialog(parent),
    ui(new Ui::addquestion)
{
    ui->setupUi(this);

    cropBandPic = new QRubberBand(QRubberBand::Line, this);
    cropBandNumb = new QRubberBand(QRubberBand::Line, this);
    cropBandPicSol = new QRubberBand(QRubberBand::Line, this);
    cropBandNumbSol = new QRubberBand(QRubberBand::Line, this);

    startCheckPic = startCheckNumb = startCheckPicSol = startCheckNumbSol = false;

    currentQIndex = 0;
    currentGrade = "_5om";
    currentGradeIndex = 0;

    QPalette pal;
    pal.setBrush(QPalette::Highlight, QBrush(Qt::red));
    cropBandNumb->setPalette(pal);
    cropBandNumbSol->setPalette(pal);


    lastWindowSize = this->rect();
    resizeTimer = new QTimer(this);
    resizeTimer->setInterval(500);
    connect(resizeTimer,SIGNAL(timeout()),this,SLOT(timerResizeEvent()));

    startPointPic = startPointNumb = startPointPicSol = startPointNumbSol = QPoint(0,0);
    endPointPic = endPointNumb = endPointPicSol = endPointNumbSol = QPoint(0,0);

    isQ = true;
    isPic = true;

    ui->radioButtonQ->setChecked(true);
    ui->radioButtonS->setDisabled(true);

    imageSelected = false;

    itemID = itemId;

    if(itemId == -1)
    {
        setWindowTitle("افزودن پرسش");
        pix = new QPixmap(5,5);
        pixSol = new QPixmap(5,5);
        showCategory();
    }
    else
    {
        setWindowTitle("تغییر پرسش");
        ui->pushButtonAdd->setText("اعمال تغییرات");
        ui->checkBox->setVisible(false);

        int tempGCat;
        QString tempCat;
        QSqlQuery query;
        if(qCat == 0)
            query.exec("SELECT * FROM qtesti WHERE state = 1 AND id = "+QString::number(itemId)+" ;");
        else if(qCat == 1)
            query.exec("SELECT * FROM qtashrihi WHERE state = 1 AND id = "+QString::number(itemId)+" ;");
        else if(qCat == 2)
            query.exec("SELECT * FROM qsakht WHERE state = 1 AND id = "+QString::number(itemId)+" ;");
        else if(qCat == 3)
            query.exec("SELECT * FROM qhoosh WHERE state = 1 AND id = "+QString::number(itemId)+" ;");
        if(query.next())
        {
            if(qCat == 3)
            {
                fileName = QDir::currentPath()+"/data/"+query.value(2).toString();
                lastPicNumb = query.value(3).toString();
                fileNameSol = QDir::currentPath()+"/data/"+query.value(4).toString();
                lastSolNumb = query.value(5).toString();
                tempGCat = -1;
                tempCat = query.value(1).toString();
            }
            else
            {
                fileName = QDir::currentPath()+"/data/"+query.value(3).toString();
                lastPicNumb = query.value(4).toString();
                fileNameSol = QDir::currentPath()+"/data/"+query.value(5).toString();
                lastSolNumb = query.value(6).toString();
                tempGCat = query.value(1).toInt();
                tempCat = query.value(2).toString();
            }
        }

        if(!QFile::exists(fileName) || !QFile::exists(fileNameSol))
        {
            QMessageBox::warning(0, qApp->tr("خطا"),
            qApp->tr("فایل مورد نظر موجود نیست!")
                                , "تایید");
            this->close();
            return;
        }

        showCategory(qCat,tempGCat);

        QPixmap pix3;
        pix = new QPixmap(fileName);
        pix3 = pix->scaled(ui->picLabel->width(),ui->picLabel->height(),Qt::KeepAspectRatio);
        ui->picLabel->setPixmap(pix3);
        ui->picLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);

        pixSol = new QPixmap(fileNameSol);
        pix3 = pixSol->scaled(ui->picLabelSol->width(),ui->picLabelSol->height(),Qt::KeepAspectRatio);
        ui->picLabelSol->setPixmap(pix3);
        ui->picLabelSol->setAlignment(Qt::AlignLeft|Qt::AlignTop);

        ui->comboBoxQCat->setCurrentIndex(qCat);
        if(tempGCat == -1)
            ui->comboBoxGradeCat->setDisabled(true);
        else
            ui->comboBoxGradeCat->setCurrentIndex(tempGCat);
        ui->comboBoxCat->setCurrentText(tempCat);

        lastQCat = qCat;
    }
}

addquestion::~addquestion()
{
    delete ui;
    delete pix;
    delete pixSol;
    delete cropBandPic;
    delete cropBandNumb;
    delete cropBandPicSol;
    delete cropBandNumbSol;
}

void addquestion::closeEvent(QCloseEvent *)
{
    delete pix;
    delete pixSol;
    delete cropBandPic;
    delete cropBandNumb;
    delete cropBandPicSol;
    delete cropBandNumbSol;
}

void addquestion::resizeEvent(QResizeEvent *event)
{
    int height = event->size().height(), width = event->size().width();
    QRect browseBtn, addBtn, checkbox, qcatCombo, catCombo, gcatCombo, otherText, group1, group2;
    QRect group3, radioQ, radioS, tabWidget, picLable, picLableSol;

    browseBtn.setRect(width - 240,10,230,40);
    group1.setRect(width - 240,50,230,105);
    qcatCombo.setRect(10,25,210,30);
    gcatCombo.setRect(10,65,210,30);
    group2.setRect(width - 240,160,230,105);
    catCombo.setRect(10,25,210,30);
    otherText.setRect(10,65,210,30);
    group3.setRect(width - 240,270,230,50);
    radioS.setRect(10,20,100,30);
    radioQ.setRect(120,20,100,30);
    checkbox.setRect(width - 240,height - 80,230,20);
    addBtn.setRect(width - 240,height - 50,230,40);

    tabWidget.setRect(10,10,width - 260,height - 20);

    //ui->picLabel->setGeometry(picLable);
    ui->pushButtonBrowse->setGeometry(browseBtn);
    ui->groupBox_2->setGeometry(group1);
    ui->comboBoxQCat->setGeometry(qcatCombo);
    ui->comboBoxGradeCat->setGeometry(gcatCombo);
    ui->groupBox->setGeometry(group2);
    ui->comboBoxCat->setGeometry(catCombo);
    ui->lineEditOther->setGeometry(otherText);
    ui->groupBox_3->setGeometry(group3);
    ui->radioButtonQ->setGeometry(radioQ);
    ui->radioButtonS->setGeometry(radioS);

    ui->checkBox->setGeometry(checkbox);
    ui->pushButtonAdd->setGeometry(addBtn);

    ui->tabWidget->setGeometry(tabWidget);

    int tiw = ui->tab->width(), tih = ui->tab->height();
    picLable.setRect(0,0,tiw,tih);
    picLableSol.setRect(0,0,tiw,tih);

    ui->picLabel->setGeometry(picLable);
    ui->picLabelSol->setGeometry(picLableSol);


    if(!resizeTimer->isActive() && pix->width() > 10)
        resizeTimer->start();
}

void addquestion::timerResizeEvent()
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
        if(pixSol->width() > 10)
        {
            QPixmap pix3;
            pix3 = pixSol->scaled(ui->picLabelSol->width(),ui->picLabelSol->height(),Qt::KeepAspectRatio);
            ui->picLabelSol->setPixmap(pix3);
            ui->picLabelSol->setAlignment(Qt::AlignLeft|Qt::AlignTop);
            resizeTimer->stop();
        }
    }

    lastWindowSize = this->rect();
}

QString addquestion::getDirByIndex(int index)
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

void addquestion::showCategory(int qCat, int gCatIndex)
{
    QSqlQuery query;
    if(qCat == 0)
        query.exec("SELECT lesson_category FROM qtesti WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
    else if(qCat == 1)
        query.exec("SELECT lesson_category FROM qtashrihi WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
    else if(qCat == 2)
        query.exec("SELECT lesson_category FROM qsakht WHERE state = 1 AND grade_index = "+QString::number(gCatIndex)+" ORDER BY lesson_category ASC");
    else if(qCat == 3)
        query.exec("SELECT lesson_category FROM qhoosh WHERE state = 1 ORDER BY lesson_category ASC");

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

void addquestion::mousePressEvent(QMouseEvent *event)
{
    if(isQ)
    {
        if(ui->picLabel->underMouse())
        {
            if(isPic)
            {
                startPointPic = event->pos();
                cropBandPic->setGeometry(QRect(startPointPic,startPointPic));
                cropBandPic->show();
                startCheckPic = true;
            }
            else if(cropBandPic->geometry().contains(event->pos()))
            {
                startPointNumb = event->pos();
                cropBandNumb->setGeometry(QRect(startPointNumb,startPointNumb));
                cropBandNumb->show();
                startCheckNumb = true;
            }
        }
    }
    else
    {
        if(ui->picLabelSol->underMouse())
        {
            if(isPic)
            {
                startPointPicSol = event->pos();
                cropBandPicSol->setGeometry(QRect(startPointPicSol,startPointPicSol));
                cropBandPicSol->show();
                startCheckPicSol = true;
            }
            else if(cropBandPicSol->geometry().contains(event->pos()))
            {
                startPointNumbSol = event->pos();
                cropBandNumbSol->setGeometry(QRect(startPointNumbSol,startPointNumbSol));
                cropBandNumbSol->show();
                startCheckNumbSol = true;
            }
        }
    }
}

void addquestion::mouseMoveEvent(QMouseEvent *event)
{
    if(startCheckPic || startCheckPicSol)
    {
        QPoint tempPoint, tempPoint2;
        tempPoint = event->pos();

        if(startCheckPic)
        {
            QPoint offset((ui->tabWidget->width()-ui->tab->width())/2,
                          ui->tabWidget->height()-(ui->tab->height()+(ui->tabWidget->width()-ui->tab->width())/2));
            offset += ui->tabWidget->geometry().topLeft() - QPoint(1,1);

            tempPoint2 = tempPoint - offset;
            if(tempPoint2.x()<0)
                tempPoint.setX(offset.x());
            if(tempPoint2.y()<0)
                tempPoint.setY(offset.y());
            if(tempPoint2.x() > ui->picLabel->geometry().width())
                tempPoint.setX(ui->picLabel->width()+offset.x()-1);
            if(tempPoint2.y() > ui->picLabel->geometry().height())
                tempPoint.setY(ui->picLabel->height()+offset.y());

            cropBandPic->setGeometry(QRect(startPointPic,tempPoint).normalized());
        }
        else
        {
            QPoint offset((ui->tabWidget->width()-ui->tab->width())/2,
                          ui->tabWidget->height()-(ui->tab->height()+(ui->tabWidget->width()-ui->tab->width())/2));
            offset += ui->tabWidget->geometry().topLeft() - QPoint(1,1);

            tempPoint2 = tempPoint - offset;
            if(tempPoint2.x()<0)
                tempPoint.setX(offset.x());
            if(tempPoint2.y()<0)
                tempPoint.setY(offset.y());
            if(tempPoint2.x() > ui->picLabelSol->geometry().width())
                tempPoint.setX(ui->picLabelSol->width()+offset.x()-1);
            if(tempPoint2.y() > ui->picLabelSol->geometry().height())
                tempPoint.setY(ui->picLabelSol->height()+offset.y());

            cropBandPicSol->setGeometry(QRect(startPointPicSol,tempPoint).normalized());
        }
    }
    else if(startCheckNumb || startCheckNumbSol)
    {
        QPoint tempPoint;
        tempPoint = event->pos();

        if(startCheckNumb)
        {
            if(tempPoint.x()<cropBandPic->geometry().x())
                tempPoint.setX(cropBandPic->geometry().x());
            if(tempPoint.y()<cropBandPic->geometry().y())
                tempPoint.setY(cropBandPic->geometry().y());
            if(tempPoint.x() > cropBandPic->geometry().bottomRight().x())
                tempPoint.setX(cropBandPic->geometry().bottomRight().x());
            if(tempPoint.y() > cropBandPic->geometry().bottomRight().y())
                tempPoint.setY(cropBandPic->geometry().bottomRight().y());

            cropBandNumb->setGeometry(QRect(startPointNumb,tempPoint).normalized());
        }
        else
        {
            if(tempPoint.x()<cropBandPicSol->geometry().x())
                tempPoint.setX(cropBandPicSol->geometry().x());
            if(tempPoint.y()<cropBandPicSol->geometry().y())
                tempPoint.setY(cropBandPicSol->geometry().y());
            if(tempPoint.x() > cropBandPicSol->geometry().bottomRight().x())
                tempPoint.setX(cropBandPicSol->geometry().bottomRight().x());
            if(tempPoint.y() > cropBandPicSol->geometry().bottomRight().y())
                tempPoint.setY(cropBandPicSol->geometry().bottomRight().y());

            cropBandNumbSol->setGeometry(QRect(startPointNumbSol,tempPoint).normalized());
        }
    }
}

void addquestion::mouseReleaseEvent(QMouseEvent *event)
{
    if(startCheckPic || startCheckPicSol)
    {
        QPoint tempPoint, tempPoint2;
        tempPoint = event->pos();

        if(startCheckPic)
        {
            QPoint offset((ui->tabWidget->width()-ui->tab->width())/2,
                          ui->tabWidget->height()-(ui->tab->height()+(ui->tabWidget->width()-ui->tab->width())/2));
            offset += ui->tabWidget->geometry().topLeft() - QPoint(1,1);

            tempPoint2 = tempPoint - offset;
            if(tempPoint2.x()<0)
                tempPoint.setX(offset.x());
            if(tempPoint2.y()<0)
                tempPoint.setY(offset.y());
            if(tempPoint2.x() > ui->picLabel->geometry().width())
                tempPoint.setX(ui->picLabel->width()+offset.x()-1);
            if(tempPoint2.y() > ui->picLabel->geometry().height())
                tempPoint.setY(ui->picLabel->height()+offset.y());

            endPointPic = tempPoint;
            startCheckPic = false;
            ui->radioButtonS->setDisabled(false);
            ui->radioButtonS->setChecked(true);
        }
        else
        {
            QPoint offset((ui->tabWidget->width()-ui->tab_2->width())/2,
                          ui->tabWidget->height()-(ui->tab_2->height()+(ui->tabWidget->width()-ui->tab_2->width())/2));
            offset += ui->tabWidget->geometry().topLeft() - QPoint(1,1);

            tempPoint2 = tempPoint - offset;
            if(tempPoint2.x()<0)
                tempPoint.setX(offset.x());
            if(tempPoint2.y()<0)
                tempPoint.setY(offset.y());
            if(tempPoint2.x() > ui->picLabelSol->geometry().width())
                tempPoint.setX(ui->picLabelSol->width()+offset.x()-1);
            if(tempPoint2.y() > ui->picLabelSol->geometry().height())
                tempPoint.setY(ui->picLabelSol->height()+offset.y());

            endPointPicSol = tempPoint;
            startCheckPicSol = false;
            ui->radioButtonS->setDisabled(false);
            ui->radioButtonS->setChecked(true);
        }
    }
    else if(startCheckNumb || startCheckNumbSol)
    {
        QPoint tempPoint;
        tempPoint = event->pos();

        if(startCheckNumb)
        {
            if(tempPoint.x()<cropBandPic->geometry().x())
                tempPoint.setX(cropBandPic->geometry().x());
            if(tempPoint.y()<cropBandPic->geometry().y())
                tempPoint.setY(cropBandPic->geometry().y());
            if(tempPoint.x() > cropBandPic->geometry().bottomRight().x())
                tempPoint.setX(cropBandPic->geometry().bottomRight().x());
            if(tempPoint.y() > cropBandPic->geometry().bottomRight().y())
                tempPoint.setY(cropBandPic->geometry().bottomRight().y());

            endPointNumb = tempPoint;
            startCheckNumb = false;
        }
        else
        {
            if(tempPoint.x()<cropBandPicSol->geometry().x())
                tempPoint.setX(cropBandPicSol->geometry().x());
            if(tempPoint.y()<cropBandPicSol->geometry().y())
                tempPoint.setY(cropBandPicSol->geometry().y());
            if(tempPoint.x() > cropBandPicSol->geometry().bottomRight().x())
                tempPoint.setX(cropBandPicSol->geometry().bottomRight().x());
            if(tempPoint.y() > cropBandPicSol->geometry().bottomRight().y())
                tempPoint.setY(cropBandPicSol->geometry().bottomRight().y());

            endPointNumbSol = tempPoint;
            startCheckNumbSol = false;
        }
    }
}

void addquestion::on_tabWidget_currentChanged(int index)
{
    isQ = (index == 0)? true : false;
    if(isQ)
    {
        if(startPointPic != QPoint(0,0) || endPointPic != QPoint(0,0))
        {
            cropBandPic->show();
            ui->radioButtonS->setDisabled(false);
        }
        else
        {
            ui->radioButtonS->setDisabled(true);
            ui->radioButtonQ->setChecked(true);
        }
        if(startPointNumb != QPoint(0,0) || endPointNumb != QPoint(0,0))
            cropBandNumb->show();
        cropBandPicSol->hide();
        cropBandNumbSol->hide();
    }
    else
    {
        cropBandPic->hide();
        cropBandNumb->hide();
        if(startPointPicSol != QPoint(0,0) || endPointPicSol != QPoint(0,0))
        {
            cropBandPicSol->show();
            ui->radioButtonS->setDisabled(false);
        }
        else
        {
            ui->radioButtonS->setDisabled(true);
            ui->radioButtonQ->setChecked(true);
        }
        if(startPointNumbSol != QPoint(0,0) || endPointNumbSol != QPoint(0,0))
            cropBandNumbSol->show();
    }
}

void addquestion::on_radioButtonQ_toggled(bool checked)
{
    isPic = checked;
}

void addquestion::on_pushButtonBrowse_clicked()
{
    QDir CurrentDir;

    QSettings settings(settingAddress, QSettings::IniFormat);

    if(isQ)
    {
        fileName = QFileDialog::getOpenFileName(this,
                            tr("Open Image"), settings.value(DEFAULT_DIR_KEY).toString(), tr("Image Files (*.png *.jpg *.bmp)"));
        if(fileName.isEmpty())
            return
        settings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(fileName));
        QPixmap pix3;
        pix->load(fileName);
        pix3 = pix->scaled(ui->picLabel->width(),ui->picLabel->height(),Qt::KeepAspectRatio);
        ui->picLabel->setPixmap(pix3);
        ui->picLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        imageSelected = true;
    }
    else
    {
        fileNameSol = QFileDialog::getOpenFileName(this,
                            tr("Open Image"), settings.value(DEFAULT_DIR_KEY).toString(), tr("Image Files (*.png *.jpg *.bmp)"));
        if(fileNameSol.isEmpty())
            return
        settings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(fileNameSol));
        QPixmap pix3;
        pixSol->load(fileNameSol);
        pix3 = pixSol->scaled(ui->picLabelSol->width(),ui->picLabelSol->height(),Qt::KeepAspectRatio);
        ui->picLabelSol->setPixmap(pix3);
        ui->picLabelSol->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        imageSelectedSol = true;
    }
}

void addquestion::on_comboBoxQCat_currentIndexChanged(int index)
{
    currentQIndex = index;
    showCategory(index,currentGradeIndex);
    if(index == 3)
        ui->comboBoxGradeCat->setDisabled(true);
    else
        ui->comboBoxGradeCat->setDisabled(false);
}

void addquestion::on_comboBoxGradeCat_currentIndexChanged(int index)
{
    currentGradeIndex = index;
    currentGrade = getDirByIndex(index);
    showCategory(currentQIndex,index);
}

QString addquestion::getNumbRectStr(QRect image, QRect layout, QPoint start, QPoint end, QPoint topLeft, QPoint bottomRight, QPoint offset)
{
    double ratio;
    if(double(image.width())/image.height() > double(layout.height())/layout.height())
        ratio = double(image.width())/layout.width();
    else
        ratio = double(image.height())/layout.height();

    QRect cropRect;
    cropRect.setTopLeft((start-offset)*ratio);
    cropRect.setBottomRight((end-offset)*ratio);
    cropRect = cropRect.normalized();

    QRect mainRect;
    mainRect.setTopLeft((topLeft-offset)*ratio);
    mainRect.setBottomRight((bottomRight-offset)*ratio);
    mainRect = mainRect.normalized();

    return QString("%1|%2|%3|%4").arg(cropRect.x()-mainRect.x()).arg(cropRect.y()-mainRect.y()).arg(cropRect.width()).arg(cropRect.height());
}

QPixmap addquestion::cropImage(QPixmap *imagePix, QRect image, QRect layout, QPoint start, QPoint end, QPoint offset)
{
    double ratio;
    if(double(image.width())/image.height() > double(layout.height())/layout.height())
        ratio = double(image.width())/layout.width();
    else
        ratio = double(image.height())/layout.height();

    QRect cropRect;
    cropRect.setTopLeft((start-offset)*ratio);
    cropRect.setBottomRight((end-offset)*ratio);
    cropRect = cropRect.normalized();

    QPixmap pix2(end.x()-start.x(),end.y()-end.y());
    pix2 = imagePix->copy(cropRect);

    return pix2;
}

void addquestion::on_pushButtonAdd_clicked()
{
    if(pix->width() > 10 && pixSol->width() > 10
       && ui->comboBoxCat->currentText() != ""
       && ((ui->comboBoxCat->currentText() == "جدید" && ui->lineEditOther->text().length() > 2) || ui->comboBoxCat->currentText() != "جدید"))
    {
        bool changePicCheck = false, changeSolCheck = false;
        if(((startPointPic == QPoint(0,0) && endPointPic == QPoint(0,0))
            ||(startPointNumb == QPoint(0,0) && endPointNumb == QPoint(0,0))
            ||(startPointPicSol == QPoint(0,0) && endPointPicSol == QPoint(0,0))
            ||(startPointNumbSol == QPoint(0,0) && endPointNumbSol == QPoint(0,0))) && itemID == -1)
        {
            QMessageBox::warning(0, qApp->tr("خطا"),
            qApp->tr("لطفا مرز پرسش ها را انتخاب کنید")
                                , "تایید");
            return;
        }

        if(itemID != -1)
        {
            if( startPointPic == QPoint(0,0) && endPointPic == QPoint(0,0) )
            {
                changePicCheck = false;
            }
            else
            {
                if(startPointNumb == QPoint(0,0) && endPointNumb == QPoint(0,0))
                {
                    QMessageBox::warning(0, qApp->tr("خطا"),
                    qApp->tr("مرز شماره پرسش را مشخص کنید")
                                        , "تایید");
                    return;
                }
                else
                    changePicCheck = true;
            }

            if( startPointPicSol == QPoint(0,0) && endPointPicSol == QPoint(0,0) )
            {
                changeSolCheck = false;
            }
            else
            {
                if(startPointNumbSol == QPoint(0,0) && endPointNumbSol == QPoint(0,0))
                {
                    QMessageBox::warning(0, qApp->tr("خطا"),
                    qApp->tr("مرز شماره پاسخ را مشخص کنید")
                                        , "تایید");
                    return;
                }
                else
                    changeSolCheck = true;
            }
        }
        else
        {
            changePicCheck = changeSolCheck = true;
        }

        QRect imagePicRect, lablePicRect;
        QRect imageSolRect, lableSolRect;

        int picId = 10000;

        if(itemID == -1 || lastQCat != currentQIndex)
        {
            QSqlQuery squery;
            if(currentQIndex == 0)
                squery.exec("SELECT id FROM qtesti WHERE state = 1 OR state = 0 ORDER BY id DESC");
            else if(currentQIndex == 1)
                squery.exec("SELECT id FROM qtashrihi WHERE state = 1 OR state = 0 ORDER BY id DESC");
            else if(currentQIndex == 2)
                squery.exec("SELECT id FROM qsakht WHERE state = 1 OR state = 0 ORDER BY id DESC");
            else if(currentQIndex == 3)
                squery.exec("SELECT id FROM qhoosh WHERE state = 1 OR state = 0 ORDER BY id DESC");
            if(squery.next())
            {
                picId = squery.value(0).toInt();
                picId++;
            }
        }
        else
            picId = itemID;

        QPoint offset((ui->tabWidget->width()-ui->tab->width())/2,
                      ui->tabWidget->height()-(ui->tab->height()+(ui->tabWidget->width()-ui->tab->width())/2));
        offset += ui->tabWidget->geometry().topLeft() - QPoint(1,1);

        QPixmap pixPicTmp;

        QString numbPic;
        if(changePicCheck)
        {
            imagePicRect = pix->rect();
            lablePicRect = ui->picLabel->rect();

            pixPicTmp = cropImage(pix, imagePicRect,lablePicRect,startPointPic,endPointPic,offset);

            numbPic = getNumbRectStr(imagePicRect,lablePicRect,startPointNumb,endPointNumb,startPointPic,endPointPic,offset);
        }
        else
        {
            pixPicTmp = *pix;
            numbPic = lastPicNumb;
        }

        QPixmap pixSolTmp;
        QString numbSol;
        if(changeSolCheck)
        {
            imageSolRect = pixSol->rect();
            lableSolRect = ui->picLabelSol->rect();

            pixSolTmp = cropImage(pixSol, imageSolRect,lableSolRect,startPointPicSol,endPointPicSol,offset);

            numbSol = getNumbRectStr(imageSolRect,lableSolRect,startPointNumbSol,endPointNumbSol,startPointPicSol,endPointPicSol,offset);
        }
        else
        {
            pixSolTmp = *pixSol;
            numbSol = lastSolNumb;
        }

        QString filePath = currentGrade+"/p"+QString::number(picId)+".png";
        QString filePathSol = currentGrade+"/s"+QString::number(picId)+".png";
        QString fileSubPath = "";

        if(currentQIndex == 0)
            fileSubPath = "testi/";
        else if(currentQIndex == 1)
            fileSubPath = "tashrihi/";
        else if(currentQIndex == 2)
            fileSubPath = "sakht/";
        else if(currentQIndex == 3)
        {
            fileSubPath = "hoosh/";
            filePath = "p"+QString::number(picId)+".png";
            filePathSol = "s"+QString::number(picId)+".png";
        }

        pixPicTmp.save(QDir::currentPath()+"/data/"+fileSubPath+filePath);
        pixSolTmp.save(QDir::currentPath()+"/data/"+fileSubPath+filePathSol);

        //MainWindow::myDB
        QString cdate = QDateTime::currentDateTime().toString("dd.MM.yyyy h:m:s ap");

        QString lessonCat = "";
        if(ui->comboBoxCat->currentText() == "جدید")
            lessonCat = ui->lineEditOther->text();
        else
            lessonCat = ui->comboBoxCat->currentText();

        QString str;

        if(itemID == -1)
        {
            if(currentQIndex == 0)
                str = "INSERT INTO `qtesti`(`id`,`grade_index`,`lesson_category`,`pic_dir`,`numb_border`,`picsol_dir`,`numbsol_border`,`date`,`state`)"
                " VALUES ("+QString::number(picId)+", "+QString::number(currentGradeIndex)+", '"+lessonCat+"', "
                " '"+fileSubPath+filePath+"', '"+numbPic+"' ,'"+fileSubPath+filePathSol+"', '"+numbSol+"' ,'"+cdate+"', 1); ";
            else if(currentQIndex == 1)
                str = "INSERT INTO `qtashrihi`(`id`,`grade_index`,`lesson_category`,`pic_dir`,`numb_border`,`picsol_dir`,`numbsol_border`,`date`,`state`)"
                " VALUES ("+QString::number(picId)+", "+QString::number(currentGradeIndex)+", '"+lessonCat+"', "
                " '"+fileSubPath+filePath+"', '"+numbPic+"' ,'"+fileSubPath+filePathSol+"', '"+numbSol+"' ,'"+cdate+"', 1); ";
            else if(currentQIndex == 2)
                str = "INSERT INTO `qsakht`(`id`,`grade_index`,`lesson_category`,`pic_dir`,`numb_border`,`picsol_dir`,`numbsol_border`,`date`,`state`)"
                " VALUES ("+QString::number(picId)+", "+QString::number(currentGradeIndex)+", '"+lessonCat+"', "
                " '"+fileSubPath+filePath+"', '"+numbPic+"' ,'"+fileSubPath+filePathSol+"', '"+numbSol+"' ,'"+cdate+"', 1); ";
            else if(currentQIndex == 3)
                str = "INSERT INTO `qhoosh`(`id`,`lesson_category`,`pic_dir`,`numb_border`,`picsol_dir`,`numbsol_border`,`date`,`state`)"
                " VALUES ("+QString::number(picId)+", '"+lessonCat+"', "
                " '"+fileSubPath+filePath+"', '"+numbPic+"' ,'"+fileSubPath+filePathSol+"', '"+numbSol+"' ,'"+cdate+"', 1); ";
        }
        else
        {
            /*str = "UPDATE `darsname` SET `grade_category` = '"+currentGrade+"' , `grade_index` = "+QString::number(currentGradeIndex)+", "
                  " `lesson_category` = '"+lessonCat+"' ,`pic_dir` = '"+filePath+"' "
                  "WHERE id = "+QString::number(itemID)+" ;";*/
            if(lastQCat != currentQIndex)
            {
                QSqlQuery squery;
                if(lastQCat == 0)
                    squery.exec("DELETE FROM qtesti WHERE id = "+QString::number(itemID)+" ");
                else if(lastQCat == 1)
                    squery.exec("DELETE FROM qtashrihi WHERE id = "+QString::number(itemID)+" ");
                else if(lastQCat == 2)
                    squery.exec("DELETE FROM qsakht WHERE id = "+QString::number(itemID)+" ");
                else if(lastQCat == 3)
                    squery.exec("DELETE FROM qhoosh WHERE id = "+QString::number(itemID)+" ");

                if(currentQIndex == 0)
                    str = "INSERT INTO `qtesti`(`id`,`grade_index`,`lesson_category`,`pic_dir`,`numb_border`,`picsol_dir`,`numbsol_border`,`date`,`state`)"
                    " VALUES ("+QString::number(picId)+", "+QString::number(currentGradeIndex)+", '"+lessonCat+"', "
                    " '"+fileSubPath+filePath+"', '"+numbPic+"' ,'"+fileSubPath+filePathSol+"', '"+numbSol+"' ,'"+cdate+"', 1); ";
                else if(currentQIndex == 1)
                    str = "INSERT INTO `qtashrihi`(`id`,`grade_index`,`lesson_category`,`pic_dir`,`numb_border`,`picsol_dir`,`numbsol_border`,`date`,`state`)"
                    " VALUES ("+QString::number(picId)+", "+QString::number(currentGradeIndex)+", '"+lessonCat+"', "
                    " '"+fileSubPath+filePath+"', '"+numbPic+"' ,'"+fileSubPath+filePathSol+"', '"+numbSol+"' ,'"+cdate+"', 1); ";
                else if(currentQIndex == 2)
                    str = "INSERT INTO `qsakht`(`id`,`grade_index`,`lesson_category`,`pic_dir`,`numb_border`,`picsol_dir`,`numbsol_border`,`date`,`state`)"
                    " VALUES ("+QString::number(picId)+", "+QString::number(currentGradeIndex)+", '"+lessonCat+"', "
                    " '"+fileSubPath+filePath+"', '"+numbPic+"' ,'"+fileSubPath+filePathSol+"', '"+numbSol+"' ,'"+cdate+"', 1); ";
                else if(currentQIndex == 3)
                    str = "INSERT INTO `qhoosh`(`id`,`lesson_category`,`pic_dir`,`numb_border`,`picsol_dir`,`numbsol_border`,`date`,`state`)"
                    " VALUES ("+QString::number(picId)+", '"+lessonCat+"', "
                    " '"+fileSubPath+filePath+"', '"+numbPic+"' ,'"+fileSubPath+filePathSol+"', '"+numbSol+"' ,'"+cdate+"', 1); ";
            }
            else
            {
                if(currentQIndex == 0)
                    str = "UPDATE `qtesti` SET `grade_index` = "+QString::number(currentGradeIndex)+", `lesson_category` = '"+lessonCat+"', "
                          "`pic_dir` = '"+fileSubPath+filePath+"', `numb_border` = '"+numbPic+"', "
                          "`picsol_dir` = '"+fileSubPath+filePathSol+"', `numbsol_border` = '"+numbSol+"' "
                          "WHERE id = "+QString::number(itemID)+" ;";
                else if(currentQIndex == 1)
                    str = "UPDATE `qtashrihi` SET `grade_index` = "+QString::number(currentGradeIndex)+", `lesson_category` = '"+lessonCat+"', "
                          "`pic_dir` = '"+fileSubPath+filePath+"', `numb_border` = '"+numbPic+"', "
                          "`picsol_dir` = '"+fileSubPath+filePathSol+"', `numbsol_border` = '"+numbSol+"' "
                          "WHERE id = "+QString::number(itemID)+" ;";
                else if(currentQIndex == 2)
                    str = "UPDATE `qsakht` SET `grade_index` = "+QString::number(currentGradeIndex)+", `lesson_category` = '"+lessonCat+"', "
                          "`pic_dir` = '"+fileSubPath+filePath+"', `numb_border` = '"+numbPic+"', "
                          "`picsol_dir` = '"+fileSubPath+filePathSol+"', `numbsol_border` = '"+numbSol+"' "
                          "WHERE id = "+QString::number(itemID)+" ;";
                else if(currentQIndex == 3)
                    str = "UPDATE `qhoosh` SET `lesson_category` = '"+lessonCat+"', "
                          "`pic_dir` = '"+fileSubPath+filePath+"', `numb_border` = '"+numbPic+"', "
                          "`picsol_dir` = '"+fileSubPath+filePathSol+"', `numbsol_border` = '"+numbSol+"' "
                          "WHERE id = "+QString::number(itemID)+" ;";
            }
        }

        QSqlQuery query;
        bool state = query.exec(str);
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
            {
                QMessageBox::information(0, qApp->tr(""),
                qApp->tr("اسلاید با موفقیت افزوده شد.")
                                    , "تایید");
                showCategory(currentGradeIndex);
            }
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
