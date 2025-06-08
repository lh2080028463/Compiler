#include "symtable.h"
#include "ui_symtable.h"

symtable::symtable(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::symtable)
{
    ui->setupUi(this);

    //返回switch窗口
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        emit this->back();
    });
}

symtable::~symtable()
{
    delete ui;
}
