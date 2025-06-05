#include "quat_result.h"
#include "ui_quat_result.h"

Quat_result::Quat_result(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Quat_result)
{
    ui->setupUi(this);
}

Quat_result::~Quat_result()
{
    delete ui;
}
