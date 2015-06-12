#ifndef PRINTSETTING_H
#define PRINTSETTING_H

#include <QDialog>

namespace Ui {
class printsetting;
}

class printsetting : public QDialog
{
    Q_OBJECT

public:
    explicit printsetting(QWidget *parent = 0);
    ~printsetting();

private:
    Ui::printsetting *ui;
};

#endif // PRINTSETTING_H
