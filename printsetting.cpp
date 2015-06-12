#include "printsetting.h"
#include "ui_printsetting.h"

printsetting::printsetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::printsetting)
{
    ui->setupUi(this);
}

printsetting::~printsetting()
{
    delete ui;
}
