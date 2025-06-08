#include "quat_result.h"
#include "ui_quat_result.h"

Quat_result::Quat_result(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Quat_result)
{
    ui->setupUi(this);

    //返回switch窗口
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        emit this->back();
    });
}

Quat_result::~Quat_result()
{
    delete ui;
}
