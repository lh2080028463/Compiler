#include "graana_result.h"
#include "ui_graana_result.h"


GraAna_result::GraAna_result(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraAna_result)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().size());

    //返回switch窗口
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        emit this->back();
    });

}

GraAna_result::~GraAna_result()
{
    delete ui;
}
