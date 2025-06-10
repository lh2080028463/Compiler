#include "graana_result.h"
#include "ui_graana_result.h"


GraAna_result::GraAna_result(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraAna_result)
{
    ui->setupUi(this);

    //返回switch窗口
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        emit this->back();
    });

}

GraAna_result::~GraAna_result()
{
    delete ui;
}
