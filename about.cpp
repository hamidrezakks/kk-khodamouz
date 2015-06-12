#include "about.h"
#include "ui_about.h"

#include <QPixmap>
#include <QDir>

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);
    QPixmap pix(QDir::currentPath()+"/image/hmstudio-png.png");
    pix = pix.scaled(ui->label_3->width(), ui->label_3->height(),Qt::KeepAspectRatio);
    ui->label_3->setPixmap(pix);
}

about::~about()
{
    delete ui;
}
